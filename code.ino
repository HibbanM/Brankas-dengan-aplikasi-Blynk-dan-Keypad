/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "template_ID_BlYNK"
#define BLYNK_TEMPLATE_NAME         "nama_Device"
#define BLYNK_AUTH_TOKEN            "AUTH_TOKEN_BLYNK"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Keypad.h>
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "isi_nama_wifi";
char pass[] = "isi_password_wifi";
char auth[] = "AUTH_TOKEN_BLYNK";

// Define the pin numbers
const int reedSwitchPin = 15;  // GPIO 15
const int relayPin = 16;       // GPIO 16

// Keypad settings
#define ROW_NUM    4  // 4 rows
#define COLUMN_NUM 3  // 3 columns

char key_layout[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {D1, D2, D3, D4};  // The ESP8266 pins connect to the row pins
byte pin_column[COLUMN_NUM] = {D5, D6, D7}; // The ESP8266 pins connect to the column pins

Keypad keypad = Keypad(makeKeymap(key_layout), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Password settings
const String password_1 = "12745";  // change your password here
String input_password;

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Initialize pins
  pinMode(reedSwitchPin, INPUT_PULLUP);  // Using internal pull-up resistor
  pinMode(relayPin, OUTPUT);
  
  // Set initial relay state
  digitalWrite(relayPin, HIGH);  // Ensure relay is onn (locked) at start
  input_password.reserve(32); // maximum input characters is 32
}

void loop()
{
  Blynk.run();

  // Read the reed switch state
  int reedState = digitalRead(reedSwitchPin);
  int doorState = reedState == HIGH ? 0 : 1; // Convert 0 to open, 1 to close

  // Send the reed switch state to Blynk app
  Blynk.virtualWrite(V2, doorState);

  // Debugging output
  Serial.print("Reed Switch State: ");
  Serial.println(doorState == 1 ? "Closed" : "Open");

    // Handle keypad input
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    if (key == '*') {
      digitalWrite(relayPin, HIGH); // lock the door
      input_password = ""; // reset the input password
    } else if (key == '#') {
      if (input_password == password_1) {
        Serial.println("Valid Password => unlock the door");
        digitalWrite(relayPin, LOW);  // unlock the door for 20 seconds
      } else {
        Serial.println("Invalid Password => Try again");
      }
      input_password = ""; // reset the input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}

// This function will be called every time the Widget in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // Get value as integer

  // Set the relay based on the button value (HIGH for locked, LOW for unlocked)
  digitalWrite(relayPin, pinValue);
  
  // Debugging output
  Serial.print("Relay State: ");
  Serial.println(pinValue == HIGH ? "Locked" : "Unlocked");
}