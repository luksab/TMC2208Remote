#include <SPI.h>
#include "RF24.h"
RF24 radio(21, 22);
byte addresses[][6] = {"1Node", "2Node"};
void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  Serial.setTimeout(50);
}

void loop() {
  while (Serial.available() == 0); // wait for Serial
  char endMarker = '\n';
  String str = Serial.readStringUntil(endMarker);
  //Serial.println(getValue(str, ' ', 0));

  const int len = str.length();
  char cha[len];//str;
  str.toCharArray(cha,str.length());
  Serial.println(cha);
  radio.stopListening();

  unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
  if (!radio.write( &cha, strlen(cha) )) {
    Serial.println(F("failed"));
  }
  radio.startListening();                                    // Now, continue listening

  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

  while ( ! radio.available() ) {                            // While nothing is received
    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  }

  if ( timeout ) {                                            // Describe the results
    Serial.println(F("Failed, response timed out."));
  } else {
    char got_time[] = "sdftrhshrshrtsthrsdthjsdtjsdtjsdtjsdtjsdthjdtjudtjudthju";      // Grab the response, compare, and send to debugging spew
    radio.read( &got_time, radio.getDynamicPayloadSize() );
    unsigned long end_time = micros();

    // Spew it
    Serial.print(F("Sent "));
    Serial.print(start_time);
    Serial.print(F(", Got response "));
    Serial.print(got_time);
    Serial.print(F(", Round-trip delay "));
    Serial.print(end_time - start_time);
    Serial.println(F(" microseconds"));
  }
  delay(100);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

