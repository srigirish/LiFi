#include "thread.h"
#include "PLiFi.hpp"
#include <stdlib.h>
#include "periph/eeprom.h"

static const int sink1= 13;
static const int sink2= 12;
static const int sink3= 3;
static const uint32_t DEVICE_ID_POS = 0;
uint8_t device_id;
uint8_t master_id[]={0xff};
uint8_t si1[]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
uint8_t si2[]={0xbb};
uint8_t si3[]={0xcc};

static char tx_stack1[THREAD_STACKSIZE_MAIN];
static char rx_stack1[THREAD_STACKSIZE_MAIN];
static char tx_stack2[THREAD_STACKSIZE_MAIN];
static char rx_stack2[THREAD_STACKSIZE_MAIN];
static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len);

static void on_message_for_me(const uint8_t *msg, uint8_t len);

PLiFi lifi1(4, 0, on_rx, NULL);
PLiFi lifi2(5, 1, on_rx, NULL);

void * tx_loop(void *arg) {
    PLiFi *lifi = (PLiFi *)arg;

    lifi->tx_loop();
    return NULL;
}

void * rx_loop(void *arg) {
    PLiFi *lifi = (PLiFi *)arg;
    lifi->rx_loop();
    return NULL;
}

static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len)
{
    if (len >= 2) {
        if (msg[0] == device_id) {
            on_message_for_me(msg + 2, len - 2);
        }
        else {
            if (lifi != &lifi1) {
                lifi1.send_data(msg, len);
            }
            if (lifi != &lifi2) {
                lifi2.send_data(msg, len);
            }
        }
    }
}

static void on_message_for_me(const uint8_t *msg, uint8_t len)
{
    if (len > 0) {
        Serial.print("Got: 0x");
        for (uint8_t i = 0; i < len - 1; i++) {
            Serial.print(msg[i], HEX);
            Serial.print(", 0x");
        }
        Serial.println(msg[len-1], HEX);
    }
    else {
        Serial.println("Got: Empty message");
    }
}

void setup(){
    device_id = eeprom_read_byte(DEVICE_ID_POS);
    Serial.print("Device ID: 0x");
    Serial.println(device_id, HEX);

    thread_create(tx_stack1, sizeof(tx_stack1), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi1, "tx1");
    thread_create(rx_stack1, sizeof(rx_stack1), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi1, "rx1");
    delay(1);

    thread_create(tx_stack2, sizeof(tx_stack2), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi2, "tx2");
    thread_create(rx_stack2, sizeof(rx_stack2), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi2, "rx2");
    delay(1);


       pinMode(sink1, INPUT_PULLUP);
       pinMode(sink2, INPUT_PULLUP);
       pinMode(sink3, INPUT_PULLUP);

    Serial.println("setup() done");
}

void loop() {

    uint8_t msg[8];
    int val1=digitalRead(sink1);
    int val2=digitalRead(sink2);
    int val3=digitalRead(sink3);
        if(val1==0)
            {
              //send sink 1 process data to neiboughr node and master
                msg[0]=master_id[0];
                msg[1]=device_id;
                msg[2]=si1[0];
                msg[3]=si1[1];
                msg[4]=si1[2];
                msg[5]=si1[3];
                msg[6]=si1[4];
                msg[7]=si1[5];
           if(lifi1.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi1 failed");
            if(lifi2.send_data(si1, sizeof(si1)))
                Serial.println("TX on lifi2 failed");

               }

        if(val2==0)
            {
              //send sink 1 process data to neiboughr node and master
                msg[0]=master_id[0];
                msg[1]=device_id;
                msg[2]=si2[0];
            if(lifi1.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi1 failed");
            if(lifi2.send_data(si2, sizeof(si2)))
                Serial.println("TX on lifi2 failed");

               }

        if(val3==0)
            {
              //send sink 1 process data to neiboughr node and master
                msg[0]=master_id[0];
                msg[1]=device_id;
                msg[2]=si3[0];
           if(lifi1.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi1 failed");
            if(lifi2.send_data(si3, sizeof(si3)))
                Serial.println("TX on lifi2 failed");

               }
            delay(100);
        }


