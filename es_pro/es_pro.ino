#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gsmSerial(9, 10);  // RX, TX
SoftwareSerial gpsSerial(7, 8);   // RX, TX
TinyGPSPlus gps;                  // create GPS object

const int limitSwitchPin = 2;     // limit switch pin
bool isAccident = false;          // flag to indicate accident detection

void setup() {
  Serial.begin(9600);         // initialize serial communication for debug
  gsmSerial.begin(9600);      // initialize GSM communication
  gpsSerial.begin(9600);      // initialize GPS communication
  pinMode(limitSwitchPin, INPUT_PULLUP);  // initialize limit switch pin
}

void loop() {
  // check if limit switch is triggered
  if (digitalRead(limitSwitchPin) == LOW) {
    isAccident = true;
    Serial.println("Accident detected!");
  }

  // check if GPS data is available
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      // check if an accident has been detected
      Serial.println("check if an accident has been detected");
      if (isAccident) {
        // get latitude and longitude from GPS data
        Serial.println("get latitude and longitude from GPS data");
        String latitude = String(gps.location.lat(), 6);
        String longitude = String(gps.location.lng(), 6);

        // send SMS with location to phone number
        String message = "Accident detected at: https://www.google.com/maps?q=" + latitude + "," + longitude;
        gsmSerial.println("AT+CMGF=1");  // set SMS mode to text
        delay(1000);
        gsmSerial.println("AT+CMGS=\"9500905349\"");  // replace with phone number to send SMS to
        delay(1000);
        gsmSerial.println(message);  // send message
        delay(100);
        gsmSerial.println((char)26);  // send CTRL+Z to indicate end of message
        Serial.println("SMS sent: " + message);

        isAccident = false;  // reset accident flag
      }
    }
  }
}