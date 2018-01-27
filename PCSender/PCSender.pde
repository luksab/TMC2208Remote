import processing.serial.*;
Serial myPort;
boolean pressed = false;
void setup() {
  printArray(Serial.list());
  myPort = new Serial(this, Serial.list()[1], 115200);
}
void draw() {
  if (!mousePressed) {
    pressed = false;
  }
}

void mousePressed() {
  if (mouseButton == LEFT) {
    if (!pressed) {
      pressed = true;
      int delay1 = 10;
      int delay2 = 1600;
      int rot1   = 400*256;
      int rot2   = 400*16;
      myPort.write(delay1+","+delay2+","+rot1+","+rot2+"\n");
    }
  }
}