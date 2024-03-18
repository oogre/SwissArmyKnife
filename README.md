# SwissArmyKnife

SwissArmyKnife is an _Arduino_ software, developped for ESP32 board. It's been writen to help art students to develop custom hardware solution without the needs to write C/C++ code. It uses [OSC](https://fr.wikipedia.org/wiki/Open_Sound_Control) to setup and control hardwares connected on the board.

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
| <-  	| /input/status 	| (ID) Integer               | ESP confirms by reply the (ID) of this button  |
| <-  	| /input/(ID) 	    | (VALUE) Integer            | At every loop ESP send Button state (VALUE)    |

 
##### INPUT_PULLUP MODE
 
| i/o 	| OSC Address 	     | DATA   	                  | Description                         	       |
|-----	|-------------	     |--------	                  |-------------------------------------	       |
| ->  	| /setup/input_pullup| (PIN_NUM) Integer          | set (PIN_NUM) to be read as a button           |
| <-  	| /input/status 	 | (ID) Integer               | ESP confirms by reply the (ID) of this button  |
| <-  	| /input/(ID) 	     | (VALUE) Integer            | At every loop ESP send Button state (VALUE)    |

### Potentiometer

Potentiometer are available on (PIN_NUM) : 32, 33, 34, 35</br>
(ID) will be sequentially attributed from 0 to 3</br>
(VALUE) will depend on potentiometer postion from 0 to 4095

| i/o 	| OSC Address 	      | DATA   	                   | Description                         	             |
|-----	|-------------	      |--------	                   |-------------------------------------	             |
| ->  	| /setup/potentiometer| (PIN_NUM) Integer          | set (PIN_NUM) to be read as a potentiometer         |
| <-  	| /input/status 	  | (ID) Integer               | ESP confirms by reply the (ID) of this potentiometer|
| <-  	| /input/(ID) 	      | (VALUE) Integer            | At every loop ESP send potentiometer state (VALUE)  |

### Touch sensor

Touch sensor are available on (PIN_NUM) : 4, 12, 13, 14, 15, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
(VALUE) will depend on Touch sensor from 0 to ...

| i/o 	| OSC Address 	      | DATA   	                   | Description                         	             |
|-----	|-------------	      |--------	                   |-------------------------------------	             |
| ->  	| /setup/touch        | (PIN_NUM) Integer          | set (PIN_NUM) to be read as a touch sensor          |
| <-  	| /touch/status 	  | (ID) Integer               | ESP confirms by reply the (ID) of this touch sensor |
| <-  	| /touch/(ID) 	      | (VALUE) Integer            | At every loop ESP send touch sensor state (VALUE)   |

### RFID

RFID sensor are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 7</br>
(VALUE) will depend on TAG enter and leave the reader

| i/o 	| OSC Address 	     | DATA   	                  | Description                         	             |
|-----	|-------------	     |--------	                  |-------------------------------------	             |
| ->  	| /setup/rfid        | (PIN_NUM) Integer          | set (PIN_NUM) to be read as a rfid sensor (SofSerial)|
| <-  	| /rfid/status 	     | (ID) Integer               | ESP confirms by reply the (ID) of this rfid sensor   |
| <-  	| /rfid/tag/(ID) 	 | (VALUE) String             | Send when RFID TAG enter the reader                  |
| <-  	| /rfid/tag/(ID) 	 |                            | Send when RFID TAG leave the reader                  |

### Temperature Humidity

Temperature Humidity sensor are available on (PIN_NUM) : 4, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33</br>
(ID) will be sequentially attributed from 0 to 3</br>
(VALUE_A) will depend on Temperature in degree celcius</br>
(VALUE_B) will depend on Humidity percent</br>

| i/o 	| OSC Address 	      | DATA   	                            | Description                         	                       |
|-----	|-------------	      |--------	                            |-------------------------------------	                       |
| ->  	| /setup/tempHumidity | (PIN_NUM) Integer                   | set (PIN_NUM) to be read as a tempHumidity sensor            |
| <-  	| /tempHumidity/status| (ID) Integer                        | ESP confirms by reply the (ID) of this tempHumidity sensor   |
| <-  	| /tempHumidity/(ID)  | (VALUE_A) Integer (VALUE_B) Integer | At every loop ESP send temperature and Humidity sensor values|

### Ultrasonic Sensor

## Outputs

### DigitalOut

### PWM

### Servo

### DC motor

### Stepper

### Oled

### MPU9250





-> /setup/output "i"
-> /setup/dist "ii"
-> /setup/rfid "i"
-> /setup/tempHumidity "i"
-> /setup/mpu9250 ""
-> /setup/stepper/2pin "ii"
-> /setup/stepper/4pin "iiii"
-> /setup/PWM "i"
-> /setup/servo "i"
-> /setup/DCmotor "iii"
-> /setup/display ""
