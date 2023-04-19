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

#define joyX T3

const unsigned long PADDLE_RATE = 48;
const unsigned long BALL_RATE = 16;
const uint8_t PADDLE_HEIGHT = 24;

// * vairbales for storing ball coordinates
uint8_t ball_x = 64, ball_y = 32;

// * variables for storing ball direction
uint8_t ball_dir_x = 1, ball_dir_y = 1;

unsigned long ball_update;
unsigned long paddle_update;
const uint8_t CPU_X = 12;
uint8_t cpu_y = 16;

// * player coordinates
const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

// * global state
int xValue, xMap;

// * variable for storing score
uint8_t score = 0;
uint8_t CPUScore = 0;
uint8_t prevScore = 0;
uint8_t prevCPUScore = 0;

int findWinner()
{
  if (score >= 3)
  {
    return 1;
  }
  if (CPUScore >= 3)
  {
    return 0;
  }
  return -1;
}

// ! TODO make this function display the winner
void displayWinner(int winner)
{
  if (winner == -1)
  {
    return;
  }
  if (winner == 1)
  {
    delay(2000);
    oled.setCursor(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2);
    oled.println("You Won :)");
    oled.display();
    delay(3000);
    return;
  }
  else
  {
    delay(2000);
    oled.setCursor(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2);
    oled.println("You lose");
    oled.display();
    delay(3000);
    return;
  }
}

void displayScore()
{
  if (CPUScore != prevCPUScore || score != prevScore)
  {
    for (int i = SCREEN_WIDTH / 2 - 20; i < SCREEN_WIDTH / 2 + 20; i++)
    {
      for (int j = 10; j < 20; j++)
      {
        oled.drawPixel(i, j, BLACK);
      }
    }
    Serial.println("Score Board updated");
    oled.display();
    delay(10);
  }
  oled.setCursor((SCREEN_WIDTH / 2) - 10, 10);
  oled.printf("%d | %d", CPUScore, score);
  oled.display();
}

void reset(int *winner)
{
  if (*winner == -1)
  {
    return;
  }
  // resetting scores
  score = 0;
  CPUScore = 0;
  // resetting winner
  *winner = -1;
  // resetting player Y coordinate
  player_y = 16;
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
  displayScore();
  prevScore = score;
  prevCPUScore = CPUScore;
  int winner = findWinner();
  displayWinner(winner);
  reset(&winner);

  // ? input from joystick
  xValue = analogRead(joyX);
  xMap = map(xValue, 0, 4095, 0, 7);
  bool update = false;
  unsigned long time = millis();

  // * -1 represents no change
  // * 0 represents down and 1 represnts up
  int up_state = 0;
  int down_state = 0;
  up_state = xMap == 3 ? -1 : xMap > 3 ? 1
                                       : 0;
  down_state = up_state == -1 ? -1 : !up_state;

  if (time > ball_update)
  {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    // Check if we hit the vertical walls
    if (new_x == 0 || new_x == 127)
    {
      new_x == 127 ? score++ : CPUScore++;
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
    if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the player paddle
    if (new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
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

    // CPU paddle
    oled.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
    const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
    if (cpu_y + half_paddle > ball_y)
    {
      cpu_y -= 1;
    }
    if (cpu_y + half_paddle < ball_y)
    {
      cpu_y += 1;
    }
    if (cpu_y < 1)
      cpu_y = 1;
    if (cpu_y + PADDLE_HEIGHT > 63)
      cpu_y = 63 - PADDLE_HEIGHT;
    oled.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

    // Player paddle
    oled.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
    if (up_state)
    {
      player_y -= 1;
    }
    if (down_state)
    {
      player_y += 1;
    }
    up_state = down_state = false;
    if (player_y < 1)
      player_y = 1;
    if (player_y + PADDLE_HEIGHT > 63)
      player_y = 63 - PADDLE_HEIGHT;
    oled.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

    update = true;
  }

  if (update)
    oled.display();
}
