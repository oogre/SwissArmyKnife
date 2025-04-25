#ifndef Stepper_h
#define Stepper_h

#define STEPPER_MIN_DELAY 3
#define STEPPER_MAX_DELAY 20

#include <Arduino.h>
#include <DHT.h>
#include "Base.h"

namespace Devices {

    class Stepper : public Base {
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
        uint8_t step_number = 0;
        uint8_t STEPPER_PIN_1 = 255;
        uint8_t STEPPER_PIN_2 = 255;
        uint8_t STEPPER_PIN_3 = 255;
        uint8_t STEPPER_PIN_4 = 255;
        bool TWO_PIN_CONTROL = false;
        bool dir = 0;
        long dist = 0;

        uint32_t lastReadAt = 0;
        uint32_t stepDelay = 100;
    public :
        Stepper(uint8_t PIN_1, uint8_t PIN_2, uint8_t PIN_3 = 0, uint8_t PIN_4 = 0)
        : STEPPER_PIN_1(PIN_1),
            STEPPER_PIN_2(PIN_2),
            STEPPER_PIN_3(PIN_3),
            STEPPER_PIN_4(PIN_4),
            TWO_PIN_CONTROL(PIN_3 == 0 || PIN_4 == 0),
            Base(Base::TYPE::STEPPER)
        {
            pinMode(STEPPER_PIN_1, OUTPUT);
            pinMode(STEPPER_PIN_2, OUTPUT);
            if (!TWO_PIN_CONTROL) pinMode(STEPPER_PIN_3, OUTPUT);
            if (!TWO_PIN_CONTROL) pinMode(STEPPER_PIN_4, OUTPUT);

            n = Stepper::COUNT++;
            if (Stepper::COUNT > Stepper::MAX) {
                throw "TO MUCH Stepper DEVICE";
            }
        }

        virtual ~Stepper() override {}
        virtual void run() override {
            if (millis() - lastReadAt > stepDelay) {
                if (dist > 0) {
                    OneStep(dir);
                    dist--;
                }
                lastReadAt = millis();
                Base::run();
            }
        }
        void setDist(int value) {
            dir = value > 0;
            dist = abs(value);
        }

        void setSpeed(int value) {
            value = 100 - abs(value);
            stepDelay = int(STEPPER_MIN_DELAY + (value * 0.01f) * (STEPPER_MAX_DELAY - STEPPER_MIN_DELAY));
            stepDelay = min(STEPPER_MAX_DELAY, max(STEPPER_MIN_DELAY, (int)stepDelay));
        }

        long getDist() {
            return dist;
        }

        int getSpeed() {
            return stepDelay;
        }

    private :
        void OneStep(bool dir) {
            if (TWO_PIN_CONTROL) {
                //        Serial.println(dist);
                digitalWrite(STEPPER_PIN_1, dir ? HIGH : LOW);
                digitalWrite(STEPPER_PIN_2, HIGH);
                delayMicroseconds(stepDelay);
                digitalWrite(STEPPER_PIN_2, LOW);
                delayMicroseconds(stepDelay);
                return;
            }
            if (dir) {
                
                switch (step_number) {
                case 0:
                    digitalWrite(STEPPER_PIN_1, HIGH);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, LOW);
                    break;
                case 1:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, HIGH);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, LOW);
                    break;
                case 2:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, HIGH);
                    digitalWrite(STEPPER_PIN_4, LOW);
                    break;
                case 3:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, HIGH);
                    break;
                }
            } else {
                switch (step_number) {
                case 0:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, HIGH);
                    break;
                case 1:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, HIGH);
                    digitalWrite(STEPPER_PIN_4, LOW);
                    break;
                case 2:
                    digitalWrite(STEPPER_PIN_1, LOW);
                    digitalWrite(STEPPER_PIN_2, HIGH);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, LOW);
                    break;
                case 3:
                    digitalWrite(STEPPER_PIN_1, HIGH);
                    digitalWrite(STEPPER_PIN_2, LOW);
                    digitalWrite(STEPPER_PIN_3, LOW);
                    digitalWrite(STEPPER_PIN_4, LOW);
                }
            }
            delay(stepDelay);
            step_number++;
            step_number %= 4;
        }
    };
    uint8_t Stepper::COUNT = 0;
}
#endif /* Stepper_h */