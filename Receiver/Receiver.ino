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

unsigned long start1;
unsigned long start2;
int delay1 = 100;
int delay2 = 100;
int rot1  = 0;
int rot2  = 0;
unsigned long rotation1 = 0;
bool start = false;
unsigned long startPosX = 0;
long startPosY = 0;
unsigned long endPosX = 0;
long endPosY = 0;
unsigned long posX = 0;
long posY = 0;


void setup() {
  Serial.begin(115200);														// Init used serial port
  while (!Serial);																// Wait for port to be ready

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
  start1 = micros();
  start2 = micros();
}

void loop() {
  if ( radio.available()) {
    int len = 0;
    len = radio.getDynamicPayloadSize();
    char cha[len] = "";
    radio.read( &cha, len );

    radio.stopListening();
    radio.write( &cha, strlen(cha) ); //send back controll
    radio.startListening();
    if (strlen(cha) > 0) {
      char** args = str_split(cha, ',');
      //for (int i=0;i<5;i++)
      //  Serial.println(args[i]);
      if (strcmp(args[0], "startPos") == 0) {
        startPosX = posX;
        startPosY = posY;
      }
      else if (strcmp(args[0], "EndPos") == 0) {
        endPosX = posX;
        endPosY = posY;
      }
      else if (strcmp(args[0], "start") == 0) {
        start = true;
        goalPosX = startPosX;
        goalPosY = startPosY
      }
      else if (strcmp(args[0], "restart") == 0) {
        driver.pdn_disable(1);                          // Use PDN/UART pin for communication
        driver.I_scale_analog(0);                       // Adjust current from the registers
        driver.rms_current(500);                        // Set driver current 500mA
        driver.toff(0x2);                               // Enable driver
        driver.microsteps(256);
        driver.mstep_reg_select(1);
        driver.en_spreadCycle(false);
      }
      else {
        delay1 = atoi(args[0]);
        delay2 = atoi(args[1]);
        rot1   = atoi(args[2]);
        rot2   = atoi(args[3]);
      }
    }
  }

  //Serial.println(getValue(cha, ' ', 0));
  if (!(micros() - start1 < delay1)) {
    start1 = micros();
    if ((rot1 > 0) && rotation1 > 0) {
      digitalWrite(DIR_PIN1, HIGH);
      if (!justRotateY)
        rot1--;
      digitalWrite(STEP_PIN1, !digitalRead(STEP_PIN1)); // Step
      rotation1--;
    }
    if (rot1 < 0) {
      digitalWrite(DIR_PIN1, LOW);
      if (!justRotateY)
        rot1++;
      digitalWrite(STEP_PIN1, !digitalRead(STEP_PIN1)); // Step
      rotation1++;
    }
  }

  if (!(micros() - start2 < delay2)) {
    start2 = micros();
    if (rot2 > 0) {
      digitalWrite(DIR_PIN2, LOW);
      if (!justRotateX)
        rot2--;
      digitalWrite(STEP_PIN2, !digitalRead(STEP_PIN2)); // Step
    } else if (rot2 < 0) {
      digitalWrite(DIR_PIN2, HIGH);
      if (!justRotateY)
        rot2++;
      digitalWrite(STEP_PIN2, !digitalRead(STEP_PIN2)); // Step
    }
  }
}

char** str_split(char* a_str, const char a_delim)
{
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
  {
    if (a_delim == *tmp)
    {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = (char**)malloc(sizeof(char*) * count);

  if (result)
  {
    size_t idx  = 0;
    char* token = strtok(a_str, delim);

    while (token)
    {
      assert(idx < count);
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    assert(idx == count - 1);
    *(result + idx) = 0;
  }

  return result;
}


