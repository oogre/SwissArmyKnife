#ifndef Button_h
#define Button_h

#include <Arduino.h>
#include "Base.h"
#include <Wire.h>
#include "dependencies/MMA7660/MMA7660.h"



namespace Devices {

    class Accelerometer : public Base {
        const static uint8_t MAX = 17;
        static uint8_t COUNT;
        uint8_t ID;
        RunHandler<bool> callback;
        MMA7660 accelemeter;
    public :
        Accelerometer(RunHandler<bool> callback)
        : 
        callback(callback),
        Base(Base::TYPE::ACCELEROMETER)
        {
            ID = Accelerometer::COUNT++;
            if (Accelerometer::COUNT > Accelerometer::MAX) {
                throw "TO MUCH Accelerometer DEVICE";
            }
            accelemeter.init();
        }
        virtual ~Accelerometer() override {}
        virtual void run() override {
            int8_t x;
            int8_t y;
            int8_t z;
            float ax, ay, az;
            accelemeter.getXYZ(&x, &y, &z);

            Serial.print("x = ");
            Serial.println(x);
            Serial.print("y = ");
            Serial.println(y);
            Serial.print("z = ");
            Serial.println(z);

            accelemeter.getAcceleration(&ax, &ay, &az);
            Serial.println("accleration of X/Y/Z: ");
            Serial.print(ax);
            Serial.println(" g");
            Serial.print(ay);
            Serial.println(" g");
            Serial.print(az);
            Serial.println(" g");
            Serial.println("*************");
            // if (millis() - lastReadAt > readDelay) {
            //     bool value = digitalRead(pin);
            //     callback(this, mode == INPUT ? value : !value);
            //     lastReadAt = millis();
            //     Base::run();
            // }
        }
    };
    uint8_t Accelerometer::COUNT = 0;
}
#endif /* Accelerometer_h */