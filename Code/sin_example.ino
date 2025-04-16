#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C  // Common address for these displays (might be 0x3D)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Animation parameters
float frequency = 1.0;  // Controls how many cycles appear
float amplitude = 20.0; // Height of the wave
float phase = 0.0;      // Phase offset (for animation)

void setup() {
  Serial.begin(9600);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Sine Wave Display"));
  display.display();
  delay(1000);
}

void loop() {
  display.clearDisplay();
  
  // Display title
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Sine Wave"));
  
  // Draw middle line (x-axis)
  display.drawLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, WHITE);
  
  // Draw the sine wave
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    // Calculate y value for this x position (center the wave vertically)
    int y = (SCREEN_HEIGHT/2) - amplitude * sin((x * frequency * PI / 64) + phase);
    
    // Draw a pixel at this position
    display.drawPixel(x, y, WHITE);
  }
  
  // Update display
  display.display();
  
  // Animate the wave by changing phase
  phase += 0.05;
  if (phase > 2*PI) {
    phase -= 2*PI;  // Keep phase between 0 and 2π
  }
  
  delay(30);  // Small delay for smooth animation
}
