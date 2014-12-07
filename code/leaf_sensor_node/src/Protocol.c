#include <stdint.h>
//#include "MyRingBuffer.h"
#include "Protocol.h"

//TODO add flush to uartRxBuffer

void Protocol_init(struct Protocol* obj, uint8_t port){

   obj->byteNumber = 0;
   obj->cmd = 0;
   obj->addr = 0;
   obj->data = 0;
   obj->checksum = 0;
   obj->return_data = 0;
   obj->executePacket = 0;

   obj->port = port;
   obj->special = 0;

}

/*
uint8_t Protocol_respond(struct Protocol obj){
   if(Serial1.available()){
      uint8_t rxBuffer[5];
      uint8_t response[5];
      uint8_t index = 0;
      uint8_t i = 0;
      uint8_t sendResponse = 0;

      while(Serial1.available() < 2); //wait for 2 bytes
      rxBuffer[index++] = Serial1.read(); //cmd
      rxBuffer[index++] = Serial1.read(); //size

      for(i=0; i<rxBuffer[1]; i++){ //get rest of bytes
         while(Serial1.available() == 0); //wait for a byte
         rxBuffer[index++] = Serial1.read();
      }

      while(Serial1.available() == 0); //wait for checksum
      rxBuffer[index++] = Serial1.read();

      //parse packet
      sendResponse = parse_packet(rxBuffer, response);

      if(sendResponse == 1){
         for(i=0; i<response[1]+3; i++){
            _uartTxBuffer.write(response[i]);
         }
         uartTx();
      }
      return 1;
   }
   return 0;
}*/

/*
void Protocol_transmit(struct Protocol obj, uint8_t cmd, uint8_t addr, uint8_t data){

   uint8_t response[5];
   form_packet(response, cmd, addr, data);
   for(uint8_t i=0; i<response[1]+3; i++){
      obj.uartTxBuffer.write(response[i]);
   }
   uartTx();

}*/

uint8_t Protocol_parse_packet(struct Protocol* obj, uint8_t* buf, uint8_t* response){
   uint8_t returnCode = 0;
   obj->cmd = buf[0];
   obj->size = buf[1];
   obj->checksum = obj->cmd + obj->size;
   uint8_t i=0;
   for(i=0; i<obj->size; i++){
      obj->payload[i] = buf[2+i];
      obj->checksum += obj->payload[i];
   }
   obj->checksum += buf[2+obj->size]; //get last byte, the checksum

   if(obj->checksum == 0){//checksum matches
      obj->executePacket = 1;
   }else{ //bad checksum
      Protocol_form_packet(response, CMD_NACK, 0, ERR_CHECKSUM);
      returnCode = 1;
      obj->executePacket = 0;
      obj->byteNumber = 0;
      obj->checksum = 0;
   }

   if(obj->executePacket == 1){
      obj->executePacket = 0;
      obj->checksum = 0;
      switch(obj->cmd){
         case(CMD_READ_REG): //read_reg
            obj->addr = obj->payload[0];

            Protocol_form_packet(response, CMD_ACK, obj->addr, obj->dataRegisters[obj->addr]);
            returnCode = 1;
            break;

         case(CMD_WRITE_REG):
            //Serial.println("Got write_reg");
            obj->addr = obj->payload[0];
            obj->data = obj->payload[1];
            obj->dataRegisters[obj->addr] = obj->data; //write value

            Protocol_form_packet(response, CMD_ACK, obj->addr, obj->dataRegisters[obj->addr]);
            returnCode = 1;
            break;

         case(CMD_ACK): //ack
            //Serial.println("Got ack");
            //Serial.print("Addr = ");Serial.println(_remainderBuf[0]);
            //Serial.print("Data = ");Serial.println(_remainderBuf[1]);
            //Serial.print("\n");
            returnCode = 0;
            break;

         case(CMD_NACK): //nack
            //Serial.println("Got nack");
            //Serial.print("Addr = ");Serial.println(_remainderBuf[0]);
            //Serial.print("Data = ");Serial.println(_remainderBuf[1]);
            //Serial.print("\n");
            returnCode = 0;
            break;

         case(CMD_NOP): //nop
            //Serial.println("Got nop");
            obj->special = 1;
            returnCode = 0;
            break;

         default: //unknown command
            //Serial.println("Got unknown command. Send nack");
            Protocol_form_packet(response, CMD_NACK, 0, ERR_COMMAND);
            returnCode = 1;
            break;
      }//end switch
   }//end if executePacket==1
   return returnCode;
}//end parse_packet

uint8_t Protocol_form_packet(uint8_t* buf, uint8_t cmd, uint8_t addr, uint8_t data){
   uint8_t index = 0;
   uint8_t checksum = cmd;

   buf[index++] = cmd;

   if(cmd == CMD_READ_REG){
      buf[index++] = (1);
      buf[index++] = (addr);

      checksum += 1;
      checksum += addr;

   }else if(cmd == CMD_WRITE_REG){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == CMD_ACK){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == CMD_NACK){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == CMD_NOP){
      buf[index++] = ((uint8_t)0);

   }else{//unknown command
      return 0;
   }

   buf[index++] = ((~checksum)+1);

   return 1;
}

/*
void Protocol_uartTx(struct Protocol obj){

   uint8_t temp;
   while(_uartTxBuffer.read(&temp)){
//         _channel->write(temp);
         Serial1.write(temp);
   }
}*/
