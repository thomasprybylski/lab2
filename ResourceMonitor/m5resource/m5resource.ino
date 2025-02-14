#include "M5Unified.h"
#include "M5GFX.h"

M5Canvas canvas(&M5.Display); 

#pragma pack(1)
typedef struct {
  uint16_t cpu;
  uint8_t mem;
  uint8_t battery;
} StatsPacket;

StatsPacket packet;  // Variable to store system stats

void setup() {
  M5.begin();  // Initialize the M5StickC Plus
  Serial.begin(115200);  // Start the serial connection
  M5.Display.setRotation(1);  // Set the screen rotation
  canvas.createSprite(M5.Display.width(), M5.Display.height());  // Create an off-screen sprite for drawing
  canvas.setTextSize(2);  // Set text size for display
}

void loop() {
  canvas.fillScreen(BLACK);  // Clear the display at the beginning of each loop
  M5.update();  // Update the button states

  // Handle A button events (Start/Pause)
  if(M5.BtnA.wasPressed()) {
    Serial.write(0x00);  // Send event code for button A press
  }
  if(M5.BtnA.wasReleased()) {
    Serial.write(0x01);  // Send event code for button A release
  }

  // Handle B button event (Quit)
  if(M5.BtnB.wasReleased()) {
    Serial.write(0xFF);  // Send quit event to the Python script
  }

  // Check if there is data available to read from the Python script
  if(Serial.available() >= sizeof(StatsPacket)) {
    Serial.readBytes((uint8_t*)&packet, sizeof(StatsPacket));  // Read the system stats packet

    // Display CPU usage
    canvas.setCursor(0, 0);
    canvas.print("CPU Util: ");
    canvas.print(packet.cpu);
    canvas.println("%");

    // Display memory usage
    canvas.setCursor(0, 20);
    canvas.print("MEM Util: ");
    canvas.print(packet.mem);
    canvas.println("%");

    // Display battery percentage
    canvas.setCursor(0, 40);
    canvas.print("Battery: ");
    canvas.print(packet.battery);
    canvas.println("%");

    // Push the updated screen content to the display
    canvas.pushSprite(0, 0);
  }
}
