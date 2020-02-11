#include "thread.h"
#include "PLiFi.hpp"
#include <stdlib.h>
#include "periph/eeprom.h"
static const uint32_t DEVICE_ID_POS = 0;
uint8_t master_id[] = { 0xff };
uint8_t control_info [] = { 0x7e };
uint8_t device_id;
uint8_t sink1[] = { 0xf1 };
uint8_t control_data[255];
static int SEN1 = 2;
int SEN2 = 3;
int forward = 8;
int reverse = 9;
int state = 0;
int sensorValue;
static char tx_stack1[THREAD_STACKSIZE_MAIN];
static char rx_stack1[THREAD_STACKSIZE_MAIN];

static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len);

static void control_message(const uint8_t *msg, uint8_t len);
static void on_message_for_me(const uint8_t *msg, uint8_t len);

PLiFi lifi1(4, 0, on_rx, NULL);

void *tx_loop(void *arg)
{
    PLiFi *lifi = (PLiFi *)arg;

    lifi->tx_loop();
    return NULL;
}

void *rx_loop(void *arg)
{
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
        else if (msg[0] == control_info[0]) {
            control_message(msg + 1, len - 1);
        }
        else
        if (lifi != &lifi1) {
            lifi1.send_data(msg, len);
        }
    }
}
static void control_message(const uint8_t *msg, uint8_t len)
{
    if (len > 0) {
        Serial.print("Got: 0x");
        for (uint8_t i = 0; i < len - 1; i++) {
            Serial.print(msg[i], HEX);
            Serial.print(", 0x");
        }
        Serial.print(msg[len - 1], HEX);

        control_data[0] = msg[len - 1];
        if (control_data[0] == 0xd4) {
            state = 1;
        }
    }
    else {
        Serial.println("Got: Empty message");
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
        Serial.println(msg[len - 1], HEX);
    }
    else {
        Serial.println("Got: Empty message");
    }
}


void setup()
{
    device_id = eeprom_read_byte(DEVICE_ID_POS);
    Serial.print("Device ID: 0x");
    Serial.println(device_id, HEX);

    thread_create(tx_stack1, sizeof(tx_stack1), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi1, "tx1");
    thread_create(rx_stack1, sizeof(rx_stack1), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi1, "rx1");
    delay(1);
    pinMode(SEN1, INPUT_PULLUP);
    pinMode(SEN2, INPUT_PULLUP);
    pinMode(forward, OUTPUT);
    pinMode(reverse, OUTPUT);
    digitalWrite(forward, LOW);
    digitalWrite(reverse, LOW);
    Serial.begin(9600);
    state = 0;

    Serial.println("setup() done");
}

void loop()
{

//forward and check for sensor 2 stop the conveyor and send back accknoladgement to master
    if (state == 1) {
        digitalWrite(forward, HIGH);
        sensorValue = digitalRead(SEN1);
         if (sensorValue == 1) {
            uint8_t msg[3];
            msg[0] = master_id[0];
            msg[1] = device_id;
           msg[2] = sink1[0];
            if (lifi1.send_data(msg, sizeof(msg))) {
                Serial.println("tx");
            }
            digitalWrite(forward, LOW);

            state = 0;
            delay(1000);
            //send data to master
        }
    }

//control_data[0]={0x00};
    else {
//do nothing
//        Serial.println("state 0");
    }
}
