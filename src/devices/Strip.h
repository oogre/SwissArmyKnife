#ifndef Strip_h
#define Strip_h

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class Strip : public Base {
        const static uint8_t MAX = 16;
        static uint8_t COUNT;
        uint8_t pin;
        uint8_t value;
        uint16_t pixelCount;
        Adafruit_NeoPixel * strip;
    public :
        Strip(uint8_t pixelCount, uint8_t pin)
        : pin(pin),
            Base(Base::TYPE::STRIP)
        {
            this->pixelCount = pixelCount;
            n = Strip::COUNT++;
            if (Strip::COUNT > Strip::MAX) {
                throw "TO MUCH DigitalOut DEVICE";
            }
            strip = new Adafruit_NeoPixel(pixelCount, pin, NEO_GRB + NEO_KHZ800);
            strip->begin();
        }
        virtual ~Strip() override {
            free(strip);
            strip = NULL;
        }

        void setValue(uint8_t id, uint8_t r, uint8_t g, uint8_t b, uint8_t l) {
            strip->setBrightness(l);
            strip->setPixelColor(id, strip->Color(r, g, b));
            strip->show();
        }


        virtual String toString() override {
            return Base::toString() + " " + String((int)pixelCount) + " " + String(pin) ;
        }
    };
    uint8_t Strip::COUNT = 0;
}
#endif /* Strip_h */