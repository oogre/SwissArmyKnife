# SwissArmyKnife

SwissArmyKnife is an _Arduino_ software, developped for ESP32 board. It's been writen to help art students to develop custom hardware solution without the needs to write C/C++ code. It uses [OSC](https://fr.wikipedia.org/wiki/Open_Sound_Control) to setup and control hardwares connected on the board.

## Install
This code depends on multiple libraries :  
* Adafruit BusIO
* Adafruit GFX Library
* Adafruit SSD1306
* Adafruit Unified Sensor
* DHT sensor library
* EasyOsc
* ESP32 ESP32S2 AnalogWrite
* EspSoftwareSerial

### With Arduino IDE
Install all in once! Clone this repository. Then unzip file *SwissArmyKnife.zip* copy everything inside the folder : *drop_content_to_arduino_Libraries* and paste them to *~/Documents/Arduino/libraries*</br>
Open *File/Exemples/SwissArmyKnife/exemple/SwissArmyKnife* then upload on your ESP32 board.

### With Plateformio
Clone this repository. Then import the main folder with the Plateformio tools. Enjoy ;)


## Default WiFi Configuration

|         |                   |
|---------|-------------------|
| SSID    | SwissArmyKnife    | 
| pwd     |                   |
| inOSC   | 192.168.4.1:8888  |
| outOSC  | 192.168.4.255:9999|
| conType | ACCESS_POINT(0)   |

## WiFi Configuration

WiFi configuration resist to restart. This config is stored to the EEPROM. Every change will be applied after restart.

| i/o 	| OSC Address 	    | DATA   	                        | Description                         	|
|-----	|-------------	    |--------	                        |-------------------------------------	|
| ->  	| *           	    | *      	                        | every OSC command strob LED-BUILTIN 	|
| ->  	| /setup/SSID 	    | String                            | set WiFi name                       	|
| ->  	| /setup/PWD  	    | String 	                        | set WiFi password                   	|
| ->    | /setup/inPort     | Integer                           | set input port                        |
| ->    | /setup/outPort    | Integer                           | set output port                       |
| ->    | /setup/outIP      | Integer Integer Integer Integer   | set output IP                         |
| ->    | /setup/conType    | Integer                           | (0) : Access Point (1) : Regular WiFi |
| ->    | /setup/wait       | Integer                           | use to slow down the loop with a delay|
| ->    | /setup/default    |                                   | reset configuration to default        |
| ->    | /restart          |                                   | restart ESP to valid WiFi conf        |



## Hardware Configuration

Tell the ESP32 what's connected to it's GPIO pin

### Button

Button are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 16</br>
(VALUE) will depend on Button postion 0 or 1

##### INPUT MODE
 
| i/o 	| OSC Address 	    | DATA   	                 | Description                         	          |
|-----	|-------------	    |--------	                 |-------------------------------------	          |
| ->  	| /setup/input 	    | (PIN_NUM) Integer          | set (PIN_NUM) to be read as a button           |
| <-  	| /input/(ID)/status 	| (ID) Integer               | ESP confirms by reply the (ID) of this button  |
| <-  	| /input/(ID) 	    | (VALUE) Integer            | At every loop ESP send Button state (VALUE)    |
| ->  	| /input/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /input/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |



 
##### INPUT_PULLUP MODE
 
| i/o 	| OSC Address 	     | DATA   	                  | Description                         	       |
|-----	|-------------	     |--------	                  |-------------------------------------	       |
| ->  	| /setup/input_pullup| (PIN_NUM) Integer          | set (PIN_NUM) to be read as a button           |
| <-  	| /input_pullup/(ID)/status 	 | (ID) Integer               | ESP confirms by reply the (ID) of this button  |
| <-  	| /input_pullup/(ID) 	     | (VALUE) Integer            | At every loop ESP send Button state (VALUE)    |
| ->  	| /input_pullup/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /input_pullup/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |


### Potentiometer

Potentiometer are available on (PIN_NUM) : 32, 33, 34, 35</br>
(ID) will be sequentially attributed from 0 to 3</br>
(VALUE) will depend on potentiometer postion from 0 to 4095

| i/o 	| OSC Address 	      | DATA   	                   | Description                         	             |
|-----	|-------------	      |--------	                   |-------------------------------------	             |
| ->  	| /setup/potentiometer| (PIN_NUM) Integer          | set (PIN_NUM) to be read as a potentiometer         |
| <-  	| /potentiometer/(ID)/status 	  | (ID) Integer               | ESP confirms by reply the (ID) of this potentiometer|
| <-  	| /potentiometer/(ID) 	      | (VALUE) Integer            | At every loop ESP send potentiometer state (VALUE)  |
| ->  	| /potentiometer/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /potentiometer/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |

### Touch/Capasitive sensor

Touch sensor are available on (PIN_NUM) : 4, 12, 13, 14, 15, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
(VALUE) will depend on Touch sensor from 0 to ...

| i/o 	| OSC Address 	      | DATA   	                   | Description                         	             |
|-----	|-------------	      |--------	                   |-------------------------------------	             |
| ->  	| /setup/touch        | (PIN_NUM) Integer          | set (PIN_NUM) to be read as a touch sensor          |
| <-  	| /touch/(ID)/status 	  | (ID) Integer               | ESP confirms by reply the (ID) of this touch sensor |
| <-  	| /touch/(ID) 	      | (VALUE) Integer            | At every loop ESP send touch sensor state (VALUE)   |
| ->  	| /touch/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /touch/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |

### RFID

RFID sensor are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
(VALUE) will depend on TAG enter and leave the reader

| i/o 	| OSC Address 	     | DATA   	                  | Description                         	             |
|-----	|-------------	     |--------	                  |-------------------------------------	             |
| ->  	| /setup/rfid        | (PIN_NUM) Integer          | set (PIN_NUM) to be read as a rfid sensor (SofSerial)|
| <-  	| /rfid/(ID)/status 	     | (ID) Integer               | ESP confirms by reply the (ID) of this rfid sensor   |
| <-  	| /rfid/tag/(ID) 	 | (VALUE) String             | Send when RFID TAG enter the reader                  |
| <-  	| /rfid/tag/(ID) 	 |                            | Send when RFID TAG leave the reader                  |
| ->  	| /rfid/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /rfid/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |

### Temperature Humidity

Temperature Humidity sensor are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 3</br>
(VALUE_A) will depend on Temperature in degree celcius</br>
(VALUE_B) will depend on Humidity percent</br>

| i/o 	| OSC Address 	      | DATA   	                            | Description                         	                       |
|-----	|-------------	      |--------	                            |-------------------------------------	                       |
| ->  	| /setup/tempHumidity | (PIN_NUM) Integer                   | set (PIN_NUM) to be read as a tempHumidity sensor            |
| <-  	| /tempHumidity/(ID)/status| (ID) Integer                        | ESP confirms by reply the (ID) of this tempHumidity sensor   |
| <-  	| /tempHumidity/(ID)  | (VALUE_A) Integer (VALUE_B) Integer | At every loop ESP send temperature and Humidity sensor values|
| ->  	| /tempHumidity/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /tempHumidity/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |

### Ultrasonic/Distance Sensor

Ultrasonic sensor are available on (TRIG_PIN) & (ECHO_PIN) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
(VALUE) will depend on Ultrasonic sensor from 0 to ...

| i/o 	| OSC Address 	  | DATA   	                              | Description                         	                |
|-----	|-------------	  |--------	                              |-------------------------------------	                |
| ->  	| /setup/dist     | (TRIG_PIN) Integer (ECHO_PIN) Integer | set (TRIG_PIN) & (ECHO_PIN) to be read as a dist sensor |
| <-  	| /dist/(ID)/status 	  | (ID) Integer                          | ESP confirms by reply the (ID) of this dist sensor      |
| <-  	| /dist/(ID) 	  | (VALUE) Integer                       | At every loop ESP send distance sensor state (VALUE)    |

### DigitalOut

DigitalOut are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 15</br>
Control this OUTPUT by sending /output/run/(ID) (VALUE)

| i/o 	| OSC Address 	    | DATA   	        | Description                         	                |
|-----	|-------------	    |--------	        |-------------------------------------	                |
| ->  	| /setup/output     | (PIN_NUM) Integer | set (PIN_NUM) to be control as an OUTPUT              |
| <-  	| /output/(ID)/status 	| (ID) Integer      | ESP confirms by reply the (ID) of this OUTPUT         |
| ->  	| /output/(ID)/run 	| (VALUE) Integer   | set (VALUE) to 1 or 0 to turn on/off this OUTPUT      |
| <-  	| /output/(ID)/run/status 	| (VALUE) Integer   | get (VALUE) to confirm      |

### PWM

PWM are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
Control this PWM by sending /PWM/run/(ID) (VALUE)

| i/o 	| OSC Address 	    | DATA   	        | Description                         	             |
|-----	|-------------	    |--------	        |-------------------------------------	             |
| ->  	| /setup/PWM        | (PIN_NUM) Integer | set (PIN_NUM) to be control as a PWM               |
| <-  	| /PWM/(ID)/status 	    | (ID) Integer      | ESP confirms by reply the (ID) of this PWM         |
| ->  	| /PWM/(ID)/run 	| (VALUE) Integer   | set (VALUE) from 0 to 255 to fadeInOut this PWM    |
| <-  	| /PWM/(ID)/run/status	| (VALUE) Integer   | get (VALUE) to confirm      |

### Servo motor

Servo are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
Control this Servo by sending /servo/pos/(ID) (VALUE)

| i/o 	| OSC Address 	    | DATA   	        | Description                         	          |
|-----	|-------------	    |--------	        |-------------------------------------	          |
| ->  	| /setup/servo      | (PIN_NUM) Integer | set (PIN_NUM) to be control as a servo          |
| <-  	| /servo(ID)/status     | (ID) Integer      | ESP confirms by reply the (ID) of this servo    |
| ->  	| /servo/(ID)/pos 	| (VALUE) Integer   | set (VALUE) from 0 to 180 to move this servo    |
| <-  	| /servo/(ID)/pos/status 	| (VALUE) Integer   | get (VALUE) to confirm      |

### DC motor

DCmotor are available on (PIN_A), (PIN_B), (PIN_SPEED) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
Control this DCmotor by sending /DCmotor/speed/(ID) (VALUE)
Control this DCmotor by sending /DCmotor/dir/(ID) (VALUE)

| i/o 	| OSC Address 	      | DATA   	                                            | Description                         	                      |
|-----	|-------------	      |--------	                                            |-------------------------------------	                      |
| ->  	| /setup/DCmotor      | (PIN_A) Integer (PIN_B) Integer (PIN_SPEED) Integer | set (PIN_A) (PIN_B) (PIN_SPEED) to be controled as a DCmotor|
| <-  	| /DCmotor/(ID)/status     | (ID) Integer      | ESP confirms by reply the (ID) of this DCmotor                                                |
| ->  	| /DCmotor/(ID)/speed | (VALUE) Integer   | set (VALUE) from 0 to 100 to set DCmotor speed                                                |
| <-  	| /DCmotor/(ID)/speed/status | (VALUE) Integer   | get (VALUE) to confirm      |
| ->  	| /DCmotor/(ID)/dir   | (VALUE) Integer   | set (VALUE) from (-1) : BACKWARD (0) : STOP (1) : FORWARD                                     |
| <-  	| /DCmotor/(ID)/dir/status | (VALUE) Integer   | get (VALUE) to confirm      |

### Stepper

##### 2 PINS

Stepper are available on (PIN_DIR), (PIN_STEP) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
Control this Stepper by sending /stepper/speed/(ID) (VALUE)
Control this Stepper by sending /stepper/dist/(ID) (VALUE)

| i/o 	| OSC Address 	      | DATA   	                        | Description                         	                    |
|-----	|-------------	      |--------	                        |-------------------------------------	                    |
| ->  	| /setup/stepper/2pin | (PIN_DIR) Integer (PIN_STEP) Integer | set (PIN_DIR) (PIN_STEP) to be controled as a Stepper|
| <-  	| /stepper/(ID)/status     | (ID) Integer                         | ESP confirms by reply the (ID) of this Stepper       |
| ->  	| /stepper/(ID)/speed | (VALUE) Integer   | set (VALUE) from 0 to 100 to set stepper speed                          |
| <-  	| /stepper/(ID)/speed/status | (VALUE) Integer   | get (VALUE) to confirm      |
| ->  	| /stepper/(ID)/dist/(ID)  | (VALUE) Integer   | set (VALUE) negative values : BACKWARD positive values : FORWARD        |
| <-  	| /stepper/(ID)/dist/status | (VALUE) Integer   | get (VALUE) to confirm      |
##### 4 PINS

Stepper are available on (PIN_A), (PIN_B), (PIN_C), (PIN_D) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
Control this Stepper by sending /stepper/speed/(ID) (VALUE)
Control this Stepper by sending /stepper/dist/(ID) (VALUE)

| i/o 	| OSC Address 	      | DATA   	                        | Description                         	                    |
|-----	|-------------	      |--------	                        |-------------------------------------	                    |
| ->  	| /setup/stepper/4pin | (PIN_A) Integer (PIN_B) Integer (PIN_C) Integer (PIN_D) Integer | set (PIN_A) (PIN_B) (PIN_C) (PIN_D) to be controled as a Stepper|
| <-  	| /stepper/(ID)/status     | (ID) Integer                    | ESP confirms by reply the (ID) of this Stepper       |
| ->  	| /stepper/(ID)/speed | (VALUE) Integer   | set (VALUE) from 0 to 100 to set stepper speed                          |
| <-  	| /stepper/(ID)/speed/status | (VALUE) Integer   | get (VALUE) to confirm      |
| ->  	| /stepper/(ID)/dist/(ID)  | (VALUE) Integer   | set (VALUE) negative values : BACKWARD positive values : FORWARD        |
| <-  	| /stepper/(ID)/dist/status | (VALUE) Integer   | get (VALUE) to confirm      |

### Oled



### MPU9250


### Accelerometer (MMA7660)

Accelerometer MMA7660 are available on (I2C port) : SDA on 21 SCL on 22</br>
(ID) will be sequentially attributed from 0 to 1</br>
(VALUE) will depend on accelerometer postion from -1 to 1

| i/o 	| OSC Address 	      | DATA   	                   | Description                         	             |
|-----	|-------------	      |--------	                   |-------------------------------------	             |
| ->  	| /setup/accel        |                            | init accelerometer on the I2C interface             |
| <-  	| /accel/(ID)/status 	  | (ID) Integer               | ESP confirms by reply the (ID) of this accelerometer|
| <-  	| /accel/(ID) 	  | (VALUE (fVec3)) float, float, float| At every loop ESP send accelerometer state (VALUE)  |
| ->  	| /accel/(ID)/delay 	| (DELAY) Integer          | set (DELAY) between 2 reading           |
| <-  	| /accel/(ID)/delay/status 	| (DELAY) Integer          | confirm the (DELAY) between 2 reading           |
