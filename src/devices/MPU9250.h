#ifndef MPU9250_h
#define MPU9250_h

#include <Arduino.h>
#include "Base.h"
#include <Wire.h>
#include "dependencies/I2C/I2C.h"



namespace Devices {

    class MPU_9250 : public Base {
        const static uint8_t MAX = 2;
        static uint8_t COUNT;

        const static uint8_t IMU_ADDRESS = 0x68;
        const static uint8_t MAG_ADDRESS = 0x0C;
        const static uint8_t GYRO_FULL_SCALE_250_DPS = 0x00;
        const static uint8_t GYRO_FULL_SCALE_500_DPS = 0x08;
        const static uint8_t GYRO_FULL_SCALE_1000_DPS = 0x10;
        const static uint8_t GYRO_FULL_SCALE_2000_DPS = 0x18;
        const static uint8_t ACC_FULL_SCALE_2G = 0x00;
        const static uint8_t ACC_FULL_SCALE_4G = 0x08;
        const static uint8_t ACC_FULL_SCALE_8G = 0x10;
        const static uint8_t ACC_FULL_SCALE_16G = 0x18;
        const static uint8_t TEMPERATURE_OFFSET = 21;// As defined in documentation
        const static float G ;

        struct gyroscope_raw {
        int16_t x, y, z;
        } gyroscope;

        struct accelerometer_raw {
        int16_t x, y, z;
        } accelerometer;

        struct magnetometer_raw {
        int16_t x, y, z;
        struct {
            int8_t x, y, z;
        } adjustment;
        } magnetometer;

        struct temperature_raw {
        int16_t value;
        } temperature;

    public :
        struct OutData {
        struct {
            float x, y, z;
        } accelerometer, gyroscope, magnetometer;
        float temperature;
        } normalized ;
    private :

        RunHandler<OutData> callback;
        uint32_t lastReadAt = 0;
        uint32_t readDelay = 100;

        void init() {
        Wire.begin();

        I2CwriteByte(IMU_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS); // Configure gyroscope range
        I2CwriteByte(IMU_ADDRESS, 28, ACC_FULL_SCALE_2G);        // Configure accelerometer range
        I2CwriteByte(IMU_ADDRESS, 55, 0x02); // Set by pass mode for the magnetometers
        I2CwriteByte(IMU_ADDRESS, 56, 0x01); // Enable interrupt pin for raw data
        //setMagnetometerAdjustmentValues
        I2CwriteByte(MAG_ADDRESS, 0x0A, 0x1F); // Set 16-bits output & fuse ROM access mode
        delay(1000);
        uint8_t buff[3];
        I2Cread(MAG_ADDRESS, 0x10, 3, buff); // Read adjustments
        magnetometer.adjustment.x = buff[0]; //Adjustment for X axis
        magnetometer.adjustment.y = buff[1]; //Adjustment for Y axis
        magnetometer.adjustment.z = buff[2]; //Adjustment for Z axis
        I2CwriteByte(MAG_ADDRESS, 0x0A, 0x10); // Power down
        I2CwriteByte(MAG_ADDRESS, 0x0A, 0x12); // Request continuous magnetometer measurements in 16 bits (mode 1)
        }
        bool isImuReady() {
        uint8_t isReady; // Interruption flag
        I2Cread(IMU_ADDRESS, 58, 1, &isReady);
        return isReady & 0x01; // Read register and wait for the RAW_DATA_RDY_INT
        }
        bool isMagnetometerReady() {
        uint8_t isReady; // Interruption flag
        I2Cread(MAG_ADDRESS, 0x02, 1, &isReady);
        return isReady & 0x01; // Read register and wait for the DRDY
        }
        void readRawImu()
        {
        uint8_t buff[14];
        // Read output registers:
        // [59-64] Accelerometer
        // [65-66] Temperature
        // [67-72] Gyroscope
        I2Cread(IMU_ADDRESS, 59, 14, buff);
        // Accelerometer, create 16 bits values from 8 bits data
        accelerometer.x = (buff[0] << 8 | buff[1]);
        accelerometer.y = (buff[2] << 8 | buff[3]);
        accelerometer.z = (buff[4] << 8 | buff[5]);
        // Temperature, create 16 bits values from 8 bits data
        temperature.value = (buff[6] << 8 | buff[7]);
        // Gyroscope, create 16 bits values from 8 bits data
        gyroscope.x = (buff[8] << 8 | buff[9]);
        gyroscope.y = (buff[10] << 8 | buff[11]);
        gyroscope.z = (buff[12] << 8 | buff[13]);
        }
        void readRawMagnetometer() {
        uint8_t buff[7];
        // Read output registers:
        // [0x03-0x04] X-axis measurement
        // [0x05-0x06] Y-axis measurement
        // [0x07-0x08] Z-axis measurement
        I2Cread(MAG_ADDRESS, 0x03, 7, buff);
        // Magnetometer, create 16 bits values from 8 bits data
        magnetometer.x = (buff[1] << 8 | buff[0]);
        magnetometer.y = (buff[3] << 8 | buff[2]);
        magnetometer.z = (buff[5] << 8 | buff[4]);
        }
        void normalize(gyroscope_raw gyroscope)
        {
        // Sensitivity Scale Factor (MPU datasheet page 8)
        normalized.gyroscope.x = gyroscope.x / 32.8;
        normalized.gyroscope.y = gyroscope.y / 32.8;
        normalized.gyroscope.z = gyroscope.z / 32.8;
        }
        void normalize(accelerometer_raw accelerometer)
        {
        // Sensitivity Scale Factor (MPU datasheet page 9)
        normalized.accelerometer.x = accelerometer.x * G / 16384;
        normalized.accelerometer.y = accelerometer.y * G / 16384;
        normalized.accelerometer.z = accelerometer.z * G / 16384;
        }
        void normalize(temperature_raw temperature)
        {
        // Sensitivity Scale Factor (MPU datasheet page 11) & formula (MPU registers page 33)
        normalized.temperature = ((temperature.value - TEMPERATURE_OFFSET) / 333.87) + TEMPERATURE_OFFSET;
        }
        void normalize(magnetometer_raw magnetometer)
        {
        // Sensitivity Scale Factor (MPU datasheet page 10)
        // 0.6 µT/LSB (14-bit)
        // 0.15µT/LSB (16-bit)
        // Adjustment values (MPU register page 53)
        normalized.magnetometer.x = magnetometer.x * 0.15 * (((magnetometer.adjustment.x - 128) / 256) + 1);
        normalized.magnetometer.y = magnetometer.y * 0.15 * (((magnetometer.adjustment.y - 128) / 256) + 1);
        normalized.magnetometer.z = magnetometer.z * 0.15 * (((magnetometer.adjustment.z - 128) / 256) + 1);
        }
    public :
        MPU_9250(RunHandler<OutData> callback)
        : callback(callback),
            Base(Base::TYPE::MPU9250)
        {
            n = MPU_9250::COUNT++;
            if (MPU_9250::COUNT >= MPU_9250::MAX) {
                throw "TO MUCH MPU9250 DEVICE";
            }
            init();

        }
        virtual ~MPU_9250() override {}
        virtual void run() override {
            if (millis() - lastReadAt > readDelay) {
                if (isImuReady()) {
                    readRawImu();
                    normalize(gyroscope);
                    normalize(accelerometer);
                    normalize(temperature);
                }

        //        if (isMagnetometerReady()) {
        //          readRawMagnetometer();
        //          normalize(magnetometer);
        //        }
                callback(this, normalized);
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
    };
    uint8_t MPU_9250::COUNT = 0;
    const float MPU_9250::G = 9.80665f;
}

#endif /* MPU9250_h */