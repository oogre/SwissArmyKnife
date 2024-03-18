#ifndef TempHumidity_h
#define TempHumidity_h

#include <Arduino.h>
#include <DHT.h>
#include "Base.h"

namespace Devices {

    class TempHumidity : public Base {
    public :
        struct Data {
            float humidity;
            float temperature;
        };
    private :
        const static uint8_t MAX = 4;
        static uint8_t COUNT;
        uint8_t ID;
        DHT * dht;

        RunHandler<Data> callback;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;
    public :
        TempHumidity(uint8_t pin, RunHandler<Data> callback)
        : dht(new DHT(pin, DHT11)), // DHT11 || DHT22 || DHT21
            callback(callback),
            Base(Base::TYPE::TEMPERATURE_HUMIDITY)
        {
            ID = TempHumidity::COUNT++;
            if (TempHumidity::COUNT > TempHumidity::MAX) {
                throw "TO MUCH TempHumidity DEVICE";
            }
            dht->begin();
        }
        virtual ~TempHumidity() override {}
        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                callback(this, Data{dht->readHumidity(), dht->readTemperature()});
                lastReadAt = millis();
                Base::run();
            }
        }
    };
    uint8_t TempHumidity::COUNT = 0;
}
#endif /* TempHumidity_h */