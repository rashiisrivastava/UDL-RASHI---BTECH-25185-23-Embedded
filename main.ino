#include <Arduino.h>
#include "driver/rmt.h"
#define LED_PIN            5
#define RMT_TX_CHANNEL     RMT_CHANNEL_0
#define RMT_CLK_DIV        2
#define T0H                400
#define T0L                850
#define T1H                800
#define T1L                450
#define RESET_US           60
#define NS_TO_TICKS(ns)    ((ns) / 25)
void initWS2812(uint8_t pin);
void setPixelColor(uint8_t pin, uint8_t r, uint8_t g, uint8_t b);
void setup()
{
    Serial.begin(115200);
    initWS2812(LED_PIN);
}
void loop()
{
    setPixelColor(LED_PIN, 255, 0, 0);
    delay(1000);
    setPixelColor(LED_PIN, 0, 255, 0);
    delay(1000);
    setPixelColor(LED_PIN, 0, 0, 255);
    delay(1000);
    setPixelColor(LED_PIN, 255, 255, 255);
    delay(1000);
    setPixelColor(LED_PIN, 0, 0, 0);
    delay(1000);
}
void initWS2812(uint8_t pin)
{
    rmt_config_t config = {};
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = (gpio_num_t)pin;
    config.mem_block_num = 1;
    config.clk_div = RMT_CLK_DIV;
    config.tx_config.loop_en = false;
    config.tx_config.carrier_en = false;
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    esp_err_t err;
    err = rmt_config(&config);
    if (err != ESP_OK)
    {
        Serial.println("RMT config failed");
        return;
    }
    err = rmt_driver_install(config.channel, 0, 0);
    if (err != ESP_OK)
    {
        Serial.println("RMT driver install failed");
        return;
    }
    Serial.println("WS2812 initialized");
}
void setPixelColor(uint8_t pin, uint8_t r, uint8_t g, uint8_t b)
{
    (void)pin;
    uint8_t data[3] = { g, r, b };
    rmt_item32_t items[24];
    uint8_t itemIndex = 0;
    for (uint8_t byteIndex = 0; byteIndex < 3; byteIndex++)
    {
        for (int8_t bit = 7; bit >= 0; bit--)
        {
            bool bitVal = data[byteIndex] & (1 << bit);
            if (bitVal)
            {
                items[itemIndex].level0 = 1;
                items[itemIndex].duration0 = NS_TO_TICKS(T1H);
                items[itemIndex].level1 = 0;
                items[itemIndex].duration1 = NS_TO_TICKS(T1L);
            }
            else
            {
                items[itemIndex].level0 = 1;
                items[itemIndex].duration0 = NS_TO_TICKS(T0H);
                items[itemIndex].level1 = 0;
                items[itemIndex].duration1 = NS_TO_TICKS(T0L);
            }
            itemIndex++;
        }
    }
    rmt_write_items(RMT_TX_CHANNEL, items, 24, true);
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    delayMicroseconds(RESET_US);
}