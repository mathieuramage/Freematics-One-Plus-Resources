// Freematics_Verification.ino
// Author: Mathieu Ramage
// Description: This script verifies the functionality of the Freematics One+ device, including CAN bus, SD card, external I/O, WiFi, motion sensor, and LED.
// License: MIT

// Required Libraries
#include <ESP32-TWAI-CAN.hpp>
#include <WiFi.h>
#include <SD.h>
#include <Wire.h>

// Pin Definitions
#define CAN_TX_PIN GPIO_NUM_26  // GPIO pin for CAN TX
#define CAN_RX_PIN GPIO_NUM_34  // GPIO pin for CAN RX
#define SD_CS_PIN 5             // GPIO pin for SD card CS
#define BAUD_RATE 115200        // Serial monitor baud rate
#define LED_PIN 25              // GPIO pin for LED control

// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Function to initialize CAN bus
void initializeCAN() {
  Serial.println(F("[CAN Test] Initializing CAN bus..."));
  ESP32Can.setPins(CAN_TX_PIN, CAN_RX_PIN);
  ESP32Can.setSpeed(ESP32Can.convertSpeed(500));
  if (ESP32Can.begin()) {
    Serial.println(F("[CAN Test] CAN bus initialized"));
  } else {
    Serial.println(F("[CAN Test] CAN bus initialization failed"));
  }
}

// Function to test SD card
void checkSDCard() {
  Serial.println(F("[SD Card Test] Checking microSD card..."));
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("[SD Card Test] microSD card initialization failed"));
  } else {
    Serial.println(F("[SD Card Test] microSD card initialized successfully"));
    File dataFile = SD.open("test.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(F("microSD Card Test Successful."));
      dataFile.close();
      Serial.println(F("[SD Card Test] Data written to test.txt"));
    } else {
      Serial.println(F("[SD Card Test] Failed to open test.txt"));
    }
  }
}

// Function to test GPIOs
void testGPIOs() {
  Serial.println(F("[External I/O Test] Testing GPIOs..."));
  pinMode(32, INPUT);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  if (digitalRead(32) == HIGH) {
    Serial.println(F("[External I/O Test] GPIO32 reads HIGH"));
  } else {
    Serial.println(F("[External I/O Test] GPIO32 reads LOW"));
  }
}

// Function to test WiFi
void checkWiFi() {
  Serial.println(F("[WiFi Test] Connecting to WiFi..."));
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(F("."));
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\n[WiFi Test] Connected to WiFi"));
  } else {
    Serial.println(F("\n[WiFi Test] Failed to connect to WiFi"));
  }
}

// Function to test accelerometer values
void readAccelerometer() {
  Serial.println(F("[Motion Sensor Test] Reading accelerometer values..."));
  Wire.begin();
  Wire.beginTransmission(0x68);  // Assuming the ICM20948 is connected at I2C address 0x68
  if (Wire.endTransmission() == 0) {
    Serial.println(F("[Motion Sensor Test] Sensor is available."));
    // Read accelerometer values (for demonstration purposes)
    Wire.beginTransmission(0x68);
    Wire.write(0x3B); // Starting register for Accelerometer readings
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);
    int16_t AcX = Wire.read() << 8 | Wire.read();
    int16_t AcY = Wire.read() << 8 | Wire.read();
    int16_t AcZ = Wire.read() << 8 | Wire.read();
    Serial.printf("[Motion Sensor Test] Acceleration: X=%d, Y=%d, Z=%d\n", AcX, AcY, AcZ);
  } else {
    Serial.println(F("[Motion Sensor Test] Sensor not detected."));
  }
}

// Function to test LED colors
void testLEDColors() {
  Serial.println(F("[LED Test] Testing LED colors..."));
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    // Test red color
    digitalWrite(LED_PIN, HIGH);
    Serial.println(F("[LED Test] LED ON (Red)"));
    delay(500);
    digitalWrite(LED_PIN, LOW);
    Serial.println(F("[LED Test] LED OFF"));
    delay(500);

    // Test white color (if applicable)
    digitalWrite(LED_PIN, HIGH);
    Serial.println(F("[LED Test] LED ON (White)"));
    delay(500);
    digitalWrite(LED_PIN, LOW);
    Serial.println(F("[LED Test] LED OFF"));
    delay(500);

    // Test green color (if applicable)
    Serial.println(F("[LED Test] Testing other color (Green if available)..."));
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ;  // Wait for Serial Monitor to connect
  }

  // Run individual tests
  initializeCAN();
  checkSDCard();
  testGPIOs();
  checkWiFi();
  readAccelerometer();
  testLEDColors();
}

void loop() {
  CanFrame rx_frame;
  if (ESP32Can.readFrame(rx_frame, 100)) {  // Check for CAN messages
    Serial.print(F("[CAN Frame] ID: 0x"));
    Serial.print(rx_frame.identifier, HEX);
    Serial.print(F(" Data: "));
    for (int i = 0; i < rx_frame.data_length_code; i++) {
      Serial.print(F("0x"));
      if (rx_frame.data[i] < 0x10) Serial.print(F("0"));
      Serial.print(rx_frame.data[i], HEX);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  delay(5000);  // Wait before reading again
}
