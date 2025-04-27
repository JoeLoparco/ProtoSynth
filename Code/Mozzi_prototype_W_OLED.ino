// Mozzi configuration for ESP32
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_INTERNAL_DAC

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// I2C pins for OLED - only used for ESP32
#define SDA_PIN 21
#define SCL_PIN 23  

// ESP32 DAC pin - GPIO25 is DAC1
#define DAC_PIN 25

// Oscillator for sound generation
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
#define CONTROL_RATE 64

// Buffer to store waveform data
byte waveform[SCREEN_WIDTH];
int waveIndex = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Mozzi Sine Wave with OLED Display");
  
  // Initialize I2C for OLED
  #ifdef ESP32
    Wire.begin(SDA_PIN, SCL_PIN);
  #else
    Wire.begin();
  #endif
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Mozzi A4 Sine");
  display.println("440 Hz");
  display.display();
  delay(1000);
  
  // Initialize Mozzi
  startMozzi(CONTROL_RATE);
  aSin.setFreq(440); // A4 note
}

void updateControl() {
  // This runs at the control rate
  // Capture audio data for display
  waveform[waveIndex] = (byte)((128 + aSin.next())>>1); // Scale to 0-128 range
  waveIndex = (waveIndex + 1) % SCREEN_WIDTH;
  
  // Update display every 4 control cycles
  static byte displayCounter = 0;
  if(++displayCounter >= 4) {
    displayCounter = 0;
    updateDisplay();
  }
}

AudioOutput_t updateAudio() {
  // This runs at the audio rate
  return MonoOutput::from8Bit(aSin.next());
}

void updateDisplay() {
  display.clearDisplay();
  
  // Draw title
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("A4 Sine: 440Hz");
  
  // Draw axes
  int centerY = SCREEN_HEIGHT/2 + 10;
  display.drawLine(0, centerY, SCREEN_WIDTH, centerY, WHITE);
  
  // Draw waveform
  for(int i = 0; i < SCREEN_WIDTH-1; i++) {
    int x1 = i;
    int y1 = centerY - map(waveform[(waveIndex + i) % SCREEN_WIDTH], 0, 255, -20, 20);
    int x2 = i + 1;
    int y2 = centerY - map(waveform[(waveIndex + i + 1) % SCREEN_WIDTH], 0, 255, -20, 20);
    
    display.drawLine(x1, y1, x2, y2, WHITE);
  }
  
  display.display();
}

void loop() {
  audioHook(); // Required for Mozzi
}
