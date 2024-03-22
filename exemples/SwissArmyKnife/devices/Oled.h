#ifndef Oled_h
#define Oled_h

#define SSD1306_NO_SPLASH

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Base.h"

namespace Devices {

    class Oled : public Devices::Base , public Adafruit_SSD1306 {
        uint8_t * data;
        const static uint8_t MAX = 2;
        static uint8_t COUNT;
        uint8_t ID;
        uint16_t len  = 0;
    public :
        Oled(uint8_t addr = 0x3C)
        : Devices::Base(Devices::Base::TYPE::OLED),
            Adafruit_SSD1306 (128, 64)
        {
            ID = Oled::COUNT++;
            if (Oled::COUNT > Oled::MAX) {
                throw "TO MUCH Oled DEVICE";
            }
            if (!Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, addr)) {
                Serial.println(F("SSD1306 allocation failed"));
                throw "OLED allocation failed";
            }
            data = (uint8_t *)malloc(getBufferLen());
            memset(data, 0, getBufferLen());
        }
        virtual ~Oled() override {
            free(data);
            data = NULL;
        }
        void run() override {
            clearDisplay();
            drawBitmap( 0, 0, data, width(), height(), 1);
            display();
        }
        
        uint8_t * getData() {
            return data;
        }

        uint16_t getBufferLen() {
            if (len == 0 ) len = (width() * (height())) >> 3;
            return len;
        }
    };

    uint8_t Oled::COUNT = 0;
}
#endif /* Oled_h */