#include <Arduino.h>

#include "Streaming.h"
#include <Preferences.h>
#include "Tools.h"
#include <EasyOsc.h>
#include "devices/Devices.h"

#define OUT Serial<<"<- "<<
#define IN Serial<<"-> "<<

Preferences prefs;
EasyOsc com;
uint32_t wait;

void setup() {
  Serial.begin(115200);
  while(!Serial)delay(1);

  prefs.begin("SwissArmyKnife", false);

  OUT "To Reset Pref Timeout : short D4 to GND at startup \n";
  bool shoudResetPref = Tools::startingUp([]() {
    delay(50);
    Serial.print(".");
    pinMode(2, OUTPUT);
    digitalWrite(2, !digitalRead(2));
    pinMode(4, INPUT_PULLUP);
    return !digitalRead(4);
  }, 3000);
  if (shoudResetPref) {
    prefs.clear();
    OUT "ResetPref\n";
  }
  OUT "\nDONE\n";

  EasyOsc::ConnectionConf conf = EasyOsc::ConnectionConf()
                                 .setSSID(prefs.getString("SSID", "__DEVICE_NAME__"))
                                 .setPWD(prefs.getString("PWD", ""))
                                 .setHostname(prefs.getString("hostname", "knife"))
                                 .setInPort(prefs.getUInt("inPort", 8888))
                                 .setOutPort(prefs.getUInt("outPort", 9999))
                                 .setOutIP(IPAddress(prefs.getUInt("outIP", (uint32_t)IPAddress(192, 168, 4, 255))))
                                 .setLocalIP(IPAddress(prefs.getUInt("localIP", (uint32_t)IPAddress(192, 168, 4, 1))))
                                 .setGateway(IPAddress(prefs.getUInt("gateway", (uint32_t)IPAddress(192, 168, 4, 1))))
                                 .setSubnet(IPAddress(prefs.getUInt("subnet", (uint32_t)IPAddress(255, 255, 0, 0))))
                                 .setConType((EasyOsc::CONNECTION_TYPE)prefs.getUChar("conType", (uint8_t)EasyOsc::CONNECTION_TYPE::ACCESS_POINT));

  wait = prefs.getUInt("wait", 0);

  com = EasyOsc(conf);
  Serial.println(com.toString());
  com.begin();
  
  /* ON RECEIVING VALID OSC MESSAGE BLINK BUILTIN_LED */
  IN com.onMessage("*", {
    [](OSCMessage & msg) {
      pinMode(2, OUTPUT);
      digitalWrite(2, !digitalRead(2));
    }
  });

  { // NETWORK
    /*
        -> /setup/SSID [SSID]
        it will be available at next startup
    */
    IN com.onMessage("/setup/SSID", "s", {
      [](OSCMessage & msg) {
        char buffer[64];
        msg.getString(0, buffer, 64);
        prefs.putString("SSID", String(buffer));
      }
    });

    /*
        -> /setup/PWD [PWD]
        it will be available at next startup
    */
    IN com.onMessage("/setup/PWD", "s", {
      [](OSCMessage & msg) {
        char buffer[64];
        msg.getString(0, buffer, 64);
        prefs.putString("PWD", String(buffer));
      }
    });

    /*
        -> /setup/hostname [hostname]
        it will be available at next startup
    */
    IN com.onMessage("/setup/hostname", "s", {
      [](OSCMessage & msg) {
        char buffer[64];
        msg.getString(0, buffer, 64);
        prefs.putString("hostname", String(buffer));
      }
    });

    /*
        -> /setup/inPort [inPort]
        it will be available at next startup
    */
    IN com.onMessage("/setup/inPort", "i", {
      [](OSCMessage & msg) {
        uint32_t inPort = msg.getInt(0);
        prefs.putUInt("inPort", inPort);
      }
    });

    /*
        -> /setup/outPort [outPort]
        it will be available at next startup
    */
    IN com.onMessage("/setup/outPort", "i", {
      [](OSCMessage & msg) {
        uint32_t outPort = msg.getInt(0);
        prefs.putUInt("outPort", outPort);
      }
    });

    /*
        -> /setup/outIP [byte0 byte2 byte3 byte4]
        it will be available at next startup
    */
    IN com.onMessage("/setup/outIP", "iiii", {
      [](OSCMessage & msg) {
        uint8_t A = msg.getInt(0);
        uint8_t B = msg.getInt(1);
        uint8_t C = msg.getInt(2);
        uint8_t D = msg.getInt(3);
        prefs.putUInt("outIP", (uint32_t)IPAddress(A, B, C, D));
      }
    });

    /*
        -> /setup/localIP [byte0 byte2 byte3 byte4]
        it will be available at next startup
    */
    IN com.onMessage("/setup/localIP", "iiii", {
      [](OSCMessage & msg) {
        uint8_t A = msg.getInt(0);
        uint8_t B = msg.getInt(1);
        uint8_t C = msg.getInt(2);
        uint8_t D = msg.getInt(3);
        prefs.putUInt("localIP", (uint32_t)IPAddress(A, B, C, D));
      }
    });
  
    /*
        -> /setup/gateway [byte0 byte2 byte3 byte4]
        it will be available at next startup
    */
    IN com.onMessage("/setup/gateway", "iiii", {
      [](OSCMessage & msg) {
        uint8_t A = msg.getInt(0);
        uint8_t B = msg.getInt(1);
        uint8_t C = msg.getInt(2);
        uint8_t D = msg.getInt(3);
        prefs.putUInt("gateway", (uint32_t)IPAddress(A, B, C, D));
      }
    });

    /*
        -> /setup/subnet [byte0 byte2 byte3 byte4]
        it will be available at next startup
    */
    IN com.onMessage("/setup/subnet", "iiii", {
      [](OSCMessage & msg) {
        uint8_t A = msg.getInt(0);
        uint8_t B = msg.getInt(1);
        uint8_t C = msg.getInt(2);
        uint8_t D = msg.getInt(3);
        prefs.putUInt("subnet", (uint32_t)IPAddress(A, B, C, D));
      }
    });

    /*
        -> /setup/conType [conType] 0 : ACCESS_POINT, 1 : REGULAR_WIFI, 2 : USB_SERIAL
        it will be available at next startup
    */
    IN com.onMessage("/setup/conType", "i", {
      [](OSCMessage & msg) {
        uint8_t conType = msg.getInt(0);
        prefs.putUChar("conType", conType);
      }
    });
  }
  
  { // TOOLS
    /*
      -> /setup/wait [wait]
    */
    IN com.onMessage("/setup/wait", "i", {
      [](OSCMessage & msg) {
        wait = msg.getInt(0);
        prefs.putUInt("wait", wait);
      }
    });

    /*
        -> /setup/default
        reset esp connection conf and restart
    */
    IN com.onMessage("/setup/default", "", {
      [](OSCMessage & msg) {
        prefs.remove("conType");
        prefs.remove("subnet");
        prefs.remove("gateway");
        prefs.remove("localIP");
        prefs.remove("outIP");
        prefs.remove("outPort");
        prefs.remove("inPort");
        prefs.remove("hostname");
        prefs.remove("PWD");
        prefs.remove("SSID");
        ESP.restart();
      }
    });
    
    /*
        -> /setup/restart
        restart the esp32
    */
    IN com.onMessage("/restart", "", {
      [](OSCMessage & msg) {
        ESP.restart();
      }
    });
  }
  
  { // POTENTIOMETER
    /*
      POTENTIOMETER
      -> /setup/potentiometer [PIN]
      <- /potentiometer/status id
      <- /potentiometer/ID VALUE
    */
    IN com.onMessage("/setup/potentiometer", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::AnalogIn( msg.getInt(0), {
          [](Devices::Base * device, uint16_t value) {
            EasyOsc::MessageOSC("/potentiometer/" + String(device->getN())).add(value).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/potentiometer/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/potentiometer/" + String(device->getN()) + "/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::AnalogIn>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/potentiometer/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });
        IN com.onMessage("/potentiometer/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::AnalogIn>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/potentiometer/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/potentiometer/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/potentiometer/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/potentiometer/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/potentiometer/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }

  { // ENCODER
    /*
      ENCODER
      -> /setup/encoder [DT, CLK]
      <- /encoder/status id
      <- /encoder/ID VALUE
    */
    IN com.onMessage("/setup/encoder", "ii", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Encoder( msg.getInt(0), msg.getInt(1), {
          [](Devices::Base * device, int32_t value) {
            EasyOsc::MessageOSC("/encoder/" + String(device->getN())).add(value).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/encoder/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/encoder/" + String(device->getN()) + "/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Encoder>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/encoder/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });
        IN com.onMessage("/encoder/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Encoder>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/encoder/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/encoder/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/encoder/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/encoder/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/encoder/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // OUTPUT
    /*
      OUTPUT
      -> /setup/output [PIN]
      <- /output/status id
      -> /output/id [VALUE (0 is LOW larger is HIGH)]
    */
    IN com.onMessage("/setup/output", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::DigitalOut(msg.getInt(0)));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/output/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/output/" + String(device->getN()) + "/run", "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::DigitalOut>(id);
            device->setValue(msg.getInt(0) == 0 ? LOW : HIGH);
            OUT EasyOsc::MessageOSC("/output/" + String(device->getN()) + "/run/status/").add(device->getValue()).send(&com).toString();
          }
        });
      }
    });
  }

  { // STRIP
    /*
      STRIP
      -> /setup/strip [PIN, PIXEL_COUNT]
      <- /strip/status ID
      -> /strip/ID/run [LED_ID, RED, GREEN, BLUE, BRIGHTNESS]
    */
    IN com.onMessage("/setup/strip", "ii", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Strip(msg.getInt(1), msg.getInt(0)));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/strip/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/strip/" + String(device->getN()) + "/run", "iiiii", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Strip>(id);
            device->setValue(msg.getInt(0), msg.getInt(1), msg.getInt(2), msg.getInt(3), msg.getInt(4));
            OUT EasyOsc::MessageOSC("/output/" + String(device->getN()) + "/strip/status/").add(1).send(&com).toString();
          }
        });
      }
    });
  }

  { // INPUT
    /*
      INPUT
      -> /setup/input [PIN]
      <- /input/status id
      <- /input VALUE
    */
    IN com.onMessage("/setup/input", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Button( msg.getInt(0), INPUT, {
          [](Devices::Base * device, bool value) {
            EasyOsc::MessageOSC("/input/" + String(device->getN())).add(value).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/input/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/input/"+ String(device->getN())+"/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Button>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/input/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });

        IN com.onMessage("/input/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Button>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/input/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/input/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/input/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/input/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/input/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // INPUT_PULLUP
    /*
      INPUT_PULLUP
      -> /setup/input_pullup [PIN]
      <- /input_pullup/status id
      <- /input_pullup VALUE
    */
    IN com.onMessage("/setup/input_pullup", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Button( msg.getInt(0), INPUT_PULLUP, {
          [](Devices::Base * device, bool value) {
            EasyOsc::MessageOSC("/input_pullup/" + String(device->getN())).add(value).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/input_pullup/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/input_pullup/"+ String(device->getN())+"/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Button>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/input_pullup/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });

        IN com.onMessage("/input_pullup/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Button>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/input_pullup/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/input_pullup/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/input_pullup/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/input_pullup/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/input_pullup/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // TOUCH
    /*
      TOUCH - CAPACITIVE SENSOR
      -> /setup/touch [PIN] 4 15 13 12 14 27 33 32
      <- /touch/status id
      <- /touch/id VALUE
    */
    IN com.onMessage("/setup/touch", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Touch( msg.getInt(0), {
          [](Devices::Base * device, uint16_t value) {
            EasyOsc::MessageOSC("/touch/" + String(device->getN())).add(value).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/touch/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/touch/"+ String(device->getN())+"/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Touch>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/touch/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });

        IN com.onMessage("/touch/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Touch>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/touch/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/touch/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/touch/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/touch/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/touch/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // ULTRASONIC DISTANCE SENSOR
    /*
      ULTRASONIC DISTANCE SENSOR
      -> /setup/dist [PIN_TRIG, PIN_ECHO]
      <- /dist/status id
      <- /dist/id VALUE
    */
    IN com.onMessage("/setup/dist", "ii", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::UltraSonic( msg.getInt(0), msg.getInt(1), {
          [](Devices::Base * device, float value) {
            EasyOsc::MessageOSC("/dist/" + String(device->getN())).add(value).send(&com);
          }
        }));
        OUT EasyOsc::MessageOSC("/dist/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
      }
    });
  }

  { // RFID
    /*
      RFID SENSOR
      -> /setup/rfid [PIN]
      <- /rfid/status id
      <- /rfid/tag/id VALUE
      <- /rfid/onLeave/id VALUE
    */
    IN com.onMessage("/setup/rfid", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::RFID(msg.getInt(0), {
          [](Devices::Base * device, String value) {
            EasyOsc::MessageOSC("/rfid/" + String(device->getN()) + "/tag/" + String(device->getN())).add(value).send(&com);
          }
        }, {
          [](Devices::Base * device, String value) {
            EasyOsc::MessageOSC("/rfid/" + String(device->getN()) + "/tag/" + String(device->getN())).add("").send(&com);
          }
        }));
        OUT EasyOsc::MessageOSC("/rfid/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
      }
    });
  }
  
  { // TempHumidity
    /*
      TempHumidity SENSOR
      -> /setup/tempHumidity [PIN]
      <- /tempHumidity/status id
      <- /tempHumidity/id TEMPERATURE HUMIDITY
    */
    IN com.onMessage("/setup/tempHumidity", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::TempHumidity(msg.getInt(0), {
          [](Devices::Base * device, Devices::TempHumidity::Data value) {
            EasyOsc::MessageOSC("/tempHumidity/" + String(device->getN())).add(value.temperature).add(value.humidity).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/tempHumidity/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/tempHumidity/" + String(device->getN()) + "/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::TempHumidity>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/tempHumidity/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });

        IN com.onMessage("/tempHumidity/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::TempHumidity>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/tempHumidity/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/tempHumidity/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/tempHumidity/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/tempHumidity/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/tempHumidity/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }

  { // MPU9250
    /*
      MPU9250
      -> /setup/mpu9250
      <- /mpu9250/status id
      <- /mpu9250/temperature VALUE
      <- /mpu9250/gyroscope VALUE VALUE VALUE
      <- /mpu9250/accelerometer VALUE VALUE VALUE
      //    <- /mpu9250/magnetometer VALUE VALUE VALUE
    */
    IN com.onMessage("/setup/mpu9250", "", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::MPU_9250({
          [](Devices::Base * device, Devices::MPU_9250::OutData data) {
            OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/temperature").add(data.temperature).send(&com).toString();
            OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/gyroscope").add(data.gyroscope.x).add(data.gyroscope.y).add(data.gyroscope.z).send(&com).toString();
            OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/accelerometer").add(data.accelerometer.x).add(data.accelerometer.y).add(data.accelerometer.z).send(&com).toString();
            //          OUT EasyOsc::MessageOSC("/mpu9250/magnetometer").add(data.magnetometer.x).add(data.magnetometer.y).add(data.magnetometer.z).send(&com).toString();
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/mpu9250/"+ String(device->getN())+"/delay" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::MPU_9250>(id);
            device->setDelay(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
          }
        });

        IN com.onMessage("/mpu9250/"+ String(device->getN())+"/active" , "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::MPU_9250>(id);
            device->setActive(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/mpu9250/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
          }
        });
      }
    });

    IN com.onMessage("/mpu9250/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/mpu9250/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/mpu9250/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/mpu9250/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // STEPPER
    /*
      STEPPER 2 WIRES AND 4 WIRES
      -> /setup/stepper [PIN1 - PIN2 - PIN3 - PIN4] OR [DIR_PIN - STEP_PIN]
      <- /stepper/status id
      -> /stepper/speed/id [SPEED (0 - 100)]
      <- /stepper/id/speed/status SPEED
      -> /stepper/dist/id [DIST (>0 Cloclwise <0 CounterClockwise )]
      <- /stepper/id/dist/status DIST
    */
    auto runStepper = [](uint8_t id) {
      auto device = getDevice<Devices::Stepper>(id);
      OUT EasyOsc::MessageOSC("/stepper/" + String(device->getN()) + "/status/").add(id).send(&com).toString();
      IN com.onMessage("/stepper/speed/" + String(device->getN()), "i", {
        [id](OSCMessage & msg) {
          auto device = getDevice<Devices::Stepper>(id);
          device->setSpeed(msg.getInt(0));
          OUT EasyOsc::MessageOSC("/stepper/" + String(device->getN()) + "/speed/status/").add((int32_t)device->getSpeed()).send(&com).toString();
        }
      });
      IN com.onMessage("/stepper/dist/" + String(device->getN()), "i", {
        [id](OSCMessage & msg) {
          auto device = getDevice<Devices::Stepper>(id);
          device->setDist(msg.getInt(0));
          OUT EasyOsc::MessageOSC("/stepper/" + String(device->getN()) + "/speed/status/").add((int32_t)device->getDist()).send(&com).toString();
        }
      });
    };

    IN com.onMessage("/setup/stepper/2pin", "ii", {
      [runStepper](OSCMessage & msg) {
        auto device = addDevice(new Devices::Stepper(msg.getInt(0), msg.getInt(1)));
        uint8_t id = device->getID();
        runStepper(id);
      }
    });
    
    IN com.onMessage("/setup/stepper/4pin", "iiii", {
      [runStepper](OSCMessage & msg) {
        auto device = addDevice(new Devices::Stepper(msg.getInt(0), msg.getInt(1), msg.getInt(2), msg.getInt(3)));
        uint8_t id = device->getID();
        runStepper(id);
      }
    });
  }

  { // PWM
    /*
      PWM
      -> /setup/PWM [PIN - CHANNEL] - FREQUENCE : default 1000] - RESOLUTION : default 8 ]
      <- /PWM/status id
      -> /PWM/run/id [VALUE (0, 255)]
      <- /PWM/id/status id
    */
    IN com.onMessage("/setup/PWM", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::PWM(msg.getInt(0)));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/PWM/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/PWM/" + String(device->getN()) + "/run", "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::PWM>(id);
            device->setValue(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/PWM/" + String(device->getN()) + "/run/status/").add(device->getValue()).send(&com).toString();
          }
        });
      }
    });
  }

  { // SERVO
    /*
      SERVO
      -> /setup/servo [PIN1]
      <- /servo/status id
      -> /servo/pos/id [pos (0, 180)]
      <- /servo/id/pos/status POS
    */
    IN com.onMessage("/setup/servo", "i", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Servo(msg.getInt(0)));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/servo/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/servo/" + String(device->getN()) + "/pos", "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Servo>(id);
            device->setPos(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/servo/" + String(device->getN()) + "/pos/status/").add(device->getPos()).send(&com).toString();
          }
        });
      }
    });
  }
 
  { // DCmotor
    /*
      DCmotor
      -> /setup/DCmotor [PIN1 PIN2 SPEED_PIN]
      <- /DCmotor/status id
      -> /DCmotor/speed/id [speed (0, 100)]
      -> /DCmotor/dir/id [dir  (-1 BACKWARD 0 STOP 1 FORWARD)]
    */
    IN com.onMessage("/setup/DCmotor", "iii", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::DCMotor(msg.getInt(0), msg.getInt(1), msg.getInt(2)));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/DCmotor/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/DCmotor/speed/" + String(device->getN()), "i", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::DCMotor>(id);
            device->setSpeed(msg.getInt(0));
            OUT EasyOsc::MessageOSC("/DCmotor/" + String(device->getN()) + "/speed/status/").add(device->getSpeed()).send(&com).toString();
          }
        });
        IN com.onMessage("/DCmotor/dir/" + String(device->getN()), "i", {
          [id](OSCMessage & msg) {
            Devices::DCMotor::DIRECTION dir = (Devices::DCMotor::DIRECTION) Tools::sign(msg.getInt(0));
            auto device = getDevice<Devices::DCMotor>(id);
            device->setDir(dir);
            OUT EasyOsc::MessageOSC("/DCmotor/" + String(device->getN()) + "/dir/status/").add(device->getDir()).send(&com).toString();
          }
        });
      }
    });
  }

  { // SPI DISPLAY
    /*
      SPI DISPLAY
      -> /setup/display
      <- /display/status 1
      -> /display/run [PIXELS blob 1024byte]
      <- /display/run/status BLOB_LENGTH
    */
    IN com.onMessage("/setup/display", "", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Oled());
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/display/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
        IN com.onMessage("/display/" + String(device->getN()) + "/run", "b", {
          [id](OSCMessage & msg) {
            auto device = getDevice<Devices::Oled>(id);
            msg.getBlob(0, device->getData(), device->getBufferLen());
            OUT EasyOsc::MessageOSC("/display/" + String(device->getN()) + "/run/status").add(device->getN()).send(&com).toString();
          }
        });
      }
    });
  }
  
  { // ACCELEROMETER
    /*
      I2C ACCELEROMETER
      -> /setup/accel
      <- /accel/status id
      <- /accel/acc x y z
    */
    IN com.onMessage("/setup/accel", "", {
      [](OSCMessage & msg) {
        auto device = addDevice(new Devices::Accelerometer({
          [](Devices::Base * device, Devices::Accelerometer::Accel value) {
            EasyOsc::MessageOSC("/accel/" + String(device->getN())).add(value.acc.x).add(value.acc.y).add(value.acc.z).send(&com);
          }
        }));
        uint8_t id = device->getID();
        OUT EasyOsc::MessageOSC("/accel/" + String(device->getN()) + "/status").add(device->getN()).send(&com).toString();
          IN com.onMessage("/accel/"+ String(device->getN())+"/delay" , "i", {
            [id](OSCMessage & msg) {
              auto device = getDevice<Devices::Accelerometer>(id);
              device->setDelay(msg.getInt(0));
              OUT EasyOsc::MessageOSC("/accel/" + String(device->getN()) + "/delay/status").add((uint16_t)device->getDelay()).send(&com).toString();
            }
          });

          IN com.onMessage("/accel/"+ String(device->getN())+"/active" , "i", {
            [id](OSCMessage & msg) {
              auto device = getDevice<Devices::Accelerometer>(id);
              device->setActive(msg.getInt(0));
              OUT EasyOsc::MessageOSC("/accel/" + String(device->getN()) + "/active/status").add((uint16_t)device->getActive()).send(&com).toString();
            }
          });
      }
    });

    IN com.onMessage("/accel/last/delay", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/accel/" + String(devices.at(devices.size()-1)->getN()) + "/delay").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });

    IN com.onMessage("/accel/last/active", "i", {
      [](OSCMessage & msg) {
          OUT EasyOsc::MessageOSC("/accel/" + String(devices.at(devices.size()-1)->getN()) + "/active").add(msg.getInt(0)).loopBack(&com).toString();
      }
    });
  }
  
  { // burn
    /*
        -> /burn
        record devices config over reboot
    */
    IN com.onMessage("/burn", "", {
      [](OSCMessage & msg) {
        String _devices = "";
        for (auto device : devices) {
          _devices += device->toString() + "\n";
        }
        prefs.putString("burn", _devices);
      }
    });

    /*
        -> /clear
        destroy devices config over reboot
    */
    IN com.onMessage("/clear", "", {
      [](OSCMessage & msg) {
        prefs.remove("burn");
        ESP.restart();
      }
    });

    String burned = prefs.getString("burn", "");
    Serial.println(burned);
    if(burned != ""){
      Serial.println("***************** BURNED ******************");

      int lines = Tools::countChar(burned, '\n');
      for(int i = 0 ; i < lines ; i ++){
        String line = Tools::getString(burned, '\n', i);
        int items = Tools::countChar(line, ' ');
        int type = Tools::getString(line, ' ', 0).toInt();
        switch (type)
        {
        case Devices::Base::TYPE::ULTRA_SONIC :
          OUT EasyOsc::MessageOSC("/setup/dist").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/dist/" + String(devices.at(devices.size()-1)->getN()) + "/active").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::BUTTON :
          if(INPUT == Tools::getString(line, ' ', 2).toInt()){
            OUT EasyOsc::MessageOSC("/setup/input").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
            OUT EasyOsc::MessageOSC("/input/last/delay").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
            OUT EasyOsc::MessageOSC("/input/last/active").add((uint16_t)Tools::getString(line, ' ', 4).toInt()).loopBack(&com).toString();
          }else if(INPUT_PULLUP== Tools::getString(line, ' ', 2).toInt()){
            OUT EasyOsc::MessageOSC("/setup/input_pullup").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
            OUT EasyOsc::MessageOSC("/input_pullup/last/delay").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
            OUT EasyOsc::MessageOSC("/input_pullup/last/active").add((uint16_t)Tools::getString(line, ' ', 4).toInt()).loopBack(&com).toString();
          }
          break;
        case Devices::Base::TYPE::DIGITAL_OUT :
          OUT EasyOsc::MessageOSC("/setup/output").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::ANALOG_IN :
          OUT EasyOsc::MessageOSC("/setup/potentiometer").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/potentiometer/last/delay").add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/potentiometer/last/active").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::ENCODER :
          OUT EasyOsc::MessageOSC("/setup/encoder").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/encoder/last/delay").add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/encoder/last/active").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::TOUCH :
          OUT EasyOsc::MessageOSC("/setup/touch").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/touch/last/delay").add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          OUT EasyOsc::MessageOSC("/touch/last/active").add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::PWM :
          OUT EasyOsc::MessageOSC("/setup/PWM").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::SERVO :
          OUT EasyOsc::MessageOSC("/setup/servo").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::DC_MOTO :
          OUT EasyOsc::MessageOSC("/setup/DCmotor").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).add((uint16_t)Tools::getString(line, ' ', 2).toInt()).add((uint16_t)Tools::getString(line, ' ', 3).toInt()).loopBack(&com).toString();
          break;
        case Devices::Base::TYPE::NONE :
          /*TODO*/
          break;
        case Devices::Base::TYPE::SOFT_SERIAL :
          /*TODO*/
        break;
        case Devices::Base::TYPE::MPU9250 :
          /*TODO*/
        break;
        case Devices::Base::TYPE::TEMPERATURE_HUMIDITY :
          /*TODO*/
        break;
        case Devices::Base::TYPE::STEPPER :
          /*TODO*/
        break;
        case Devices::Base::TYPE::OLED :
          /*TODO*/
        break;
        case Devices::Base::TYPE::RFID :
          /*TODO*/
        break;
        case Devices::Base::TYPE::ACCELEROMETER :
          /*TODO*/
        break;
        case Devices::Base::TYPE::STRIP :
          OUT EasyOsc::MessageOSC("/setup/strip").add((uint16_t)Tools::getString(line, ' ', 1).toInt()).add((uint16_t)Tools::getString(line, ' ', 2).toInt()).loopBack(&com).toString();
          break;
        default:
          /*TODO*/
        break;
        }
      }
      Serial.println("*******************************************");
    }
  }

  Serial.println(com.toString());
}

void loop() {
  com.update();
  for (auto device : devices) {
    device->run();
  }

  if(wait>0){
    delay(wait);
  }
}












