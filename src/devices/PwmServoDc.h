
#ifndef PWM_SERVO_DC_MOTOR_h
#define PWM_SERVO_DC_MOTOR_h

#include <Arduino.h>
#include <pwmWrite.h>
#include "Base.h"

namespace Devices {

    Pwm pwm = Pwm();

    class PwmServoDc : public Base {
        const static uint8_t MAX = 8;
        static uint8_t COUNT;
    protected :
        uint8_t pin;
        uint8_t ch;
    public :
        PwmServoDc(uint8_t pin, uint8_t channel, Base::TYPE type)
        : Base(type),
            pin(pin),
            ch(channel)
        {
            n = PwmServoDc::COUNT++;
            if (PwmServoDc::COUNT > PwmServoDc::MAX) {
                throw "TO MUCH PwmServoDc DEVICE";
            }

            if (ch % 2 == 0) { // even ch
                pwm.mem[channel + 1].pin = pin;
            } else { // odd ch
                pwm.mem[channel - 1].pin = pin;
            }
        };
    };
    uint8_t PwmServoDc::COUNT = 0;




    class PWM : public PwmServoDc {
        int value;
    public :
        PWM(uint8_t pin) 
        : PwmServoDc(pin, pwm.attach(pin), Base::TYPE::PWM) {}
        virtual ~PWM() override {}
        void setValue(int value) {
            pwm.write(PwmServoDc::pin, value);
            this->value = value;
        }
        int getValue() {
            return value;
        }
    };

    class Servo : public PwmServoDc {
        int value;
    public :
        Servo(uint8_t pin) 
        : PwmServoDc(pin, pwm.attachServo(pin), Base::TYPE::SERVO) {}
        virtual ~Servo() override {}
        void setPos(int pos) {
            this->value = pos;
            pwm.writeServo(PwmServoDc::pin, pos);
        }
        int getPos() {
            return value;
        }
    };

    class DCMotor : public PwmServoDc {
        uint8_t pinA;
        uint8_t pinB;
        uint8_t speed;
        uint8_t dir;
    public :
        enum DIRECTION { FORWARD, BACKWARD, STOP };
        DCMotor(uint8_t pinA, uint8_t pinB, uint8_t pinSpeed)
        : pinA(pinA),
            pinB(pinB),
            PwmServoDc(pinSpeed, pwm.attach(pinSpeed), Base::TYPE::DC_MOTO)
        {
            pinMode(pinA, OUTPUT);
            pinMode(pinB, OUTPUT);
        }
        virtual ~DCMotor() override {}
        void setDir(DIRECTION dir) {
            this->dir = (uint8_t)dir;
            switch (dir) {
                case FORWARD:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, HIGH);
                break;
                case BACKWARD:
                digitalWrite(pinA, HIGH);
                digitalWrite(pinB, LOW);
                break;
                case STOP:
                digitalWrite(pinA, LOW);
                digitalWrite(pinB, LOW);
                break;
            }
        }
        void setSpeed(int speed) {
            this->speed = speed;
            pwm.write(PwmServoDc::pin, (int)floor(speed * 2.55f));
        }

        uint8_t getSpeed() {
            return speed;
        }
        uint8_t getDir() {
            return dir;
        }

    };
}


#endif /* PWM_SERVO_DC_MOTOR_h */