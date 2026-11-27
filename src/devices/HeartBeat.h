#ifndef HeartBeat_h
#define HeartBeat_h

#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include "Base.h"

const byte RATE_SIZE = 5;   // How many beats we average

namespace Devices {

    class HeartBeat : public Base {
        const static uint8_t MAX = 2;
        static uint8_t COUNT;
        
        MAX30105 sensor;
        RunHandler<long> callback;
        byte rates[RATE_SIZE];      // Store last few valid BPM values
        byte rateSpot = 0;
        byte validBeats = 0;
        long lastBeat = 0;
        float currentBPM = 0;
        int averageBPM = 0;
        const float MIN_BPM = 45;
        const float MAX_BPM = 180;


    public :
        HeartBeat(RunHandler<long> callback)
        : callback(callback),
            Base(Base::TYPE::DIGITAL_OUT)
        {
            n = HeartBeat::COUNT++;
            if (HeartBeat::COUNT > HeartBeat::MAX) {
                throw "TO MUCH HeartBeat DEVICE";
            }

             if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
                Serial.println("Error initializing the sensor...");
                while (1);
            }

            sensor.setup();                    // Use default settings
            sensor.setPulseAmplitudeRed(0);    // Turn off red LED
        }
        virtual ~HeartBeat() override {}
        virtual void run() override {
            
            long irValue = sensor.getIR();      // Read infrared value
            if (checkForBeat(irValue)) {
                long timeBetweenBeats = millis() - lastBeat;
                lastBeat = millis();
                currentBPM = 60.0 / (timeBetweenBeats / 1000.0);
                if (currentBPM >= MIN_BPM && currentBPM <= MAX_BPM) {
                    rates[rateSpot] = (byte)currentBPM;
                    rateSpot++;
                    if (rateSpot >= RATE_SIZE){
                        rateSpot = 0;
                    }
                    // Count valid beats (until buffer is full)
                    if (validBeats < RATE_SIZE) {
                        validBeats++;
                    }
                    
                    // Only calculate average when buffer is full
                    if (validBeats >= RATE_SIZE) {
                        // Calculate the average
                        averageBPM = 0;
                        for (byte i = 0; i < RATE_SIZE; i++) {
                            averageBPM += rates[i];
                        }
                        averageBPM /= RATE_SIZE;
                    }
                }
            }

            if (irValue < 50000) {
                callback(this, -1);
            }
            else if (averageBPM == 0) {
                // Finger is present but not enough valid beats yet
                callback(this, 0);
            }
            else {
                callback(this, averageBPM);    
            }
            
            Base::run();
        }
        virtual String toString() override {
            return Base::toString();
        }
    };
    uint8_t HeartBeat::COUNT = 0;
}
#endif /* HeartBeat_h */