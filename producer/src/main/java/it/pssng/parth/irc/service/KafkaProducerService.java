package it.pssng.parth.irc.service;

import org.apache.kafka.clients.producer.ProducerRecord;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.stereotype.Service;

import it.pssng.parth.irc.view.Message;
import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class KafkaProducerService {
    
    private Logger LOG = LoggerFactory.getLogger(getClass());
    private final KafkaTemplate<String,String> kafkaTemplate;

public void sendMessageWithHeader(Message message){
    ProducerRecord<String, String> producerRecord = new ProducerRecord<>("irc-support", null, message.getMessageBody());

    byte[] userIdValueBytes = Long.toString(message.getUserId()).getBytes();

    producerRecord.headers().add("user-id", userIdValueBytes);
    kafkaTemplate.send(producerRecord);
    
    LOG.info("Message sent with header");
}

}
