# Beer Temp Sensor / A Beginner's Guide to Getting Started with the Internet of Things
A WiFi-enabled temperature sensor that uploads data online and texts when the temp is out of range. Also, an excessively documented guide to getting started with a simple internet-enabled project. 

**CONTENTS** 
* [Background](#background)
* [The Problem to Solve](#problem)
* [The Solution](#solution)
* [Why Read This](#whyreadthis)
* [Equipment:](#equipment)
  * [ESP8266 Wifi-Microcontroller](#esp8266)
  * [Temp Sensor DS18B20](#tempsensor)
  * [RGB LED](#rgbled)
  * [Power Supply](#powersupply)
  * [Misc Bits](#miscbits)
* [Time to Build](#building)
  * [Set up the Microcontroller](#setupesp)
  * [Connect Temp Sensor](#connecttempsensor)
  * [Set up RGB LED](#setuprgbled)
* [Connect to the Internet](#connecttointernet)
  * [Set up ThinkSpeak](#setupthingspeak)
  * [Set up IFTTT](#setupifttt)
  * [Create a ThingHTTP Action](#thinghttpaction)
  * [Create ThingSpeak React App](#createreactapp)
* [Putting It All Together](#puttingitalltogether)
  * [Caveats](#caveats)
* [Conclusion](#conclusion)
* [What's Next](#whatsnext)


## Background <a name="background"></a>
In my quest for knowledge I slightly turned my focus from doing programmy things to doing programmy things with microcontrollers. There's something so cool about seeing your program running around in the wild, outside of the confines of a laptop. After getting the basics down with an Arduino Uno (well, [SparkFun's version of it](https://www.sparkfun.com/products/13975)) I decided to push myself a little bit with an Internet of Things project. The first project I could think of was connecting our downstairs closet to the internet...

## The Problem to Solve <a name="problem"></a>
When Kevin brews he needs to keep a close eye on the temperature of the beer for the first several days. This often means texts from him while he's at work checking on the temp and asking me to throw an ice pack into the water bath if necessary. Sometimes it gets hot in a hurry in the closet the beer is in or things cool down too much and don't ferment right. And sometimes I forget to check or I'm not home to tell him the temp. I hate ruining his beer. Also, I wanted a simple project that reads data from a sensor and sends that data to the internet, perhaps acting on it as well, like sending a text. This was the perfect opportunity for such a project.

## The Solution <a name="solution"></a>
A WiFi-enabled microcontroller ([ESP8266 Thing Dev Board](https://www.sparkfun.com/products/13804)) with a waterproof temperature sensor ([DS18B20](https://www.sparkfun.com/products/11050)) that transmits temperature data to [ThingSpeak.com](https://thingspeak.com) which in turn triggers [IFTTT](https://ifttt.com) to send a text when the temp gets too high or low. Ta da! Kevin gets to keep an eye on his beer while at work and will get notified if things get too hot or cold. I also put an RGB LED on it that flashes red if it's too hot, blue if it's too cold and green if it's in the proper temp range, so you can see at a glance if the temp is good without going online. 

## Why Read This <a name="whyreadthis"></a>
I'm a beginner with microcontrollers. A real beginner. I had about 5,000 WTF moments while building this thing. I had no idea what I was doing when I started or what equipment I even needed. But I figured it all out and decided to excessively document the hell out of it for any other beginners out there trying to do the same kind of thing - and so I wouldn't have to reinvent the wheel if I forgot things and wanted to build another project. 

## Equipment <a name="equipment"></a>

### WiFi-Enabled Microcontroller <a name="esp8266"></a>
**[ESP8266 Thing Dev Board](https://www.sparkfun.com/products/13804)** ($16.50) <br/>
I spent two days researching what I needed to hook up to the internet and decided I needed some version of an ESP8266. A [basic ESP8266](https://www.sparkfun.com/products/13678) is a cheap little WiFi microcontroller that allows other microcontrollers to connect to a WiFi network. I think you can even operate it on its own without another microcontroller, but it only has a few pins and it seemed awfully complex to figure out all the fussy details. The pins don't fit into a breadboard, it only operates at 3.3 volts so you'd need something to regulate the voltage to it and you'd need some way to get the programs onto it. So I kept looking and found that SparkFun (as well as plenty of others like Adafruit) make breakout boards for the ESP8266 which make working with it easier. The one I settled on was the [ESP8266 Thing Dev Board](https://www.sparkfun.com/products/13804). Here are a few reasons why:
* **Integrated FTDI USB-to-Serial chip:** Without this you'd need a special cord or an [FTDI breakout board](https://www.sparkfun.com/products/9873) to get your programs onto it. With the ESP8266 Thing Dev Board you can upload programs with a regular USB to micro-B cable. This seemed much easier (and cheaper) and less likely for me to have problems.
* **Onboard 3.3V regulator:** Plug it into a 5V wall charger or your computer and the Thing will regulate the power itself. No need for a [logic level converter](https://www.sparkfun.com/products/12009) that I don't know how to use anyway. Less chance to blow things up.
* **Cost:** While the basic ESP8266 is only 5 or so dollars, I'd still have to hook it up to another Arduino and would still need to buy the power thingy and the FTDI thingy, so at $16.50 I'd actually be saving money.
* **A few extra perks:** Like onboard LED's, the ability to program it with the Arduino IDE that I was already familiar with, a power switch and the pins already soldered on. 

### Temperature Sensor <a name="tempsensor"></a>
**[Waterproof DS18B20](https://www.sparkfun.com/products/11050)** ($9.95) <br/>
Cheap enough, long 6' cord, seemed like a good basic choice. 

### RGB LED <a name="rgbled"></a>
**[LED - RGB Clear Common Cathode](https://www.sparkfun.com/products/105)** ($1.95) <br/>
I'm sure there are much cheaper ways to get these but I was already getting everything else through SparkFun so added this along with it. I wanted an RGB LED so I could have three colors (blue for cold, red for hot and green for good) coming from one spot, instead of wrestling with three separate LEDs. 

### Power Supply <a name="powersupply"></a>
**[5V/1A USB Wall Charger](https://www.sparkfun.com/products/11456)** ($3.95) <br/>
**[USB to Micro-B Cable](https://www.sparkfun.com/products/14742)** ($3.95)  <br/>
Like I stated above, the ESP8266 Thing Dev Board has it's own power regulator so you can use a regular wall charger with it without blowing things up. You can also use the USB cord to program it. 

### Misc Bits <a name="miscbits"></a>
If you're playing along at home you'll also need some jumper wires and three 330 ohm resisters, one 4.7K ohm resistor, and a mini breadboard. 

## Time to Build <a name="building"></a>

### First Things First - Set Up the Microcontroller <a name="setupesp"></a>
Easier said than done. I followed the beginning of [this tutorial from SparkFun](https://learn.sparkfun.com/tutorials/esp8266-thing-development-board-hookup-guide) to get things up and running. I was used to working with the [Arduino IDE](https://www.arduino.cc/en/Main/Software) so I decided to stick with that. Most notably you'll need to add a board manager for the ESP8266, install the proper board manager and then choose the proper board from the dropdown menu. All details on [this page](https://learn.sparkfun.com/tutorials/esp8266-thing-development-board-hookup-guide/setting-up-arduino). Then test it out with the sample code on the same page.

This didn't work for me. I kept getting `espcomm` errors everywhere. I tried what they recommended (selecting a generic board) but that didn't work either. (I have the "SparkFun ESP8266 Thing" selected) I tried a different USB cable and that didn't work. I nearly pulled my hair out trying to figure this out. Here's what DID work: **Ground pin 0**. First make sure the board is switched on so the power light is glowing. Then attach the board to a breadboard and connect the pin 0 to ground with a jumper wire. So, connect the first pin on the right to the 4th pin down on the right. Oddly, if I power down my device and then power it back up and pin 0 is still grounded my program won't run. I have to disconnect pin 0 from ground after powering up. I dunno. 

Also, I changed a file deep in an Arduino folder and I don't know if it would have worked without this change. But it didn't hurt. I changed boards.txt file so the line `thing.upload.resetmethod=ck` was now `thing.upload.resetmethod=nodemcu`. I can't for the life of me remember how I got to that file on my Mac, but here's the path on Windows: \\Users\{UserName}\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.0.0\boards.txt

Suddenly it worked! The little onboard LED was blinking away. 

### Next - Connect the Temp Sensor <a name="connecttempsensor"></a>
Again, EASIER SAID THAN DONE. The temp sensor is straightforward, only has has three wires. Connect the black one to ground, the red one to power (I connected it to the 5V pin on the left but the 3V3 pin probably works just as well), and the white one to a pin on your board. I hooked mine up to GPIO4. Didn't work. Turns out I needed two things: the 1-wire library and a resistor. 

**Connect a 4.7K resister between signal and power** <br/>
![temp sensor](https://github.com/craftykate/beer-temp/blob/master/images/temp%20sensor%20hook%20up.png) <br/>
I colored the legs of the resistor so you can see better how it goes. See how one leg is on the same row as the white wire and the other leg is on the same row as the red wire? That's where it goes. Now you don't have to tear your hair out, too. 

**Include the proper libraries** <br/>
Go to Sketch > Include Libraries > Manage Libraries and download the OneWire library and the DallasTemperature library. I played around with it to make sure I could get it working right before moving on. I wrote a small program that takes the temp and spits it out to the serial monitor. 

**Test it with some code** <br/>
Here's my sensor set up in the Arduino IDE before the setup function:

```c++
// TEMP SENSOR SET UP
// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
```

In the setup function:

```c++
// Start up the temperature library
sensors.begin();
```

And here is how I get the temp:

```c++
// Send the command to get temperatures
sensors.requestTemperatures(); 
// Use the function ByIndex, and get the temperature from the first sensor only
temp = sensors.getTempFByIndex(0);
```

That last line gets the Farenheit from the first sensor (your only sensor). You'll obviously need more - set up the serial communication, declare your temp variable, output the temp variable to serial etc, but those are the specific lines to get the temp sensor working and hopefully get you a proper reading.

### Finally - set up the RGB LED <a name="setuprgbled"></a>
Ahhh, something I already knew how to do. This went quickly. RGB LEDs are cool. One bulb, many colors. You just need to make sure you connect the right pins to the right legs. <br/>
![rgb led](https://cdn.sparkfun.com/assets/learn_tutorials/6/3/6/RGB_LED_Pins.png) <br />
The longest leg connects to ground, the other legs connect to pins on your board **with a 330 ohm resister** between the led leg and the jumper wire to the board. Something like this: <br/>
![temp sensor](https://github.com/craftykate/beer-temp/blob/master/images/rgbledlegs.png) <br/>
If you're playing along at home, I connected the red leg to pin 12, the green leg to pin 13 and the blue leg to pin 16.

**Test it out with some code** <br/>
Before your setup function establish which color goes to which pins:
```c++
// LED pins
int RedPin = 12;
int GreenPin = 13;
int BluePin = 16;
```

Then in your loop function try turning on the various legs like this:
```c++
digitalWrite(RedPin, LOW);
digitalWrite(GreenPin, LOW);
digitalWrite(BluePin, HIGH);
```
That's it! That's all the hardware. Hopefully you're able to upload a basic blinky program to the ESP8266, you can read the correct temp and output it to serial and you can change the colors on your LED. Now we program!

Little guy in all its breadboardy glory:<br/>
![temp sensor](https://github.com/craftykate/beer-temp/blob/master/images/breadboard_version.JPG) <br/>

## Connect to the Internet <a name="connecttointernet"></a>

I learned a lot from this [Internet of Things Experiment Guide](https://learn.sparkfun.com/tutorials/internet-of-things-experiment-guide) from SparkFun. They have a lot of detail on ThingSpeak and IFTTT. I highly recommend reading through it. I'm going to refer back to it a lot here. 

### Set Up ThingSpeak <a name="setupthingspeak"></a>
ThingSpeak is a free way to log data to the internet. With a little trickery you can have ThingSpeak trigger an IFTTT event if the data you're logging satisfies certain requirements. For instance, if the beer temp is too hot my program sends the number 1 to one of my fields. When that field logs a 1 instead of a 0 ThingSpeak tells IFTTT to send me a text. It also fetches the current temp and attaches that to the text. Pretty cool! Follow the directions [on this page](https://learn.sparkfun.com/tutorials/internet-of-things-experiment-guide/configure-thingspeak) to set up ThingSpeak and get the right library. Then go to the [next page](https://learn.sparkfun.com/tutorials/internet-of-things-experiment-guide/experiment-1-temperature-and-humidity-logging) and follow the directions for setting up your channel. Right now we need two fields - the Temperature field and one called Too Hot or something. The Too Hot field will be our trigger to text if the temp gets too hot. Triggers beyond that are up to you and what you need.

I recommend looking at the SparkFun tutorial's program and pulling out just what you need to log data - like a constant for now, since we're not using their temperature sensor - to ThingSpeak. The below code probably has everything you need to log a value to the first field of your ThingSpeak channel. Remeber to put in your own values for the WiFi and Channel parameters!!!

Set up your variables:
```c++
// Necessary libraries
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

// WiFi and Channel parameters 
const char WIFI_SSID[] = "<YOUR WIFI SSID>";
const char WIFI_PSK[] = "<YOUR WIFI PASSWORD>";
unsigned long CHANNEL_ID = <THINGSPEAK CHANNEL ID>;
const char * WRITE_API_KEY = "<THINGSPEAK WRITE API KEY>";

// Pin definitions
const int LED_PIN = 5;

// Global variables
WiFiClient client;
```

Set up the LED, connect to Wifi and connect to Thingspeak:
```c++
void setup() {

  // Set up LED for debugging
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);
}
```

Set field one of your ThingSpeak to something, maybe the number 8, then send the data to ThingSpeak. Wait 20 seconds
```c++
void loop() {

  // Flash LED to show that we're sampling
  digitalWrite(LED_PIN, LOW);

  // Write the values to our ThingSpeak channel
  ThingSpeak.setField(1, 8); // <====== Change the number 8 to another value and see if it logs that 
  ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  
  // Turn LED off when we've posted the data
  digitalWrite(LED_PIN, HIGH);

  // ThingSpeak will only accept updates every 15 seconds
  delay(20000);
}
```

The function you need to connect to Wifi.
```c++
// Attempt to connect to WiFi
void connectWiFi() {

  byte led_status = 0;

  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);

  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Blink LED while we wait for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
  }

  // Turn LED off when we are connected
  digitalWrite(LED_PIN, HIGH);
}
```

Fuss around with that until you can log data to ThingSpeak

## Set up IFTTT <a name="setupifttt"></a>
IF This, Then That (IFTTT) is also free! If some requirement is satisfied, do something else. There are SO MANY triggers and things it can do in response. In our case, if we send a certain value to a field in ThingSpeak, it will send an HTTP request to IFTTT to send a text. The tutorial I've been referring back to has [a page setting up IFTTT](https://learn.sparkfun.com/tutorials/internet-of-things-experiment-guide/experiment-2-iot-buttons) that sets up three buttons - one button sends an email, the second sends a text, the third sends a tweet, so the page is a little cluttered with all those instructions. You'll want to follow the directions for Button B. 

Basically, set up an account at IFTTT, click your username then New Applet. Click on the big "+ this" then search for Webhooks and click that then follow directions. Click "Receive a web request" as the trigger and name it something like "temp_too_hot". Click "Create trigger" then you'll go back to the "if this then that" screen. Click "+ that" and choose SMS as the action. Click Connect then, in the message portion of the SMS action type something like "Temperature too hot! Temp: {{Value1}}". The `{{Value1}}` is where the temperature will be inserted later, so make sure it's typed exactly like that. 

Scroll down the page until you see the heading "Find Your IFTTT Maker Channel Secret Key" to get your IFTTT secret key. 

### Create a ThingSpeak ThingHTTP Action <a name="thinghttpaction"></a>
At ThingSpeak, click on Apps > ThingHTTP and click New ThingHTTP. This is what will contact the IFTTT applet we just made and send along our temp info. Give it a name (like, Temp Too Hot) and enter in a url. It will look like this: https://maker.ifttt.com/trigger/<IFTTT_APPLET_NAME>/with/key/<IFTTT_SECRET_KEY>. Replace <IFTTT_APPLET_NAME> with temp_too_hot or whatever you named your applet and obviously replace <IFTTT_SECRET_KEY> with the secret key you got from the step above. Mine looks like: `https://maker.ifttt.com/trigger/out_of_range/with/key/xxxxxxxxxxxxxxxxxxxxxx` The method should be POST and make sure to enter application/json in the Content Type field. In the body type `{"value1":"%%channel_<YOUR_CHANNEL_ID>_field_1%%"}` replacing <YOUR_CHANNEL_ID> with your own info. This will grab the first field from your channel and send it along to IFTTT as value 1. If you are copy/pasting make sure your quotes aren't stylized quotes! 

### Create ThingSpeak React App <a name="createreactapp"></a>
On ThingSpeak, click Apps again, and then React, then click New React. This is what will check our data and see if it meets our requirements for a text. If so it will launch the ThingHTTP we made in the last step. Give it a name (Temp Too High), Condition Type is Numeric, Test Frequency is On Data Insertion. For the condition, choose the field 2 (the too high field) is equal to 1. For the action, perform ThingHTTP Temp Too Hot. I have it run this every time the condition is met, but you can set it to run only if the condition was previously false and now it's true.

Phew! That was a lot of work. Test it all out to see if it works. In the program we wrote earlier change the line `ThingSpeak.setField(1, 8);` to `ThingSpeak.setField(1, 87);` because 87 seems like a hot temp. Below that type `ThingSpeak.setField(2, 1);` which will send the value 1 to the second field. Remember this is our trigger field. If it equals 1 it's supposed to send us a text with the temp (87). Upload the program and let it run for a minute. You should get a few texts saying "Temp too hot! Temp: 87". Turn the board off so you don't get 5,000 texts! Make sure you get a text and it includes the temp. Change the 2 field line to `ThingSpeak.setField(2, 0);` and turn it on again, upload and make sure you don't get texts. 

## Putting It All Together <a name="puttingitalltogether"></a>
If that worked then you have a functioning connection between your program, ThingSpeak and IFTTT! Your program can now get on the WiFi and log data to the internet and text you if a condition is met. You can also get a temp reading using OneWire and DallasTemperature. And you can change the colors of an RGB LED. How you put all that together is mostly up to you and what you need from your program. I've tried giving you the building blocks for each element so you can modify it how you like. You can check out my beer_temp.ino file for exactly how I wrote mine. Hopefully it's commented enough so you can see what's going on. My program basically does the following:

1. Connect to WiFi to communicate with the internet when needed
2. Take a temperature reading 
3. Send the temperature to ThingSpeak so it can be accessed remotely over the internet
4. Check if temperature is too high or too low. If it is, send a signal to ThingSpeak so to trigger IFTTT to send a text.
5. Blink the LED blue if it's too cold, red if it's too hot and green if it's in the right range 
6. Wait until enough time has passed before taking another temperature reading. 

### Caveats <a name="caveats"></a>
**WHY NOT TO - AND WHY TO - USE DELAY()** <br/>
You might notice that when I blinked my LED I didn't use the delay() function. Instead I use millis() and created a variable to see how long ago it last blinked etc. Because if I delayed while blinking the program wouldn't check the temp. Why? [this post](https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay) does a great job of explaining it and explains the solution. I also ran the temp sensor checking loop with the same millis() logic, so the light could blink between checking the temp.

Another thing about delays is that you should build them into your program here and there. Maybe just a delay(100). SparkFun says it best [here](https://learn.sparkfun.com/tutorials/esp8266-thing-development-board-hookup-guide/example-sketch-web-server): 
> Delays allow the ESP8266 to perform critical tasks defined outside of the sketch. These tasks include setting up, and maintaining, a WiFi connection. Potentially infinite loops are generally dangerous. Add delays -- allowing the processor to perform other tasks -- wherever possible.

**C++ IS NOT MY FIRST PROGRAMMING LANGUAGE** <br/>
Or my second or third or fourth or 9th. Maybe somewhere around 15, I've lost count. In fact, I wouldn't even say that I know it. I just know enough _other_ languages that I could figure out what I wanted to do. I'm 100% positive there are tighter, better ways to write my code. I can see ways to refactor my code and take out extra fluff but I'm leaving it as is so that it's clear to you - and me later - what each part does. 

## In Conclusion <a name="conclusion"></a>
I hope you enjoyed that and if you're a n00b like I was you feel like you learned something. I'm still learning and I bet I got a few things wrong here and there, but all this worked for me so hopefully it can help someone else. 

## What's Next <a name="whatsnext"></a>
Most of my program involves logic depending on the temperature reading and the range it falls in. The acceptable temperature range gets hardcoded into the program. When Kevin brews a new beer I'll change the values in the program and upload it again. But now that I'm done building the thing Kevin has decided he wants a way to change the range himself. The first 24 hours the beer will need to be in one range and then for several days after it will need to be in another. My first thought was that I could drastically simplify the whole thing and just send the temperature to ThingSpeak and let ThingSpeak analize the temp - if it's under or over certain values trigger the IFTTT. However, that would require Kevin getting into the ThingSpeak React apps online and fiddling with fields and he's soooo not a computer guy and isn't going to do well with that. So I'm going to think up another way to do this. 

Eventually I'd like to communicate with the device away remotely - either with a remote server or through an Alexa skill, but I don't know enough about the security issues with setting up remote servers so until I know more I'm not messing with that! The Alexa skill seems like an interesting challenge, though. I could say, "Alexa, set the temp sensor's upper range to 70 degrees" or something. 
