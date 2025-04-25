#ifndef DigitalOut_h
#define DigitalOut_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class DigitalOut : public Base {
        const static uint8_t MAX = 16;
        static uint8_t COUNT;
        uint8_t pin;
        uint8_t value;
    public :
        DigitalOut(uint8_t pin)
        : pin(pin),
            Base(Base::TYPE::DIGITAL_OUT)
        {
            n = DigitalOut::COUNT++;
            if (DigitalOut::COUNT > DigitalOut::MAX) {
                throw "TO MUCH DigitalOut DEVICE";
            }
            pinMode(pin, OUTPUT);
        }
        virtual ~DigitalOut() override {}
        void setValue(bool value) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, value);
            this->value = value;
        }
        uint8_t getValue() {
            return value;
        }
    };
    uint8_t DigitalOut::COUNT = 0;
}
#endif /* DigitalOut_h */