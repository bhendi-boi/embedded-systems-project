#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BAR_WIDTH 15
#define BAR_HEIGHT 5

void gameSetup(void);
void gameSetup()
{
  oled.clearDisplay();
  oled.fillRect(0, 20, BAR_HEIGHT, BAR_WIDTH, WHITE);
  oled.display();
  oled.fillRect(SCREEN_WIDTH - BAR_HEIGHT - 2, 20, BAR_HEIGHT, BAR_WIDTH, WHITE);
  oled.display();
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }

  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println("Setup complete");
  oled.display();
  delay(500);
  gameSetup();
}

void loop()
{
  delay(10000);
}
