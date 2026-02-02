#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
#include <string.h>


LiquidCrystal lcd(8, 9, 5, 6, 7, 3);

const int btn1 = A0;
const int btn2 = A1;
const int btn3 = A2;
const int btn4 = A3;

const int ledPin1 = 2;
const int ledPin2 = 10;
const int ledPin3 = 11;
const int ledPin4 = 12;

const char scrollText[] = "1Fut 2Gies 3Auf 4Sl                ";
const int LCD_COLS = 16;

unsigned long lastScrollMs = 0;
const unsigned long scrollIntervalMs = 250;
int scrollIndex = 0;

bool showingMsg = false;
unsigned long msgUntilMs = 0;
const unsigned long msgShowMs = 1200;

void printScrollLine2(int startIndex) {
  lcd.setCursor(0, 1);
  int len = (int)strlen(scrollText);
  for (int i = 0; i < LCD_COLS; i++) {
    char c = scrollText[(startIndex + i) % len];
    lcd.print(c);
  }
}

void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Was tun?");
  printScrollLine2(scrollIndex);
}

void showMsgNonBlocking(const char* line1, const char* line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  showingMsg = true;
  msgUntilMs = millis() + msgShowMs;
}

bool pressedOnceAnalog(int pin) {
  static unsigned long lastPressMs[4] = {0, 0, 0, 0};
  int idx = pin - A0;
  if (idx < 0 || idx > 3) return false;

  unsigned long now = millis();
  if (digitalRead(pin) == LOW) {
    if (now - lastPressMs[idx] > 180) {
      lastPressMs[idx] = now;
      return true;
    }
  }
  return false;
}

void taskLCD() {
  unsigned long now = millis();

  if (showingMsg && now >= msgUntilMs) {
    showingMsg = false;
    showMenu();
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  }

  if (!showingMsg && (now - lastScrollMs >= scrollIntervalMs)) {
    lastScrollMs = now;
    scrollIndex++;
    printScrollLine2(scrollIndex);
  }

  if (!showingMsg) {
    if (pressedOnceAnalog(btn1)) {
      digitalWrite(ledPin1, HIGH);
      showMsgNonBlocking("Lecker!", "Hat gut geschm.");
    }
    if (pressedOnceAnalog(btn2)) {
      digitalWrite(ledPin2, HIGH);
      showMsgNonBlocking("Regen! Hurra!", "");
    }
    if (pressedOnceAnalog(btn3)) {
      digitalWrite(ledPin3, HIGH);
      showMsgNonBlocking("Guten Morgen!", "");
    }
    if (pressedOnceAnalog(btn4)) {
      digitalWrite(ledPin4, HIGH);
      showMsgNonBlocking("Muede.", "Ich muss schlafen!");
    }
  }
}


#define DATA_PIN 4
#define WIDTH 32
#define HEIGHT 8
#define NUMPIXELS (WIDTH * HEIGHT)
#define BRIGHTNESS 35


#define COLUMN_WIRED true
#define SERPENTINE true
#define FLIP_X false   
#define FLIP_Y false   

Adafruit_NeoPixel pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

const int jumpBtnPin = 13;


unsigned long lastFrameMs = 0;
const unsigned long FRAME_MS = 60;   
const unsigned long PIPE_MS = 130;   


float birdYf = 0.0f;
float birdVy = 0.0f;

const float GRAVITY = 0.18f;
const float JUMP_IMPULSE = -1.70f;
const float MAX_FALL = 1.40f;

const int birdX = 5;


int pipeX[2] = {31, 16};
int prevPipeX[2] = {31, 16};
int gapTop[2] = {2, 3};
int gapSize = 2;
int pipeWidth = 1; 


int score = 0;


enum GameState { STATE_START, STATE_PLAYING, STATE_GO };
GameState state = STATE_START;

unsigned long goStartedMs = 0;
const unsigned long GO_SHOW_MS = 900;

bool lastJumpDown = false;


static inline void transformXY(int &x, int &y) {
  if (FLIP_X) x = WIDTH - 1 - x;
  if (FLIP_Y) y = HEIGHT - 1 - y;
}

int XY(int x, int y) {
  transformXY(x, y);
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return -1;

  if (COLUMN_WIRED) {
    if (!SERPENTINE) return x * HEIGHT + y;
    if (x % 2 == 0) return x * HEIGHT + y;
    return x * HEIGHT + (HEIGHT - 1 - y);
  } else {
    if (!SERPENTINE) return y * WIDTH + x;
    if (y % 2 == 0) return y * WIDTH + x;
    return y * WIDTH + (WIDTH - 1 - x);
  }
}

void setXY(int x, int y, uint32_t c) {
  int i = XY(x, y);
  if (i >= 0 && i < NUMPIXELS) pixels.setPixelColor(i, c);
}


const uint8_t DIGITS_3x5[10][5] = {
  {0b111,0b101,0b101,0b101,0b111}, // 0
  {0b010,0b110,0b010,0b010,0b111}, // 1
  {0b111,0b001,0b111,0b100,0b111}, // 2
  {0b111,0b001,0b111,0b001,0b111}, // 3
  {0b101,0b101,0b111,0b001,0b001}, // 4
  {0b111,0b100,0b111,0b001,0b111}, // 5
  {0b111,0b100,0b111,0b101,0b111}, // 6
  {0b111,0b001,0b010,0b010,0b010}, // 7
  {0b111,0b101,0b111,0b101,0b111}, // 8
  {0b111,0b101,0b111,0b001,0b111}, // 9
};

void drawDigit3x5(int x0, int y0, int d, uint32_t color) {
  if (d < 0 || d > 9) return;
  for (int row = 0; row < 5; row++) {
    uint8_t bits = DIGITS_3x5[d][row];
    for (int col = 0; col < 3; col++) {
      bool on = (bits & (1 << (2 - col))) != 0;
      if (on) setXY(x0 + col, y0 + row, color);
    }
  }
}


void drawScoreTopLeft(uint32_t color) {
  int s = score;
  if (s < 0) s = 0;
  if (s > 999) s = 999;

  int hundreds = s / 100;
  int tens = (s / 10) % 10;
  int ones = s % 10;

  int x = 0;
  int y = 0;
  int spacing = 1;

 
  if (hundreds > 0) {
    drawDigit3x5(x, y, hundreds, color);
    x += 3 + spacing;
    drawDigit3x5(x, y, tens, color);
    x += 3 + spacing;
    drawDigit3x5(x, y, ones, color);
  } else if (tens > 0) {
    drawDigit3x5(x, y, tens, color);
    x += 3 + spacing;
    drawDigit3x5(x, y, ones, color);
  } else {
    drawDigit3x5(x, y, ones, color);
  }
}


struct Glyph5x7 { char ch; uint8_t rows[7]; };
const Glyph5x7 FONT_GO[] = {
  {'G', {0b01110,0b10001,0b10000,0b10111,0b10001,0b10001,0b01110}},
  {'O', {0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110}},
};

const Glyph5x7* findGlyphGO(char c) {
  for (unsigned i = 0; i < sizeof(FONT_GO)/sizeof(FONT_GO[0]); i++) {
    if (FONT_GO[i].ch == c) return &FONT_GO[i];
  }
  return nullptr;
}

void drawChar5x7(int x0, int y0, char c, uint32_t color) {
  const Glyph5x7* g = findGlyphGO(c);
  if (!g) return;
  for (int row = 0; row < 7; row++) {
    uint8_t bits = g->rows[row];
    for (int col = 0; col < 5; col++) {
      bool on = (bits & (1 << (4 - col))) != 0;
      if (on) setXY(x0 + col, y0 + row, color);
    }
  }
}

void drawGO() {
  pixels.clear();
  uint32_t c = pixels.Color(255, 0, 0);

  
  int x0 = (WIDTH - 11) / 2;
  int y0 = 0;

  drawChar5x7(x0,     y0, 'G', c);
  drawChar5x7(x0 + 6, y0, 'O', c);

  pixels.show();
}


bool jumpPressedEdge() {
  bool down = (digitalRead(jumpBtnPin) == LOW);
  bool edge = down && !lastJumpDown;
  lastJumpDown = down;
  return edge;
}

void resetToStart() {
  state = STATE_START;

  birdYf = (HEIGHT - 1) / 2.0f;
  birdVy = 0.0f;

  pipeX[0] = WIDTH - 1;
  pipeX[1] = WIDTH / 2;
  prevPipeX[0] = pipeX[0];
  prevPipeX[1] = pipeX[1];

  gapTop[0] = random(1, 4);
  gapTop[1] = random(1, 4);

  score = 0;

  lastFrameMs = millis();
  lastPipeMs = millis();
  lastJumpDown = false;
}

void drawStartScreen() {
  pixels.clear();
  uint32_t pipeColor = pixels.Color(0, 255, 0);
  uint32_t birdColor = pixels.Color(255, 255, 0);


  for (int p = 0; p < 2; p++) {
    int x = pipeX[p];
    for (int row = 0; row < gapTop[p]; row++) setXY(x, row, pipeColor);
    for (int row = gapTop[p] + gapSize; row < HEIGHT; row++) setXY(x, row, pipeColor);
  }

  int by = (int)lround(birdYf);
  if (by < 0) by = 0;
  if (by > HEIGHT - 1) by = HEIGHT - 1;
  setXY(birdX, by, birdColor);

 
  drawScoreTopLeft(pixels.Color(255, 255, 255));

  
  setXY(WIDTH - 2, HEIGHT - 1, pixels.Color(0, 0, 255));

  pixels.show();
}


void taskFlappy() {
  unsigned long now = millis();

 
  if (state == STATE_START) {
    static unsigned long lastDraw = 0;
    if (now - lastDraw > 80) {
      lastDraw = now;
      drawStartScreen();
    }
    if (jumpPressedEdge()) {
      state = STATE_PLAYING;
      lastFrameMs = now;
      lastPipeMs = now;
      birdVy = JUMP_IMPULSE; 
    }
    return;
  }

  
  if (state == STATE_GO) {
    if (now - goStartedMs < GO_SHOW_MS) {
      if (((now / 200) % 2) == 0) drawGO();
      else { pixels.clear(); pixels.show(); }
      return;
    }
    resetToStart();
    return;
  }

  
  if (now - lastFrameMs < FRAME_MS) return;
  lastFrameMs = now;

  
  if (jumpPressedEdge()) {
    birdVy = JUMP_IMPULSE;
  }

  
  birdVy += GRAVITY;
  if (birdVy > MAX_FALL) birdVy = MAX_FALL;

  birdYf += birdVy;
  if (birdYf < 0) { birdYf = 0; birdVy = 0; }
  if (birdYf > (HEIGHT - 1)) { birdYf = (float)(HEIGHT - 1); birdVy = 0; }

  
  if (now - lastPipeMs >= PIPE_MS) {
    lastPipeMs = now;

    for (int p = 0; p < 2; p++) {
      prevPipeX[p] = pipeX[p];
      pipeX[p]--;

      
      if (prevPipeX[p] >= birdX && pipeX[p] < birdX) {
        score++;
      }

      if (pipeX[p] < 0) {
        pipeX[p] = WIDTH - 1;
        gapTop[p] = random(1, 4);
      }
    }
  }

 
  pixels.clear();
  uint32_t pipeColor = pixels.Color(0, 255, 0);
  uint32_t birdColor = pixels.Color(255, 255, 0);
  uint32_t scoreColor = pixels.Color(255, 255, 255);

  int birdY = (int)lround(birdYf);
  if (birdY < 0) birdY = 0;
  if (birdY > HEIGHT - 1) birdY = HEIGHT - 1;

  
  for (int p = 0; p < 2; p++) {
    int x = pipeX[p];

   
    for (int row = 0; row < gapTop[p]; row++) setXY(x, row, pipeColor);
   
    for (int row = gapTop[p] + gapSize; row < HEIGHT; row++) setXY(x, row, pipeColor);

    
    if (x == birdX && (birdY < gapTop[p] || birdY >= gapTop[p] + gapSize)) {
      state = STATE_GO;
      goStartedMs = now;
      return;
    }
  }

  
  setXY(birdX, birdY, birdColor);

  
  drawScoreTopLeft(scoreColor);

  pixels.show();
}


void setup() {
  
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  lcd.begin(16, 2);
  showMenu();

  
  pinMode(jumpBtnPin, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.clear();
  pixels.show();

  randomSeed(analogRead(A0));
  resetToStart();
}

void loop() {
  taskLCD();
  taskFlappy();
}
