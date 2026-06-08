#ifndef Encoder_h
#define Encoder_h

#include <ESP32Encoder.h>
#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class Encoder : public Base {
        const static uint8_t MAX = 4;
        static uint8_t COUNT;
        uint8_t DT;
        uint8_t CLK;
        int64_t oldCount = 0;
        RunHandler<int64_t> callback;
        ESP32Encoder * encoder;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 0;
    public :
        Encoder(uint8_t DT, uint8_t CLK, RunHandler<int64_t> callback)
        :   DT(DT),
            CLK(CLK),
            callback(callback),
            Base(Base::TYPE::ENCODER)
        {

            encoder = new ESP32Encoder() ;
            encoder->attachHalfQuad ( DT, CLK );
            encoder->setCount ( oldCount );
            n = Encoder::COUNT++;
            if (Encoder::COUNT > Encoder::MAX) {
                throw "TO MUCH Encoder DEVICE";
            }
        }
        virtual ~Encoder() override {
            free(encoder);
            encoder = NULL;
        }

        virtual void run() override {
            if(!_active)return;
            if (millis() - lastReadAt > readDelay) {
                int64_t value = encoder->getCount() / 2;
                if(oldCount != value){
                    callback(this, value);
                }
                oldCount = value; 
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
            return Base::toString() + " " + String((int)DT) + " " + String((int)CLK) + " " + String(readDelay) + " " + String(_active) ;
        }
    };
    uint8_t Encoder::COUNT = 0;

}
#endif /* Encoder_h */