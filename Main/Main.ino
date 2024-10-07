#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Create a SoftwareSerial object for GSM module communication
SoftwareSerial SIM900A(10, 11); // RX, TX for GSM
// Create a SoftwareSerial object for GPS module communication
SoftwareSerial gpsSerial(12, 13); // RX, TX for GPS
// Create a TinyGPSPlus object to handle GPS data
TinyGPSPlus gps;

// Variable to store the sender's number when receiving an SMS
String senderNumber = "";
// Pin definition for the button
const int buttonPin = 9;  
// Variable for reading the button status
int buttonState = 0;      
// Flag to ensure message is sent only once
bool messageSent = false; 

// Timing variables
unsigned long previousMillis = 0; // Stores the last time coordinates were sent
unsigned long lastSentTime = 0;   // Stores the last time message was sent
// Interval of 5 minutes between sending coordinates automatically
const long interval = 300000;     
// Cooldown of 1 minute between sending messages to avoid spamming
const long messageCooldown = 60000; 

// Array of mobile numbers to send messages to
String mobileNumbers[] = {"+639765274948", "+639941722189"}; 
// Number of mobile numbers to send messages to
int numberOfNumbers = 2; 

void setup() {
  // Set the button pin as an input
  pinMode(buttonPin, INPUT); 
  // Initialize the GSM module at 9600 baud
  SIM900A.begin(9600);  
  // Initialize the GPS module at 9600 baud
  gpsSerial.begin(9600); 
  // Initialize the serial monitor for debugging at 9600 baud
  Serial.begin(9600);   
  // Print confirmation that the GSM module is ready
  Serial.println("SIM900A Ready");
  // Delay to ensure everything is initialized properly
  delay(100);
  // Print message to prompt user input in the serial monitor
  Serial.println("Type s to send message");
  delay(90000);
}

void loop() {
  // Check if the button is pressed (LOW now means pressed due to internal pull-up)
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    // Call SendCoordinates when the button is pressed
    SendCoordinates();
    // Debounce delay to avoid multiple presses
    delay(1000); 
  }
  
  // Continuously read available data from the GPS module
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Get the current time in milliseconds
  unsigned long currentMillis = millis();
  // Check if GPS data is available, and if it has been more than 1 minute since the last message
  if (gps.location.isUpdated() && !messageSent && (currentMillis - lastSentTime > messageCooldown)) {
    // Send a message with the GPS coordinates
    delay(30000);
    SendMessage();
    // Set messageSent flag to prevent repeated sends
    messageSent = true; 
    // Update the last time a message was sent
    lastSentTime = currentMillis; 
  }

  // Automatically call SendCoordinates every 5 minutes
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    SendCoordinates();
  }

  // Check if a serial command has been received from the user
  if (Serial.available() > 0) {
    // If 's' is typed, send the message with the GPS coordinates
    switch (Serial.read()) {
      case 's':
        SendMessage();
        break;
    }
  }
}

// Method to send a message containing the GPS coordinates
void SendMessage() {
  // Check if GPS location data is available and updated
  if (gps.location.isUpdated()) {
    // Store latitude and longitude as strings with 6 decimal places
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    // Prepare the message content to be sent
    String message = "(Copy the coordinates and paste them into Google Maps)\n\nLocation: ";
    String coordinates = latitude + ", " + longitude;

    // Loop through all the mobile numbers and send the message
    for (int i = 0; i < numberOfNumbers; i++) {
      Serial.println("Sending Message to " + mobileNumbers[i]);
      SIM900A.println("AT+CMGF=1");    // Set GSM Module in Text Mode
      delay(1000);
      SIM900A.println("AT+CMGS=\"" + mobileNumbers[i] + "\"\r"); // Send to each mobile number
      delay(1000);
      SIM900A.println(message); // Send the message content
      delay(100);
      SIM900A.println((char)26); // Send CTRL+Z to indicate the end of the message
      delay(1000);
      Serial.println("Message has been sent to " + mobileNumbers[i]);
      delay(3000);
    }
    
    // Send only coordinates in the second message
    for (int i = 0; i < numberOfNumbers; i++) {
      Serial.println("Sending Message to " + mobileNumbers[i]);
      SIM900A.println("AT+CMGF=1");    // Set GSM Module in Text Mode
      delay(1000);
      SIM900A.println("AT+CMGS=\"" + mobileNumbers[i] + "\"\r"); 
      delay(1000);
      SIM900A.println(coordinates); // Send only the coordinates
      delay(100);
      SIM900A.println((char)26); 
      delay(1000);
      Serial.println("Message has been sent to " + mobileNumbers[i]);
      delay(3000); 
    }
  } else {
    // If GPS data is not available, notify the user in the serial monitor
    UnavailableData();
  }
}

// Method to send only GPS coordinates when called
void SendCoordinates() {
  // Check if GPS location data is available and updated
  if (gps.location.isUpdated()) {
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    String coordinates = latitude + ", " + longitude;

    // Send the coordinates to all mobile numbers
    for (int i = 0; i < numberOfNumbers; i++) {
      Serial.println("Sending Message to " + mobileNumbers[i]);
      SIM900A.println("AT+CMGF=1");    // Set GSM Module in Text Mode
      delay(1000);
      SIM900A.println("AT+CMGS=\"" + mobileNumbers[i] + "\"\r");
      delay(1000);
      SIM900A.println(coordinates); // Send the coordinates
      delay(100);
      SIM900A.println((char)26); 
      delay(1000);
      Serial.println("Message has been sent to " + mobileNumbers[i]);
      delay(3000);
    }
  } else {
    // Notify the user if GPS data is unavailable
    UnavailableData();
  }
}

// Method to notify users when GPS data is unavailable
void UnavailableData() {
  String message = "GPS Data is currently unavailable...";

  // Send a notification message to all mobile numbers
  for (int i = 0; i < numberOfNumbers; i++) {
    Serial.println("Sending Message to " + mobileNumbers[i]);
    SIM900A.println("AT+CMGF=1");    // Set GSM Module in Text Mode
    delay(1000);
    SIM900A.println("AT+CMGS=\"" + mobileNumbers[i] + "\"\r");
    delay(1000);
    SIM900A.println(message); // Send the "unavailable" message
    delay(100);
    SIM900A.println((char)26); 
    delay(1000);
    Serial.println("Message has been sent to " + mobileNumbers[i]);
    delay(3000);
  }
}
