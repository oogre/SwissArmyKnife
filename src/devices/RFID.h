#ifndef RFID_h
#define RFID_h

#include <Arduino.h>
#include "Base.h"
#include "SoftSerial.h"

namespace Devices {

    class RFID : public SoftSerial {
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
        uint8_t ID;
        RunHandler<String> onEnter;
        RunHandler<String> onLeave;
        String oldValue = "";
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 500;
    public :
        RFID(uint8_t rx, RunHandler<String> onEnter = {[](Base * target, String value) {}}, RunHandler<String> onLeave = {[](Base * target, String value) {}})
        : onEnter(onEnter),
            onLeave(onLeave),
            SoftSerial(rx, 2400, RFID::readSerial, {
        RFID::onUpdate
        })
        {
            ID = RFID::COUNT++;
            if (RFID::COUNT > RFID::MAX) {
                throw "TO MUCH RFID DEVICE";
            }
        }

        virtual ~RFID() override {
        }

        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                SoftSerial::run();
                lastReadAt = millis();
            }
        }

        void setDelay(uint32_t time){
            readDelay = time;
        }
        uint32_t getDelay(){
            return readDelay;
        }

        static void onUpdate(Base * target, String value) {
            RFID* self = (RFID*)target;
            if (self->oldValue == "" && value != "") {
                self->onEnter(target, value);
            }
            else if (value == "" && self->oldValue != "") {
                self->onLeave(target, self->oldValue);
            }
            else if (self->oldValue != value) {
                self->onLeave(target, self->oldValue);
                self->onEnter(target, value);
            }
            self->oldValue = value;
        }

        static String readSerial(SoftwareSerial* serial) {
            int bytesread = 0;
            int  val = 0;
            char code[10];
            String tag = "";

            if (serial->available() > 0) {         // If data available from reader
                if ((val = serial->read()) == 10) {  // Check for header
                bytesread = 0;
                while (bytesread < 10) {              // Read 10 digit code
                    if ( serial->available() > 0) {
                    val = serial->read();
                    if ((val == 10) || (val == 13)) { // If header or stop bytes before the 10 digit reading
                        break;                         // Stop reading
                    }
                    code[bytesread] = val;           // Add the digit
                    bytesread++;                     // Ready to read next digit
                    }
                }
                if (bytesread == 10) {               // If 10 digit read is complete
                    for (int x = 0; x < 10; x++)       //Copy the Chars to a String
                    {
                    tag += code[x];
                    }
                    serial->flush();
                }
                }
            }
            return tag;
        }
    };
    uint8_t RFID::COUNT = 0;
}

#endif /* RFID_h */