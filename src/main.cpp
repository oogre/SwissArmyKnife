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
                                 .setInPort(prefs.getUInt("inPort", 8888))
                                 .setOutPort(prefs.getUInt("outPort", 9999))
                                 .setOutIP(IPAddress(prefs.getUInt("outIP", (uint32_t)IPAddress(192, 168, 4, 255))))
                                 .setConType((EasyOsc::CONNECTION_TYPE)prefs.getUChar("conType", (uint8_t)EasyOsc::CONNECTION_TYPE::ACCESS_POINT));

  wait = prefs.getUInt("wait", 0);

  com = EasyOsc(conf);
  com.begin();
  Serial.println(com.toString());

  /* ON RECEIVING VALID OSC MESSAGE BLINK BUILTIN_LED */
  IN com.onMessage("*", {
    [](OSCMessage & msg) {
      pinMode(2, OUTPUT);
      digitalWrite(2, !digitalRead(2));
    }
  });



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
      -> /setup/conType [conType] 0 : ACCESS_POINT, 1 : REGULAR_WIFI, 2 : USB_SERIAL
      it will be available at next startup
  */
  IN com.onMessage("/setup/conType", "i", {
    [](OSCMessage & msg) {
      uint8_t conType = msg.getInt(0);
      prefs.putUChar("conType", conType);
    }
  });

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
      prefs.clear();
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
    }
  });

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
    }
  });

  /*
    INPUT_PULLUP
    -> /setup/input_pullup [PIN]
    <- /touch/status id
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
    }
  });

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
    }
  });

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
    }
  });

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
    }
  });

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
      OUT EasyOsc::MessageOSC("/setup/" + String(device->getN()) + "/status/").add(device->getN()).send(&com).toString();
      IN com.onMessage("/servo/" + String(device->getN()) + "/pos", "i", {
        [id](OSCMessage & msg) {
          auto device = getDevice<Devices::Servo>(id);
          device->setPos(msg.getInt(0));
          OUT EasyOsc::MessageOSC("/servo/" + String(device->getN()) + "/pos/status/").add(device->getPos()).send(&com).toString();
        }
      });
    }
  });

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
  }
});
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

