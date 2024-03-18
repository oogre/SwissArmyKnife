#ifndef Touch_h
#define Touch_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class Touch : public Base {
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
        uint8_t ID;
        uint8_t pin;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;
        RunHandler<uint16_t> callback;
    public :
        Touch(uint8_t pin, RunHandler<uint16_t> callback)
        : pin(pin),
            callback(callback),
            Base(Base::TYPE::TOUCH)
        {
            ID = Touch::COUNT++;
            if (Touch::COUNT > Touch::MAX) {
                throw "TO MUCH Touch DEVICE";
            }
        }
        virtual ~Touch() override {}
        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                callback(this, touchRead(pin));
                lastReadAt = millis();
                Base::run();
            }
        }
    };
    uint8_t Touch::COUNT = 0;
}
#endif /* Touch_h */