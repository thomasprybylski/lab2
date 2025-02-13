#include "M5Unified.h"
#include "M5GFX.h"

M5Canvas canvas(&M5.Display); // a canvas allows you to do all drawing off-screen, helping to avoid artifacts from drawing while displaying

#pragma pack(1)
typedef struct {
  uint16_t cpu;
  uint8_t mem;
  uint8_t battery;
} StatsPacket;

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Display.setRotation(1);
  canvas.createSprite(M5.Display.width(),M5.Display.height()); 
  canvas.setTextSize(2);
}
void loop() {
  // put your main code here, to run repeatedly:
  M5.update();

  //check if there is enough data to read the StatsPacket 
  if(Serial.available() > sizeof(StatsPacket)){
      //read it
      //update the canvas with new values
  }
  //handle any inputs, which should cause "events" (a single byte with various values) to be sent over the serial port
  //you do not need a struct here, because all values fit into a single byte, but you could use one if you want

  //lastly, update the display
  canvas.pushSprite(0,0); 
}
