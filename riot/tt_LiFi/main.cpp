#include "thread.h"
#include "PLiFi.hpp"
#include <stdlib.h>
#include "periph/eeprom.h"
static const uint32_t DEVICE_ID_POS = 0;
uint8_t device_id;
uint8_t control_info [] = { 0x7e };
uint8_t sink1[] = { 0xd4 };
uint8_t sink2[] = { 0xd2 };
uint8_t sink3[] = { 0xd3 };
uint8_t control_data[255];
static int SEN = 2;
static int left = 11;
static int right = 10;
static int forward = 9;
static int reverse = 8;
static int right_sen = 13;
static int left_sen =12;
int state = 0;
static int sensorValue;
static char tx_stack1[THREAD_STACKSIZE_DEFAULT];
static char rx_stack1[THREAD_STACKSIZE_DEFAULT];
static char tx_stack3[THREAD_STACKSIZE_DEFAULT];
static char rx_stack3[THREAD_STACKSIZE_DEFAULT];
static char tx_stack4[THREAD_STACKSIZE_DEFAULT];
static char rx_stack4[THREAD_STACKSIZE_DEFAULT];
static char tx_stack2[THREAD_STACKSIZE_DEFAULT];
static char rx_stack2[THREAD_STACKSIZE_DEFAULT];
static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len);
static void control_message(const uint8_t *msg, uint8_t len);
static void on_message_for_me(const uint8_t *msg, uint8_t len);

PLiFi lifi1(4, 0, on_rx, NULL);
PLiFi lifi2(5, 1, on_rx, NULL);
PLiFi lifi3(6, 2, on_rx, NULL);
PLiFi lifi4(7, 3, on_rx, NULL);

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
        else {
            if (lifi != &lifi1) {
                lifi1.send_data(msg, len);
            }
            if (lifi != &lifi2) {
                lifi2.send_data(msg, len);
            }
            if (lifi != &lifi3) {
                lifi3.send_data(msg, len);
            }
            if (lifi != &lifi4) {
                lifi4.send_data(msg, len);
            }
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
        Serial.println(msg[len - 1], HEX);
        control_data[0] = msg[len - 1];
        if (control_data[0] == sink1[0]) {
            state = 1;
        }
        if (control_data[0] == sink2[0]) {
            state = 2;
        }
        if (control_data[0] == sink3[0]) {
            state = 3;
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

    thread_create(tx_stack2, sizeof(tx_stack2), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi2, "tx2");
    thread_create(rx_stack2, sizeof(rx_stack2), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi2, "rx2");
    delay(1);
    thread_create(tx_stack3, sizeof(tx_stack3), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi3, "tx3");
    thread_create(rx_stack3, sizeof(rx_stack3), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi3, "rx3");
    delay(1);
    thread_create(tx_stack4, sizeof(tx_stack4), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi4, "tx4");
    thread_create(rx_stack4, sizeof(rx_stack4), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi4, "rx4");
    delay(1);
    pinMode(SEN, INPUT_PULLUP);
    pinMode(right_sen, INPUT_PULLUP);
    pinMode(left_sen, INPUT_PULLUP);
    pinMode(right, OUTPUT);
    pinMode(left, OUTPUT);
    pinMode(forward, OUTPUT);
    pinMode(reverse, OUTPUT);
    digitalWrite(right, LOW);
    digitalWrite(left, LOW);
    digitalWrite(forward, LOW);
    digitalWrite(reverse, LOW);

    Serial.println("setup() done");
}

void loop()
{
    uint8_t msg[2];
    switch (state) {
        case 1:
            digitalWrite(forward, HIGH);
            sensorValue = digitalRead(SEN);
            if (sensorValue == 1) {
                msg[0] = control_info[0];
                msg[1] = sink1[0];
                if (lifi2.send_data(msg, sizeof(msg))) {
                    delay(10);
                }
                 digitalWrite(forward,LOW);
                 delay(1000);
                state = 4;
            }
            break;
        case 2:
            digitalWrite(forward, HIGH);
            sensorValue = digitalRead(SEN);
            if (sensorValue == 1) {
                digitalWrite(forward, LOW);
                msg[0] = control_info[0];
                msg[1] = sink2[0];
                if (lifi3.send_data(msg, sizeof(msg))) {
                    delay(10);
                }
                delay(3000);
               digitalWrite(forward,HIGH);
               delay(6000);
               digitalWrite(forward,LOW);
               delay(3000);
                state = 0;
            }
            break;
        case 3:
             digitalWrite(forward, HIGH);
            sensorValue = digitalRead(SEN);
            if (sensorValue == 1) {
                msg[0] = control_info[0];
                msg[1] = sink3[0];
                if (lifi4.send_data(msg, sizeof(msg))) {
                    delay(10);

                }

               digitalWrite(forward,LOW);
               delay(1000);
                state = 5;
            }
            break;
        case 4:
            digitalWrite(left, HIGH);
            sensorValue = digitalRead(left_sen);
            if (sensorValue == 0) {
                digitalWrite(left,LOW);
                digitalWrite(forward,HIGH);
                delay(6000);
                digitalWrite(forward,LOW);
                delay(3000);
                state = 6;
            }
            break;
        case 5:
            digitalWrite(left, HIGH);
            sensorValue = digitalRead(left_sen);
            if (sensorValue == 0) {
                digitalWrite(left,LOW);
                digitalWrite(reverse,HIGH);
                delay(6000);
                digitalWrite(reverse,LOW);
                delay(3000);
                state = 6;
            }
            break;
        case 6:
            digitalWrite(right, HIGH);
            sensorValue = digitalRead(right_sen);
            if (sensorValue == 0) {
                digitalWrite(right,LOW);
                delay(5000);
                state = 0;
            }
            break;
        default:
            sensorValue = digitalRead(right_sen);
            if (sensorValue == 0)
                delay(10);
            else{
                state = 6;
            }
            break;
    }
}
