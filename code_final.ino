#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_sleep.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Preferences prefs;

const int ledPins[4] = {25, 26, 27, 14};
const int ledPinsbegin = 33;
const int buttonPins[4] = {17, 5, 18, 19};
const int buzzerPin = 16;
const int switchPin = 4;

const int MAX_SEQUENCE = 100;
int sequence[MAX_SEQUENCE];
int sequenceLength = 0;
int highScore = 0;

enum Difficulty { EASY, NORMAL, HARD, EXTREME };
Difficulty currentDifficulty = NORMAL;

const char* difficultyNames[] = {"Facile", "Normale", "Difficile", "Extreme"};
int difficultySpeed[] = {600, 400, 250, 150};
int difficultyAdd[] = {1, 1, 2, 3};

// === SETUP ===
void setup() {
  Serial.begin(115200);

  digitalWrite(ledPinsbegin, LOW);

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(ledPinsbegin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (digitalRead(switchPin) == LOW) {
    showMessage("Switch OFF", "Veille...");
    digitalWrite(ledPinsbegin, LOW);
    delay(1000);
    display.clearDisplay();
    display.display();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)switchPin, 1);
    esp_deep_sleep_start();
  }

  digitalWrite(ledPinsbegin, HIGH);

  prefs.begin("simonGame", false);
  highScore = prefs.getInt("highScore", 0);

  display.setCursor(0, 0);
  display.println("Bienvenue dans");
  display.println("Simon Game !");
  display.display();
  delay(1000);

  drawStartAnimation();
  welcomeAnimation();
  showDifficultyMenu();
}

// === LOOP ===
void loop() {
  if (checkSwitchAndSleep()) return;

  addToSequence();
  showMessage("Regarde bien", "");
  playSequence();

  showMessage("A toi de jouer", "");
  if (!getPlayerInput()) {
  drawGameOverCross();
  playGameOverSound();
  delay(2000);

  showMessage("Perdu !", "Retour au menu...");
  delay(1500);

  sequenceLength = 0;
  showDifficultyMenu();  // Revenir au menu
  return; // Redémarrer le jeu depuis le début
  } else {
    showMessage("Bien joue !", "");
    if (sequenceLength > highScore) {
      highScore = sequenceLength;
      prefs.putInt("highScore", highScore);
    }
    delay(1000);
    char buffer[16];
    sprintf(buffer, "Score: %d", sequenceLength);
    char buffer2[16];
    sprintf(buffer2, "Record: %d", highScore);
    showMessage(buffer, buffer2);
    delay(2000);
  }
}

// === ANIMATIONS ===
void welcomeAnimation() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH);
      tone(buzzerPin, 300 + j * 100, 100);
      delay(100);
      digitalWrite(ledPins[j], LOW);
    }
    delay(200);
  }
}

void drawStartAnimation() {
  int size = 10;
  for (int i = 0; i < SCREEN_WIDTH - size; i += 4) {
    display.clearDisplay();
    display.fillRect(i, SCREEN_HEIGHT / 2 - size / 2, size, size, SSD1306_WHITE);
    display.display();
    delay(30);
  }
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Pret ?");
  display.display();
  delay(1000);
}

void drawGameOverCross() {
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.drawLine(30, 20, 90, 44, SSD1306_WHITE);
    display.drawLine(30, 44, 90, 20, SSD1306_WHITE);
    display.display();
    delay(300);
    display.clearDisplay();
    display.display();
    delay(300);
  }
}

// === JEU ===
void addToSequence() {
  for (int i = 0; i < difficultyAdd[currentDifficulty]; i++) {
    if (sequenceLength < MAX_SEQUENCE) {
      sequence[sequenceLength++] = random(0, 4);
    }
  }
}

void playSequence() {
  int delayTime = difficultySpeed[currentDifficulty];
  for (int i = 0; i < sequenceLength; i++) {
    if (checkSwitchAndSleep()) return;

    int idx = sequence[i];
    digitalWrite(ledPins[idx], HIGH);
    tone(buzzerPin, 440 + 100 * idx, delayTime);
    delay(delayTime);
    digitalWrite(ledPins[idx], LOW);
    delay(150);
  }
}

bool getPlayerInput() {
  for (int i = 0; i < sequenceLength; i++) {
    int input = waitForButtonPress();
    if (input != sequence[i]) {
      return false;
    }
  }
  return true;
}

int waitForButtonPress() {
  while (true) {
    if (checkSwitchAndSleep()) return -1;

    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        tone(buzzerPin, 440 + 100 * i, 200);
        digitalWrite(ledPins[i], HIGH);
        delay(200);
        digitalWrite(ledPins[i], LOW);
        while (digitalRead(buttonPins[i]) == LOW) delay(10);
        return i;
      }
    }
  }
}

void playGameOverSound() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 200, 150);
    for (int j = 0; j < 4; j++) digitalWrite(ledPins[j], HIGH);
    delay(150);
    for (int j = 0; j < 4; j++) digitalWrite(ledPins[j], LOW);
    delay(150);
  }
}

// === OLED ===
void showMessage(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}

// === MENU DIFFICULTÉ ===
void showDifficultyMenu() {
  int selected = 1;
  showDifficultyScreen(selected);

  while (true) {
    if (checkSwitchAndSleep()) return;

    if (digitalRead(buttonPins[2]) == LOW) {
      while (digitalRead(buttonPins[2]) == LOW) delay(10);
      selected = (selected - 1 + 4) % 4;
      showDifficultyScreen(selected);
      delay(200);
    }

    if (digitalRead(buttonPins[3]) == LOW) {
      while (digitalRead(buttonPins[3]) == LOW) delay(10);
      selected = (selected + 1) % 4;
      showDifficultyScreen(selected);
      delay(200);
    }

    if (digitalRead(buttonPins[0]) == LOW) {
      while (digitalRead(buttonPins[0]) == LOW) delay(10);
      currentDifficulty = (Difficulty)selected;

      // Son différent par difficulté
      int confirmTones[] = {500, 700, 900, 1100};
      tone(buzzerPin, confirmTones[selected], 300);
      delay(300);
      noTone(buzzerPin);

      // Confirmation visuelle : clignotement LED
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPins[selected], HIGH);
        delay(150);
        digitalWrite(ledPins[selected], LOW);
        delay(150);
      }

      showMessage("Mode choisi :", difficultyNames[selected]);
      delay(1000);
      return;
    }
  }
}

void showDifficultyScreen(int index) {
  const int colorLeds[] = {25, 26, 27, 14};
  const char* icons[] = {"*", "**", "!!!", "🔥"};

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Choix du niveau:");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(difficultyNames[index]);

  display.setTextSize(1);
  display.setCursor(0, 45);
  display.println("Rouge: ^  Vert: v");
  display.setCursor(0, 55);
  display.println("Jaune: OK");

  display.display();

  for (int i = 0; i < 2; i++) {
    digitalWrite(colorLeds[index], HIGH);
    delay(200);
    digitalWrite(colorLeds[index], LOW);
    delay(200);
  }
}

// === VEILLE ===
bool checkSwitchAndSleep() {
  if (digitalRead(switchPin) == LOW) {
    showMessage("Switch OFF", "Veille...");
    digitalWrite(ledPinsbegin, LOW);
    delay(1000);
    display.clearDisplay();
    display.display();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)switchPin, 1);
    esp_deep_sleep_start();
    return true;
  }
  return false;
}
