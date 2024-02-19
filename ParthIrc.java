public class ParthIrc {

    static{
        System.load("/root/test/libnative.dylib");
    }

    public static void main(String []args){
        // new HelloWorldJNI().sayHello();
        new ParthIrc().startServer(1846);

    }

    private native void broadcast(char message, int channel, int senderSocket);
    private native void handleClient(int index);
    private native void startServer(int serverPort);
    private native int closeServer();

}
