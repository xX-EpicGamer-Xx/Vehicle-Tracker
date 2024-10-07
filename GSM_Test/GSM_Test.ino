#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10, 11);
String mobileNumbers[] = {"+639765274948", "+639941722189"}; // Array of mobile phone numbers to send messages
int numberOfNumbers = 2; // Number of phone numbers in the array

void setup() {
  SIM900A.begin(9600);  // Setting the baud rate of GSM Module
  Serial.begin(9600);   // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("SIM900A Ready");
  delay(100);
  Serial.println("Type s to send message or r to receive message");
}

void loop() {
  if (Serial.available() > 0)
    switch (Serial.read()) {
      case 's':
        SendMessage();
        break;
    }

  if (SIM900A.available() > 0) {
    String message = SIM900A.readString();
    message.trim();
    message.toLowerCase();
    if (message == "send location") {
      SendResponse("okay");
    }
    Serial.write(message.c_str());
  }
}

void SendMessage() {
  for (int i = 0; i < numberOfNumbers; i++) {
    Serial.println("Sending Message to " + mobileNumbers[i]);
    SIM900A.println("AT+CMGF=1");    // Sets the GSM Module in Text Mode
    delay(1000);
    Serial.println("Set SMS Number");
    SIM900A.println("AT+CMGS=\"" + mobileNumbers[i] + "\"\r"); // Use the variable for the mobile number
    delay(1000);
    Serial.println("Set SMS Content");
    SIM900A.println("Bruh"); // Message content
    delay(100);
    Serial.println("Finish");
    SIM900A.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
    Serial.println("Message has been sent to " + mobileNumbers[i] + " -> SMS Selesai dikirim");
    delay(30000);
  }
}

void SendResponse(String response) {
  SIM900A.println("AT+CMGF=1");    // Sets the GSM Module in Text Mode
  delay(1000);
  SIM900A.println("AT+CMGS=\"" + mobileNumbers[0] + "\"\r"); // Send response to the first number in the array
  delay(1000);
  SIM900A.println(response); // Response content
  delay(100);
  SIM900A.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial.println("Response sent: " + response);
}
