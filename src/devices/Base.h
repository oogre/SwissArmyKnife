#ifndef Base_h
#define Base_h

#include <functional>
#include <utility>

namespace Devices {
  class Base {
    protected :
      const static uint8_t MAX = 255;
      static uint8_t COUNT;
      uint8_t ID;
      uint8_t n;
      bool _active = true;
    public :
      template<class T>
      struct RunHandler {
        using RunHandlerFn = std::function<void(Base*, T)>;
        RunHandler() {};
        RunHandler(RunHandlerFn dec) : run {std::move(dec)} {};
        RunHandlerFn run;
        void operator()(Base* self, T value) {
          this->run(self, value);
        };
      };

      enum TYPE {
        NONE, ENCODER, ULTRA_SONIC, BUTTON, SOFT_SERIAL, MPU9250, ANALOG_IN, TEMPERATURE_HUMIDITY, DIGITAL_OUT, TOUCH, PWM, SERVO, STEPPER, DC_MOTO, OLED, RFID, ACCELEROMETER, STRIP
      };
      TYPE type;
      Base(TYPE type = TYPE::NONE)
        : type(type)
      {
        ID = Base::COUNT++;
        if (Base::COUNT > Base::MAX) {
          throw "TO MUCH DEVICE";
        }
      }
      virtual ~Base() {}
      virtual void run() {};
      virtual uint8_t getID() {
        return ID;
      }
      virtual uint8_t getN() {
        return n;
      }
      virtual void setActive(uint32_t active){
        _active = active == 1;
      }
      uint32_t getActive(){
        return _active ? 1 : 0;
      }

      virtual String toString(){
        return String((int)type);
      }
  };
  uint8_t Base::COUNT = 0;
}
#endif /* Base_h */