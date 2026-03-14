Superloop and independent tasks


Manage several independent processes in one loop() without blocks via ESP32.

Three leds on the board:

LED1 blinks every 200 ms
LED2 blinks every 500 ms
LED3 bkinks every 1000 ms

SETUP
=====

 1. Install PlatformIO.

 2. Open the project in VS Code

 3. To build the project, execute the command in the VS terminal
 ```
 $ pio run
 ```
 4. To upload the project to the ESP32 run this command
 ```
 $ pio run -t upload
 ```
 5. Monitor serial output at 115200 baud
 ```
 $ pio device monitor -b 115200
 ```

CONTACT
=======

Please send you feedback to
 max.savin3@gmail.com
