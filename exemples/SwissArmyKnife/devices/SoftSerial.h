#ifndef SoftSerial_h
#define SoftSerial_h


#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Base.h"

namespace Devices {
    class SoftSerial : public Base {
        SoftwareSerial * serial;
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
        uint8_t ID;
        RunHandler<String> callback;
        typedef String (*ActionHandler)(SoftwareSerial*);
        ActionHandler action;
    public :
        SoftSerial(uint8_t rx, uint32_t baudRate, ActionHandler action, RunHandler<String> callback)
        : serial(new SoftwareSerial(rx)),
            action(action),
            callback(callback),
            Base(Base::TYPE::SOFT_SERIAL)
        {
            serial->begin(baudRate);
            ID = SoftSerial::COUNT++;
            if (SoftSerial::COUNT >= SoftSerial::MAX) {
                throw "TO MUCH SoftSerial DEVICE";
            }
        }

        //    SoftSerial(uint8_t rx, uint32_t baudRate, RunHandler<int> callback)
        //      : SoftSerial(rx, 255, baudRate, callback)
        //    {
        //    }
        virtual ~SoftSerial() override {
            free(serial);
            serial = NULL;
        }
        virtual void run() override {
            callback(this, action(serial));
            Base::run();
        }
    };
    uint8_t SoftSerial::COUNT = 0;
}
#endif /* SoftSerial_h */