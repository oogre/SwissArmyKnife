#ifndef AnalogIn_h
#define AnalogIn_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class AnalogIn : public Base {
        const static uint8_t MAX = 4;
        static uint8_t COUNT;
        uint8_t ID;
        uint8_t pin;
        RunHandler<uint16_t> callback;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;
    public :
        AnalogIn(uint8_t pin, RunHandler<uint16_t> callback)
        : pin(pin),
            callback(callback),
            Base(Base::TYPE::ANALOG_IN)
        {
            ID = AnalogIn::COUNT++;
            if (AnalogIn::COUNT > AnalogIn::MAX) {
                throw "TO MUCH AnalogIn DEVICE";
            }
        }
        virtual ~AnalogIn() override {}
        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                callback(this, analogRead(pin));
                lastReadAt = millis();
                Base::run();
            }
        }
        virtual uint8_t getID(bool deep = false) override {
            if(deep) return Base::getID(deep);
            return ID;
        }
    };
    uint8_t AnalogIn::COUNT = 0;

}
#endif /* AnalogIn_h */