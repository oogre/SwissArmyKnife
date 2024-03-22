#ifndef UltraSonic_h
#define UltraSonic_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class UltraSonic : public Base {
        const static float SOUND_SPEED;
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
        uint8_t ID;
        uint8_t trig;
        uint8_t echo;
        RunHandler<float> callback;
    public :
        UltraSonic(uint8_t trig, uint8_t echo, RunHandler<float> callback)
        : trig(trig),
            echo(echo),
            callback(callback),
            Base(Base::TYPE::ULTRA_SONIC)
        {
            ID = UltraSonic::COUNT++;
            if (UltraSonic::COUNT > UltraSonic::MAX) {
                throw "TO MUCH ULTRASONIC DEVICE";
            }
            pinMode(trig, OUTPUT); // Sets the trigPin as an Output
            pinMode(echo, INPUT); // Sets the echoPin as an Input
        }
        virtual ~UltraSonic() override {}
        virtual void run() override {
            digitalWrite(trig, LOW);
            delayMicroseconds(2);
            digitalWrite(trig, HIGH);
            delayMicroseconds(10);
            digitalWrite(trig, LOW);
            long duration = pulseIn(echo, HIGH);
            callback(this, duration * SOUND_SPEED * 0.5f);
            Base::run();
        }
    };
    uint8_t UltraSonic::COUNT = 0;
    const float UltraSonic::SOUND_SPEED = 0.034f;
}

#endif /* UltraSonic_h */