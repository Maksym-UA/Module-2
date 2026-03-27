ESP32 S3 Traffic lights mini project in Arduino framework.

Traffic lights:

1. Stage "Permitted to drive" (Green):
Only green light is on for 5 seconds. The program is not sleeping and
constantly checks the timer millis().

2. Stage "Attention, stop light is comming" (Green is blinking):
The green light starts blinking every 500 ms. It lasts for 3 seconds.
This code is to be written without delay() and we simultaneously count
total blinking time and each blink interval.

3. Stage "Driving not permitted" (Yellow):
Green light is off, yellow light is on for 2 seconds. It tells that the Xing needs
to stay free.

4. Stage "Stop" (Red):
Only the rel light is on for 5 seconds. All drivers are waiting.

5. Stage "Get ready" (Red & yello):
While red is still on, yellow light up. They both are on for 2 seconds.
Thi is a get ready sign.

6. The loop is over:
Yellow and read are off. the green is on again.

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
