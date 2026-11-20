#ifndef PolyButton_h
#define PolyButton_h

#include <Arduino.h>
#include "Base.h"

namespace Devices {

    class PolyButton : public Base {
    public :
        const static uint8_t MAX_PIN = 16;
    private :          
        const static uint8_t MAX = 17;
        static uint8_t COUNT;
        uint8_t pins[MAX_PIN] ;
        uint8_t pinCursor = 0;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;
        RunHandler<String> callback;
  
        void makeItGround(int pin){
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        }

        uint8_t read(int pin){
            for(uint8_t i = 0 ; i < pinCursor ; i++){
                makeItGround(pins[i]);
            }
            pinMode(pin, INPUT_PULLUP);
            return digitalRead(pin);
        }


    public :
        PolyButton(RunHandler<String> callback)
        : callback(callback),
        Base(Base::TYPE::BUTTON)
        {
            n = PolyButton::COUNT++;
            if (PolyButton::COUNT > PolyButton::MAX) {
                throw "TO MUCH Button DEVICE";
            }
            for(uint8_t i = 0 ; i < MAX_PIN ; i++){
                pins[i]= 0;
            }
        }
        virtual ~PolyButton() override {}
        virtual void run() override {
            if(!_active)return;
            if (millis() - lastReadAt > readDelay) {    
                String result = "";
                for(uint8_t i = 0 ; i < pinCursor ; i++){
                    result += String(read(pins[i]));
                }
                callback(this, result);
                Base::run();

                lastReadAt = millis();
            }
        }

        void addButton(uint8_t pin){
            if(pinCursor == MAX_PIN) throw "TO MUCH Button in PolyButton";
            pins[pinCursor++] = pin;
        }

        uint8_t getLastBtn(){
            return pins[pinCursor];
        }

        void setDelay(uint32_t time){
            readDelay = time;
        }
        uint32_t getDelay(){
            return readDelay;
        }

        virtual String toString() override {
            String _pins = "";
            for(uint8_t i = 0 ; i < MAX_PIN ; i++){
                _pins+= String((int)pins[i]) + " ";
            }

            return Base::toString() + " " + _pins + String(readDelay) + " " + String(_active) ;
        }
    };
    uint8_t PolyButton::COUNT = 0;
}
#endif /* PolyButton_h */