#ifndef Accelrometer_h
#define Accelrometer

#include <Arduino.h>
#include "Base.h"
#include <Wire.h>
#include "dependencies/MMA7660/MMA7660.h"

namespace Devices {

    class Accelerometer : public Base {
    public :
        struct fVec3 {
            float x; 
            float y; 
            float z;
        };
        struct Accel {
            fVec3 acc;
        };
    private:
        const static uint8_t MAX = 17;
        static uint8_t COUNT;
        uint8_t ID;
        RunHandler<Accel> callback;
        MMA7660 accelemeter;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 33;
    public :
        Accelerometer(RunHandler<Accel> callback)
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
            if (millis() - lastReadAt > readDelay) {
                int8_t x;
                int8_t y;
                int8_t z;
                float ax, ay, az;
                accelemeter.getXYZ(&x, &y, &z);
                accelemeter.getAcceleration(&ax, &ay, &az);
                Accel value = {
                    fVec3{ax, ay, az}
                };
                callback(this, value);
                lastReadAt = millis();
                Base::run();
            }
        }
    };
    uint8_t Accelerometer::COUNT = 0;
}
#endif /* Accelerometer_h */