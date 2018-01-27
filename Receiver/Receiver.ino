// Author Teemu Mäntykallio, 2017-04-07

// Define pins
#define DIR_PIN1   13
#define STEP_PIN1  12 															// Step on rising edge
#define DIR_PIN2   14
#define STEP_PIN2  27                               // Step on rising edge

HardwareSerial Serial2(2);

#include <SPI.h>
#include "RF24.h"

#include <TMC2208Stepper.h>												// Include library
TMC2208Stepper driver = TMC2208Stepper(&Serial2);	// Create driver and use
// HardwareSerial0 for communication

RF24 radio(21, 22);
byte addresses[][6] = {"1Node", "2Node"};

void setup() {
  Serial.begin(115200);														// Init used serial port
  while (!Serial);																	// Wait for port to be ready

  Serial.println("Hi!");

  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // LOW power
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening();

  Serial2.begin(115200);                            // Init used serial port
  while (!Serial2);

  // Prepare pins
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(STEP_PIN1, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);

  driver.pdn_disable(1);													// Use PDN/UART pin for communication
  driver.I_scale_analog(0);												// Adjust current from the registers
  driver.rms_current(500);												// Set driver current 500mA
  driver.toff(0x2);																// Enable driver
  driver.microsteps(256);
  driver.mstep_reg_select(1);
  driver.en_spreadCycle(false);

  digitalWrite(DIR_PIN1, LOW);
  digitalWrite(DIR_PIN2, LOW);
}

void loop() {
  if ( radio.available()) {
    // Variable for the received timestamp
    //while (radio.available()) {                                   // While there is data ready
    int len = 0;
    len = radio.getDynamicPayloadSize();
    char cha[len] = "";
    radio.read( &cha, len );             // Get the payload
    //}

    radio.stopListening();                                        // First, stop listening so we can talk
    radio.write( &cha, strlen(cha) );              // Send the final one back.
    radio.startListening();
    if (strlen(cha) > 0)
      Serial.println(cha);
  }


  /*for (int i = 0; i < 102040; i++) { //256*200*2
    digitalWrite(STEP_PIN1, !digitalRead(STEP_PIN1)); // Step
    delayMicroseconds(25);
    }
    digitalWrite(DIR_PIN1, !digitalRead(DIR_PIN1)); // Rotate*/
}
