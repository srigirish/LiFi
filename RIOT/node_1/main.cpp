#include "thread.h"
#include "PLiFi.hpp"
#include <stdlib.h>
#include "periph/eeprom.h"

static const int pins[] = {8, 9, 10, 11};
static const uint32_t DEVICE_ID_POS = 0;
uint8_t device_id;

static char tx_stack1[THREAD_STACKSIZE_MAIN];
static char rx_stack1[THREAD_STACKSIZE_MAIN];
static char tx_stack2[THREAD_STACKSIZE_MAIN];
static char rx_stack2[THREAD_STACKSIZE_MAIN];
static char tx_stack3[THREAD_STACKSIZE_MAIN];
static char rx_stack3[THREAD_STACKSIZE_MAIN];
static char tx_stack4[THREAD_STACKSIZE_MAIN];
static char rx_stack4[THREAD_STACKSIZE_MAIN];
static void on_rx(PLiFi *lifi, const uint8_t *msg, uint8_t len);

static void on_message_for_me(const uint8_t *msg, uint8_t len);

PLiFi lifi1(4, 0, on_rx, NULL);
PLiFi lifi2(5, 1, on_rx, NULL);
PLiFi lifi3(6, 2, on_rx, NULL);
PLiFi lifi4(7, 3, on_rx, NULL);

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
            if (lifi != &lifi3) {
                lifi3.send_data(msg, len);
            }
            if (lifi != &lifi4) {
                lifi4.send_data(msg, len);
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

    thread_create(tx_stack3, sizeof(tx_stack3), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi3, "tx3");
    thread_create(rx_stack3, sizeof(rx_stack3), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi3, "rx3");
    delay(1);

    thread_create(tx_stack4, sizeof(tx_stack4), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, tx_loop, &lifi4, "tx4");
    thread_create(rx_stack4, sizeof(rx_stack4), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, rx_loop, &lifi4,"rx4");

    for (uint8_t i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
        pinMode(pins[i], INPUT_PULLUP);
    }
    Serial.println("setup() done");
}

void loop() {

    static const uint8_t dests[] = {0x0a, 0x01, 0x42, 0x30};
    uint8_t msg[3];

    for (uint8_t i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
        if (!digitalRead(pins[i])) {
            Serial.print("Pin ");
            Serial.print(pins[i]);
            Serial.println(" pressed");
            msg[0] = dests[i];
            msg[1] = device_id;
            msg[2] = (uint8_t)pins[i];
            if (lifi1.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi1 failed");
            if (lifi2.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi2 failed");
            if (lifi3.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi3 failed");
            if (lifi4.send_data(msg, sizeof(msg)))
                Serial.println("TX on lifi4 failed");
            delay(100);
        }
    }
}
