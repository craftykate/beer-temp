/*
 * BEER TEMP SENSOR WITH EDIT BUTTONS
 * Author: Kate McFaul
 * Date: August 2018
 * 
 * Log temperature of beer's water bath to thingspeak.com and serial monitor if connected every 5 minutes
 * If temperature falls outside of acceptable range, text Kevin
 * If temperature WAS outside of acceptable range, text Kevin that it's normal again
 * Change RGB LED: Red for too hot, Blue for too cold, Green if fine
 * Press white edit button to edit the lower temp range. Blue button makes it colder by 1 degree, red button makes it hotter.
 * Press white edit button again to edit the upper temp range. 
 * Press it again to leave edit mode and trigger another temp reading which will automatically text you temp and ranges as confirmation.
 * 
 */

// INCLUDE LIBRARIES
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <ESP8266WiFi.h> 
#include <ThingSpeak.h>

// SET UP VARIABLES
// Acceptable temp range
int lowerRange = 64;
int upperRange = 66;
int lowestTemp = lowerRange;
int highestTemp = upperRange;
bool outOfRange = false;
// Edit Variables
int editMode = 0;
int editButton = 2;
int blueButton = 0;
int redButton = 14;
// LED pins
int RedPin = 12;
int GreenPin = 13;
int BluePin = 16;
// Blinking variables
int ledState = LOW;
unsigned long previousBlinkMillis = 0; // When led last changed state
unsigned long blinkInterval = 2000; // Blink LED every 2 seconds
// ThingSpeak variables
unsigned long previousThingMillis = 0; // When ThingSpeak was last connected to
unsigned long thingInterval = 300000; // Connect to ThingSpeak every 5 minutes (300000 ms) (must be at least 15 sec according to ThingSpeak)
// Onboard pin
#define LED_PIN 5
// Set up WiFi object
WiFiClient client;
// Variable to store current temp
float temp; // int will round temp to a whole number, float will send a text with a number like 82.50000

// WIFI AND THINGSPEAK CHANNEL PARAMETERS <==================== ADD YOUR OWN VALUES TO THIS SECTION
const char WIFI_SSID[] = "<YOUR WIFI SSID>";
const char WIFI_PSK[] = "<YOUR WIFI PASSWORD>";
unsigned long CHANNEL_ID = <THINGSPEAK CHANNEL ID>;
const char * WRITE_API_KEY = "<THINGSPEAK WRITE API KEY>";

// TEMP SENSOR SET UP
// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// THINGSPEAK FIELDS 
// Field 1: Temperature
// Field 2: Lower Range
// Field 3: Upper Range
// Field 4: Text Trigger

void setup(void)
{
  // Set up onboard LED for debugging
  pinMode(LED_PIN, OUTPUT);

  // Set up RGB LED
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  
  // Set the button pins as inputs with internal pull up resisters
  pinMode(editButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);
  
  // Start serial port
  Serial.begin(9600);

  // Start up the temperature library
  sensors.begin();

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);
}

void loop(void)
{ 
  // Store current time (well, how much time has passed since program started)
  unsigned long currentMillis = millis();

  // If not in edit mode
  if (editMode == 0) {
    
    // CHECK TEMP, POST TO THINGSPEAK
    // Check if enough time has passed to check temp (or the program just started)
    if (currentMillis - previousThingMillis >= thingInterval || previousThingMillis == 0) {
      // Replace last time with current time
      previousThingMillis = currentMillis;
      
      // Flash onboard LED to show that we're sampling
      digitalWrite(LED_PIN, LOW);
      // Change RGB LED to purple
      analogWrite(RedPin, 100);
      analogWrite(GreenPin, 0);
      analogWrite(BluePin, 100);
    
      // GET TEMP
      // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
      Serial.print("Requesting temperatures...");
      // Send the command to get temperatures
      sensors.requestTemperatures(); 
      // Use the function ByIndex, and get the temperature from the first sensor only
      temp = sensors.getTempFByIndex(0);
      Serial.println("DONE");
      
      // PRINT TEMP TO SERIAL
      Serial.print("Temperature is: ");
      Serial.println(temp);  
      Serial.print("Acceptable temp range: ");
      Serial.print(lowerRange);
      Serial.print(" to ");
      Serial.println(upperRange);
      Serial.println("");
    
      // ADD TEMP TO THINGSPEAK
      // Set Temp field
      ThingSpeak.setField(1, temp);
      // Set range values
      ThingSpeak.setField(2, lowerRange);
      ThingSpeak.setField(3, upperRange);
      
      // Check if it's out of range
      if (temp < lowerRange || temp > upperRange) {
        // Set the outOfRange variable so we know when it goes back in range the first time
        outOfRange = true;
        // Check if it's lower than it has been
        if (temp < lowestTemp) {
          // Update lowestTemp variable and reset highestTemp variable just in case
          lowestTemp = temp;
          highestTemp = upperRange;
          // Set field to text temp
          ThingSpeak.setField(4, 1);
        }
        // Check if it's higher than it has been
        if (temp > highestTemp) {
          // Update highestTemp variable and reset lowestTemp variable just in case
          highestTemp = temp;
          lowestTemp = lowerRange;
          // Set field to text temp
          ThingSpeak.setField(4, 1);
        }
        // Not out of range so it's in the normal range...
      } else {
        // Check if it was previously out of range
        if (outOfRange == true) {
          // Trigger temp text to alert that it's back in range
          ThingSpeak.setField(4, 1);
        }
        // Reset other variables now that it's normal
        highestTemp = upperRange;
        lowestTemp = lowerRange;
        outOfRange = false;
      }
      // Write all fields to ThingSpeak
      ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
      
      // Turn LED off after posting data
      digitalWrite(LED_PIN, HIGH);
    }
    
    // BLINK LED THE APPROPRIATE COLOR
    // Check if enough time has passed to blink led
    if (currentMillis - previousBlinkMillis >= blinkInterval || previousBlinkMillis == 0) {
      // Replace last time with current time
      previousBlinkMillis = currentMillis;
  
      // Toggle whether led should be on or off
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      
      if (temp < lowerRange) { // If temp is too low toggle blue led
        digitalWrite(RedPin, LOW);
        digitalWrite(GreenPin, LOW);
        digitalWrite(BluePin, ledState);
      }
      if (temp >= lowerRange && temp <= upperRange) { // If temp is in range toggle green led
        digitalWrite(RedPin, LOW);
        digitalWrite(GreenPin, ledState);
        digitalWrite(BluePin, LOW);
      }
      if (temp > upperRange) { // If temp is too high toggle red led
        digitalWrite(RedPin, ledState);
        digitalWrite(GreenPin, LOW);
        digitalWrite(BluePin, LOW);
      }
    }

    // IF EDIT BUTTON IS PRESSED
    if (digitalRead(editButton) == LOW) {
      // Change to lowerRange edit mode
      editMode = 1;
      flashBlue();
      Serial.println("Editing Lower Range");
      digitalWrite(BluePin, HIGH);
    }
  }

  // IF IN FIRST EDIT MODE EDIT THE LOWER RANGE
  if (editMode == 1) {
    // If edit button is pressed while in lowerRange edit mode
    if (digitalRead(editButton) == LOW) {
      flashRed();
      Serial.println("Editing Upper Range");
      // Change to upperRange edit mode
      editMode = 2;
      digitalWrite(RedPin, HIGH);
    }
    // If blue button is pressed while in lowerRange edit mode
    if (digitalRead(blueButton) == LOW) {
      flashBlue();
      // Lower the lowerRange
      lowerRange--;
      Serial.print("Lower Range: ");
      Serial.println(lowerRange);
      digitalWrite(BluePin, HIGH);
    }
    // If red button is pressed while in lowerRange edit mode
    if (digitalRead(redButton) == LOW) {
      flashRed();
      // Raise the lowerRange
      lowerRange++;
      Serial.print("Lower Range: ");
      Serial.println(lowerRange);
      digitalWrite(BluePin, HIGH);
    }
  }

  // IF IN SECOND EDIT MODE EDIT THE UPPER RANGE
  if (editMode == 2) {
    // If edit button is pressed while in upperRange edit mode
    if (digitalRead(editButton) == LOW) {
      digitalWrite(RedPin, LOW);
      // Reset editMode
      delay(400);
      editMode = 0;
      // Set ThingSpeak field to trigger a text
      ThingSpeak.setField(4, 1);
      // Set previousThingMillis back far enough that it will trigger another temp check/send text in 15 seconds
      previousThingMillis = millis() - 280000;
    }
    // If blue button is pressed while in upperRange edit mode
    if (digitalRead(blueButton) == LOW) {
      flashBlue();
      // Lower the upperRange
      upperRange--;
      Serial.print("Upper Range: ");
      Serial.println(upperRange);
      digitalWrite(RedPin, HIGH);
    }
    // If red button is pressed while in upperRange edit mode
    if (digitalRead(redButton) == LOW) {
      flashRed();
      // Raise the upperRange
      upperRange++;
      Serial.print("Upper Range: ");
      Serial.println(upperRange);
      digitalWrite(RedPin, HIGH);
    }
  }
}

// Flash LED blue
void flashBlue() {
  digitalWrite(RedPin, LOW);
  digitalWrite(GreenPin, LOW);
  digitalWrite(BluePin, LOW);
  delay(100);
  for (int i=0; i<3; i++) {
    digitalWrite(BluePin, HIGH);
    delay(100);
    digitalWrite(BluePin, LOW);
    delay(100);
  }
}

// Flash LED red
void flashRed() {
  digitalWrite(RedPin, LOW);
  digitalWrite(GreenPin, LOW);
  digitalWrite(BluePin, LOW);
  delay(100);
  for (int i=0; i<3; i++) {
    digitalWrite(RedPin, HIGH);
    delay(100);
    digitalWrite(RedPin, LOW);
    delay(100);
  }
}

// Attempt to connect to WiFi
void connectWiFi() {

  byte led_status = 0;

  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);

  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Blink LED while waiting for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
  }

  // Turn LED off when connected
  digitalWrite(LED_PIN, HIGH);
}
