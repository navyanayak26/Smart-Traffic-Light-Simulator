
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &Wire, OLED_RESET);

// Pin connections
const int RED_LED = 2;
const int YELLOW_LED = 3;
const int GREEN_LED = 4;
const int BUTTON_PIN = 7;
const int BUZZER_PIN = 8;
const int SERVO_PIN = 9;

Servo trafficServo;

// Traffic light durations in seconds
const int GREEN_TIME = 10;
const int YELLOW_TIME = 3;
const int RED_TIME = 8;

// Display countdown and traffic light status
void showStatus(const char* colour, int secondsLeft) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SMART TRAFFIC LIGHT");
  display.println("--------------------");

  display.setTextSize(2);
  display.setCursor(0, 22);
  display.println(colour);

  display.setTextSize(1);
  display.setCursor(0, 49);
  display.print("Time left: ");
  display.print(secondsLeft);
  display.println(" sec");

  display.display();
}

// Turn on one traffic light
void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_LED, red);
  digitalWrite(YELLOW_LED, yellow);
  digitalWrite(GREEN_LED, green);
}

// Run one traffic-light phase
void runPhase(const char* colour, int duration,
              bool red, bool yellow, bool green,
              int servoAngle) {
  setLights(red, yellow, green);
  trafficServo.write(servoAngle);

  for (int t = duration; t > 0; t--) {
    showStatus(colour, t);

    // Button press requests an audible alert
    if (digitalRead(BUTTON_PIN) == LOW) {
      tone(BUZZER_PIN, 1000, 250);
      delay(300);
    }

    delay(700);
  }
}

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  trafficServo.attach(SERVO_PIN);

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Traffic Simulator");
  display.println("Starting...");
  display.display();

  delay(1500);
}

void loop() {
  // Green: traffic can move
  runPhase("GREEN", GREEN_TIME,
           false, false, true, 90);

  // Yellow: prepare to stop
  runPhase("YELLOW", YELLOW_TIME,
           false, true, false, 45);

  // Red: traffic must stop
  runPhase("RED", RED_TIME,
           true, false, false, 0);
}
