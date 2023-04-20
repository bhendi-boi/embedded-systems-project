#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// GPIO 22 I2C SCL
// GPIO 21 I2C SDA
// T3 GPIO 15

#define SCREEN_WIDTH 128 // OLED oled width,  in pixels
#define SCREEN_HEIGHT 64 // OLED oled height, in pixels
// declare an SSD1306 oled object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define joyR T3
#define joyL T0

const unsigned long PADDLE_RATE = 48;
const unsigned long BALL_RATE = 16;
const uint8_t PADDLE_HEIGHT = 24;

// * vairbales for storing ball coordinates
uint8_t ball_x = 64, ball_y = 32;

// * variables for storing ball direction
uint8_t ball_dir_x = 1, ball_dir_y = 1;

unsigned long ball_update;
unsigned long paddle_update;
const uint8_t PLAYER_X_L = 12;
uint8_t PLAYER_Y_L = 16;

// * player coordinates
const uint8_t PLAYER_X_R = 115;
uint8_t PLAYER_Y_R = 16;

// * global state
int xRValue, xRMap;
int xLValue, xLMap;

// * variable for storing score_R
uint8_t score_R = 0;
uint8_t score_L = 0;
uint8_t prevscore_R = 0;
uint8_t prevscore_L = 0;
#define MAX_SCORE 11

int findWinner()
{
  if (score_R >= MAX_SCORE)
  {
    return 1;
  }
  if (score_L >= MAX_SCORE)
  {
    return 0;
  }
  return -1;
}

void displayWinner(int winner)
{
  if (winner == -1)
  {
    return;
  }
  if (winner == 1)
  {
    for (int i = SCREEN_WIDTH / 2 - 20; i < SCREEN_WIDTH / 2 + 20; i++)
    {
      for (int j = 10; j < 20; j++)
      {
        oled.drawPixel(i, j, BLACK);
      }
    }
    oled.display();
    delay(10);
    oled.setCursor((SCREEN_WIDTH / 2) - 25, 10);
    oled.printf("Right Won !");
    oled.display();
    delay(3000);
    return;
  }
  else
  {
    for (int i = SCREEN_WIDTH / 2 - 20; i < SCREEN_WIDTH / 2 + 20; i++)
    {
      for (int j = 10; j < 20; j++)
      {
        oled.drawPixel(i, j, BLACK);
      }
    }
    oled.display();
    delay(10);
    oled.setCursor((SCREEN_WIDTH / 2) - 25, 10);
    oled.printf("Left Won !");
    oled.display();
    delay(3000);
    return;
  }
}

void displayscore_R()
{
  if (score_L != prevscore_L || score_R != prevscore_R)
  {
    for (int i = SCREEN_WIDTH / 2 - 20; i < SCREEN_WIDTH / 2 + 20; i++)
    {
      for (int j = 10; j < 20; j++)
      {
        oled.drawPixel(i, j, BLACK);
      }
    }
    Serial.println("score_R Board updated");
    oled.display();
    delay(10);
  }
  oled.setCursor((SCREEN_WIDTH / 2) - 10, 10);
  oled.printf("%d | %d", score_L, score_R);
  oled.display();
}

void reset(int *winner)
{
  if (*winner == -1)
  {
    return;
  }
  // resetting score_Rs
  score_R = 0;
  score_L = 0;
  // resetting winner
  *winner = -1;
  // resetting player Y coordinate
  PLAYER_Y_R = 16;
  // remoivng last ball
  oled.drawPixel(ball_x, ball_y, BLACK);
  // resetting ball position and directions
  ball_x = 64;
  ball_y = 32;
  ball_dir_x = 1;
  ball_dir_y = 1;
  oled.clearDisplay();
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
  oled.clearDisplay();
}

void loop()
{
  // * determining winner and resetting thr game if a
  // * winner is found
  displayscore_R();
  prevscore_R = score_R;
  prevscore_L = score_L;
  int winner = findWinner();
  displayWinner(winner);
  reset(&winner);

  // ? input from joystick
  xRValue = analogRead(joyR);
  xRMap = map(xRValue, 0, 4095, 0, 7);
  xLValue = analogRead(joyL);
  xLMap = map(xLValue, 0, 4095, 0, 7);
  bool update = false;
  unsigned long time = millis();

  // * -1 represents no change
  // * 0 represents down and 1 represnts up
  int up_state_R = 0;
  int down_state_R = 0;
  int up_state_L = 0;
  int down_state_L = 0;
  up_state_R = xRMap == 3 ? -1 : xRMap > 3 ? 1
                                           : 0;
  down_state_R = up_state_R == -1 ? -1 : !up_state_R;
  up_state_L = xLMap == 3 ? -1 : xLMap > 3 ? 1
                                           : 0;
  down_state_L = up_state_L == -1 ? -1 : !up_state_L;

  if (time > ball_update)
  {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    // Check if we hit the vertical walls
    if (new_x == 0 || new_x == 127)
    {
      // ? ball touched right most pixel CPU score_Rs
      new_x == 127 ? score_L++ : score_R++;
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the horizontal walls.
    if (new_y == 0 || new_y == 63)
    {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    // Check if we hit the CPU paddle
    if (new_x == PLAYER_X_L && new_y >= PLAYER_Y_L && new_y <= PLAYER_Y_L + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the player paddle
    if (new_x == PLAYER_X_R && new_y >= PLAYER_Y_R && new_y <= PLAYER_Y_R + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    oled.drawPixel(ball_x, ball_y, BLACK);
    oled.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;

    update = true;
  }

  if (time > paddle_update)
  {
    paddle_update += PADDLE_RATE;

    // Player right paddle
    oled.drawFastVLine(PLAYER_X_R, PLAYER_Y_R, PADDLE_HEIGHT, BLACK);
    if (up_state_R)
    {
      PLAYER_Y_R -= 1;
    }
    if (down_state_R)
    {
      PLAYER_Y_R += 1;
    }
    up_state_R = down_state_R = false;
    if (PLAYER_Y_R < 1)
      PLAYER_Y_R = 1;
    if (PLAYER_Y_R + PADDLE_HEIGHT > 63)
      PLAYER_Y_R = 63 - PADDLE_HEIGHT;
    oled.drawFastVLine(PLAYER_X_R, PLAYER_Y_R, PADDLE_HEIGHT, WHITE);

    // Player left paddle
    oled.drawFastVLine(PLAYER_X_L, PLAYER_Y_L, PADDLE_HEIGHT, BLACK);
    if (up_state_L)
    {
      PLAYER_Y_L -= 1;
    }
    if (down_state_L)
    {
      PLAYER_Y_L += 1;
    }
    up_state_L = down_state_L = false;
    if (PLAYER_Y_L < 1)
      PLAYER_Y_L = 1;
    if (PLAYER_Y_L + PADDLE_HEIGHT > 63)
      PLAYER_Y_L = 63 - PADDLE_HEIGHT;
    oled.drawFastVLine(PLAYER_X_L, PLAYER_Y_L, PADDLE_HEIGHT, WHITE);

    update = true;
  }

  if (update)
    oled.display();
}
