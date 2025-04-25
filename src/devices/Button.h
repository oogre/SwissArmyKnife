#ifndef Button_h
#define Button_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class Button : public Base {
        const static uint8_t MAX = 17;
        static uint8_t COUNT;
        uint8_t ID;
        uint8_t pin;
        uint8_t mode;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;
        RunHandler<bool> callback;
    public :
        Button(uint8_t pin, uint8_t mode, RunHandler<bool> callback)
        : pin(pin),
        mode(mode),
        callback(callback),
        Base(Base::TYPE::BUTTON)
        {
            ID = Button::COUNT++;
            if (Button::COUNT > Button::MAX) {
                throw "TO MUCH Button DEVICE";
            }
            pinMode(pin, mode); // Sets the trigPin as an Output
        }
        virtual ~Button() override {}
        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                bool value = digitalRead(pin);
                callback(this, mode == INPUT ? value : !value);
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
    };
    uint8_t Button::COUNT = 0;
}
#endif /* Button_h */