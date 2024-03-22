#include <vector>
#include <memory>

#include "AnalogIn.h"
#include "DigitalOut.h"
#include "Button.h"
#include "Touch.h"
#include "UltraSonic.h"
#include "RFID.h"
#include "TempHumidity.h"
#include "Stepper.h"
#include "PwmServoDc.h"
//#include "MPU9250.h"
#include "Oled.h"


std::vector<std::shared_ptr<Devices::Base>> devices;

Devices::Base* addDevice(Devices::Base * item) {
  devices.push_back(std::shared_ptr<Devices::Base>(item));
  return (Devices::Base*)devices.back().get();
}

template <class T>
T* getDevice(uint8_t id) {
  return (T*)devices[id].get();
}
