#ifndef AnalogIn_h
#define AnalogIn_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class AnalogIn : public Base {
        const static uint8_t MAX = 4;
        static uint8_t COUNT;
        uint8_t pin;
        uint16_t oldValue = -1;
        RunHandler<uint16_t> callback;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 0;
    public :
        AnalogIn(uint8_t pin, RunHandler<uint16_t> callback)
        : pin(pin),
            callback(callback),
            Base(Base::TYPE::ANALOG_IN)
        {
            n = AnalogIn::COUNT++;
            if (AnalogIn::COUNT > AnalogIn::MAX) {
                throw "TO MUCH AnalogIn DEVICE";
            }
        }
        virtual ~AnalogIn() override {}
        virtual void run() override {
            if(!_active)return;
            if (millis() - lastReadAt > readDelay) {
                uint16_t value = analogRead(pin);
                if(value != oldValue){
                    callback(this, value);
                }
                oldValue = value;
                lastReadAt = millis();
                Base::run();
            }
        }
        void setDelay(uint32_t time){
            readDelay = time;
        }
        uint32_t getDelay(){
            return readDelay;
        }

        virtual String toString() override {
            return Base::toString() + " " + String((int)pin) + " " + String(readDelay) + " " + String(_active) ;
        }
    };
    uint8_t AnalogIn::COUNT = 0;

}
#endif /* AnalogIn_h */