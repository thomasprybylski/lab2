#include "M5Unified.h"
#include "M5GFX.h"

M5Canvas canvas(&M5.Display);

// Define StatsPacket structure to hold the CPU, memory, and battery stats
#pragma pack(1)
typedef struct {
  uint16_t cpu;    // CPU usage percentage (0-100)
  uint8_t mem;     // Memory usage percentage (0-100)
  uint8_t battery; // Battery percentage (0-100)
} StatsPacket;

double interval;                // Time interval when stopwatch is started/paused
bool isPaused = false;          // True if stopwatch is paused
double pauseTime;               // Time when stopwatch was paused
bool isReset = false;           // True if stopwatch is reset
double elapsedTime = 0;         // The actual stopwatch time in seconds

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Display.setRotation(1);
  canvas.createSprite(M5.Display.width(), M5.Display.height());
  canvas.setTextSize(2);
}

void loop() {
  M5.update();  // Update button states

  // Check if there is enough data to read the StatsPacket (e.g., 3 bytes)
  if (Serial.available() >= sizeof(StatsPacket)) {
    StatsPacket stats;
    // Read the StatsPacket from serial
    Serial.readBytes((uint8_t*)&stats, sizeof(StatsPacket));

    // Clear the canvas to avoid old data overlapping with new data
    canvas.clear(TFT_BLACK);
    
    canvas.pushSprite(0,0);
  }

  // Handle A Button (Start/Pause functionality)
  if (M5.BtnA.wasPressed()) {
    interval = millis() / 1000.0;
    isPaused = !isPaused;
    isReset = false;  // Reset is false when we press A (since it might start/pause)
  }

  // Handle B Button (Reset functionality)
  if (M5.BtnB.wasPressed()) {
    interval = millis() / 1000.0;
    elapsedTime = 0;
    isReset = true;
    isPaused = true; // Resetting also pauses the stopwatch
  }

  // If stopwatch is reset, display "Time: 0.000s"
  if (isReset) {
    canvas.setCursor(10, 10);
    canvas.printf("Time: %.3fs", elapsedTime);
  } 
  // If paused, show the paused time
  else if (isPaused) {
    pauseTime = elapsedTime; // Save current time as pauseTime
    canvas.setCursor(10, 10);
    canvas.printf("Time: %.3fs", pauseTime);
  }
  // If running, display the running stopwatch time
  else {
    elapsedTime = millis() / 1000.0 - interval + pauseTime;
    canvas.setCursor(10, 10);
    canvas.printf("Time: %.3fs", elapsedTime);
  }

  // Push the canvas to the display to show updated content
  canvas.pushSprite(0, 0);
}