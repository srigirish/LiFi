#include <string.h>
#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN 53
#define RST_PIN 49
#include "thread.h"
#include "PLiFi.hpp"
#include <stdlib.h>
#include "periph/eeprom.h"
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
static int  s1_g=6;
static int  s2_g=8;
static int  s3_g=10;
static int  s1_y=5;
static int  s2_b=7;
static int  s3_r=9;
uint8_t acc[10];
static const uint32_t DEVICE_ID_POS = 0;
static int state=0;
uint8_t device_id;
uint8_t control_info []={0x7e};
uint8_t sink1[]={0xd4};
uint8_t sink2[]={0xd2};
uint8_t sink3[]={0xd3};
static char tx_stack1[THREAD_STACKSIZE_MAIN];
static char rx_stack1[THREAD_STACKSIZE_MAIN];

static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len);

static void on_message_for_me(const uint8_t *msg, uint8_t len);

PLiFi lifi1(4, 0, on_rx, NULL);

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
        //set state
        acc[0]=msg[len-1];
            if(acc[0]==0xf1)
                state=1;
            if(acc[0]==0xf2)
                state=2;
            if(acc[0]==0xf3)
                state=3;
    }
    else {
        Serial.println("Got: Empty message");
    }
}
static const uint8_t sink1_1[] = {0x3E, 0xF1, 0xBB, 0xDF};
static const uint8_t sink1_2[] = {0xC5, 0x1D, 0xF7, 0x58};
static const uint8_t sink2_1[] = {0x74, 0x31, 0xDC, 0xE9};
static const uint8_t sink2_2[] = {0x95, 0xFB, 0xF8, 0x58};
static const uint8_t sink3_1[] = {0x74, 0x8E, 0xE1, 0xE9};
static const uint8_t sink3_2[] = {0x54, 0xE6, 0xB6, 0xEB};
void setup(){
    SPI.begin();       // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    Serial.println("RFID reading UID");
    device_id = eeprom_read_byte(DEVICE_ID_POS);
    Serial.print("Device ID: 0x");
    Serial.println(device_id, HEX);

    thread_create(tx_stack1, sizeof(tx_stack1), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi1, "tx1");
    thread_create(rx_stack1, sizeof(rx_stack1), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi1, "rx1");
    delay(1);
       pinMode(s1_y,OUTPUT);
       pinMode(s1_g,OUTPUT);
       pinMode(s2_b,OUTPUT);
       pinMode(s2_g,OUTPUT);
       pinMode(s3_r,OUTPUT);
       pinMode(s3_g,OUTPUT);
        digitalWrite(s1_y,LOW);
        digitalWrite(s1_g,LOW);
        digitalWrite(s2_b,LOW);
        digitalWrite(s2_g,LOW);
        digitalWrite(s3_r,LOW);
        digitalWrite(s3_g,LOW);
    Serial.println("setup() done");
}

void loop() {
    uint8_t msg[2];
if ( mfrc522.PICC_IsNewCardPresent())
    {
        if ( mfrc522.PICC_ReadCardSerial())
        {
            Serial.print("Tag UID:");
            for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);
            }
            Serial.println();
            if (
                    ((sizeof(sink1_1) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink1_1, sizeof(sink1_1))) ||
                    ((sizeof(sink1_2) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink1_2, sizeof(sink1_2)))
                ){
                 digitalWrite(s1_y,HIGH);
                 Serial.println("sink1");
                 msg[0]=control_info[0];
                 msg[1]=sink1[0];
                 if(lifi1.send_data(msg, sizeof(msg)))
                     delay(10);
            }
            if (
                    ((sizeof(sink2_1) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink2_1, sizeof(sink2_1))) ||
                    ((sizeof(sink2_2) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink2_2, sizeof(sink2_2)))
                ){
                 digitalWrite(s2_b,HIGH);
                 Serial.println("sink2");
                 msg[0]=control_info[0];
                 msg[1]=sink2[0];
                 if(lifi1.send_data(msg, sizeof(msg)))
                     delay(10);
            }
            if (
                    ((sizeof(sink3_1) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink3_1, sizeof(sink3_1))) ||
                    ((sizeof(sink3_2) == mfrc522.uid.size) && !memcmp(mfrc522.uid.uidByte, sink3_2, sizeof(sink3_2)))
                ){
                 digitalWrite(s3_r,HIGH);
                 Serial.println("sink3");
                 msg[0]=control_info[0];
                 msg[1]=sink3[0];
                 if(lifi1.send_data(msg, sizeof(msg)))
                     delay(10);
            }
            delay(10000);
        }
    }
//received msg
//if rx from sink1 led green 1 off...
        if(state==1)
        {
            digitalWrite(s1_y,LOW);
            digitalWrite(s1_g,HIGH);
            delay(5000);
            digitalWrite(s1_g,LOW);
            state=0;
        }
        if(state==2)
        {
            digitalWrite(s2_b,LOW);
            digitalWrite(s2_g,HIGH);
            delay(5000);
            digitalWrite(s2_g,LOW);
            state=0;
        }
        if(state==3)
        {
            digitalWrite(s3_r,LOW);
            digitalWrite(s3_g,HIGH);
            delay(5000);
            digitalWrite(s3_g,LOW);
            state=0;
        }
}
