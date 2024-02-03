package it.pssng.parth.irc.controller;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import it.pssng.parth.irc.service.KafkaProducerService;
import it.pssng.parth.irc.view.Message;
import lombok.RequiredArgsConstructor;

@RestController
@RequiredArgsConstructor
public class ProducerAPI {
    
    private final KafkaProducerService kafkaProducerService;

    @PostMapping("/send")
    public ResponseEntity<Void> sendMessage(@RequestBody Message message){
        kafkaProducerService.sendMessageWithHeader(message);
        return ResponseEntity.ok().build();
    }



}
