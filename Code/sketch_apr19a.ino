// ===================================
// 1. LIBRARIES (LIBRARIES)
// ===================================
// This includes the library for I2C communication. We need this because 
// the OLED screen talks to the Arduino using the I2C protocol.
#include <Wire.h>
// This is the core graphics library from Adafruit. It provides the drawing functions.
#include <Adafruit_GFX.h>
// This is the specific library for the SSD1306 OLED controller, which manages the screen itself.
#include <Adafruit_SSD1306.h>

// ===================================
// 2. OLED SETTINGS (OLED DISPLAY CONFIGURATION)
// ===================================
// Defining the screen's resolution in pixels. Standard for this common OLED.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// The reset pin for the OLED. -1 means we are not using a dedicated hardware reset pin.
#define OLED_RESET -1 
// This creates the actual display object, passing in the dimensions, the I2C interface (&Wire), and the reset pin.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===================================
// 3. MOTOR PINS (Motor B) (MOTOR CONNECTIONS)
// ===================================
// We are controlling one motor (Motor B) here. These pins go to the motor driver (like an L298N).
const int IN3 = 4; // Input 1 for motor direction
const int IN4 = 3; // Input 2 for motor direction
const int ENB = 5; // Enable pin, used for PWM (Pulse Width Modulation) to control speed.
// Setting a constant default speed for the motor (80 out of 255).
const int MOTOR_SPEED = 80;

// ===================================
// 4. ULTRASONIC SENSOR (THE EYES OF THE ROBOT)
// ===================================
// The Trig pin sends out the ultrasonic sound wave.
const int trigPin = 10;
// The Echo pin receives the reflected sound wave.
const int echoPin = 11;
// This is the critical threshold. If the robot measures a distance less than or 
// equal to this value (15 cm), it must stop!
const int STOP_DISTANCE = 15;

// ===================================
// 5. BUZZER (THE ALARM)
// ===================================
// The digital pin connected to the buzzer.
const int buzzerPin = 6;

// ===================================
// GLOBAL VARIABLES (VARIABLES USED EVERYWHERE)
// ===================================
// This variable will store the time it takes for the sound to travel and return (in microseconds).
long duration;
// This variable will store the calculated distance (in centimeters).
int distance;

// ===================================
// SETUP (RUNS ONCE AT STARTUP)
// ===================================
void setup() {
  // Setting the motor control pins as outputs.
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Configuring the ultrasonic sensor pins. Trig sends, Echo listens.
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Setting the buzzer pin as an output so we can make noise.
  pinMode(buzzerPin, OUTPUT);

  // Initializing the OLED display: 
  // SSD1306_SWITCHCAPVCC uses the internal charge pump to generate voltage. 
  // 0x3C is the standard I2C address for this display.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clearing any junk data on the screen buffer.
  display.clearDisplay();
}

// ===================================
// DISTANCE CALCULATION (HOW WE MEASURE DISTANCE)
// ===================================
// This function sends out a pulse and calculates the distance based on the time of flight.
int calculateDistance() {
  // 1. Ensure the Trig pin is clean (low) before sending the pulse.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // 2. Send a 10-microsecond pulse (High) to trigger the sensor.
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 3. Measure the duration of the incoming pulse on the Echo pin.
  // The timeout of 25000us (25ms) prevents the code from hanging if no echo is received