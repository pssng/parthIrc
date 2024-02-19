#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <jni.h>

#define MAX_CLIENTS 10
#define MAX_CHANNELS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char username[BUFFER_SIZE];
    int channel;
} Client;

Client clients[MAX_CLIENTS];
char channels[MAX_CHANNELS][BUFFER_SIZE];
int numClients = 0;
int numChannels = 0;
int serverSocket;

/*
 * Serve ad inviare il testo a tutti i client.
 */
JNIEXPORT void JNICALL Java_ParthIrc_broadcast(JNIEnv *env, jobject obj, jint senderSocket, jstring message, jint channel) {
    const char *cMessage = (*env)->GetStringUTFChars(env, message, NULL);

    for (int i = 0; i < numClients; ++i) {
        if (clients[i].socket != senderSocket && clients[i].channel == channel) {
            send(clients[i].socket, cMessage, strlen(cMessage), 0);
        }
    }

    (*env)->ReleaseStringUTFChars(env, message, cMessage);
}


/*
 * Serve a trattare il nuovo client collegato. L'index identifica l'utente
 */
JNIEXPORT void JNICALL Java_ParthIrc_handleClient(JNIEnv *, jobject, jint index)
 {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    while (true) {
        /*
         * recv serve a ricevere i dati da una socket sottoforma di byte
         */
        int bytesRead = recv(clients[index].socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            printf("Client disconnected: %s\n", clients[index].username);
            close(clients[index].socket); //chiude la connessione

            // Rimuove il client dalla lista
            for (int i = index; i < numClients - 1; ++i) {
                clients[i] = clients[i + 1];
            }
            numClients--;

            return;
        }

        buffer[bytesRead] = '\0';

        // Check if the client is changing the channel
        // Broadcast the message to all clients in the same channel
        sprintf(message, "%s: %s", clients[index].username, buffer);
        broadcast(message, clients[index].channel, clients[index].socket);
    }
}


JNIEXPORT void JNICALL Java_ParthIrc_startServer(JNIEnv *, jobject, jint serverPort) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Parth IRC Server is running. Waiting for clients...\n");

    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            perror("Accepting client failed");
            exit(EXIT_FAILURE);
        }

        if (numClients < MAX_CLIENTS) {
            Client newClient;
            newClient.socket = clientSocket;
            sprintf(newClient.username, "User%d", numClients + 1);
            newClient.channel = 0;

            clients[numClients++] = newClient;

            printf("New client connected: %s\n", newClient.username);

            send(clientSocket, "Welcome to the IRC server!\n", 26, 0);

            // Create a default channel if none exists
            if (numChannels == 0) {
                strcpy(channels[0], "General");
                numChannels++;
            }

            // Send channel list to the new client
            char channelList[BUFFER_SIZE];
            strcpy(channelList, "Available channels:\n");
            for (int i = 0; i < numChannels; ++i) {
                sprintf(channelList + strlen(channelList), "%d. %s\n", i, channels[i]);
            }
            send(clientSocket, channelList, strlen(channelList), 0);

            // Start a new thread to handle the client
            if (fork() == 0) {
                handleClient(numClients - 1);
                exit(0);
            }
        } else {
            send(clientSocket, "Server full. Try again later.\n", 30, 0);
            close(clientSocket);
        }
    }

}

JNIEXPORT jint JNICALL Java_ParthIrc_closeServer(JNIEnv *, jobject) {
    return close(serverSocket);
}


