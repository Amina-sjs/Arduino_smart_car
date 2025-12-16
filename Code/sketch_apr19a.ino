// ===================================
// 1. БИБЛИОТЕКИ ADAFRUIT (УЖЕ УСТАНОВЛЕНЫ)
// ===================================
#include <Wire.h> 
#include <Adafruit_GFX.h>      
#include <Adafruit_SSD1306.h>  

// Настройки дисплея
#define SCREEN_WIDTH 128    
#define SCREEN_HEIGHT 64   
#define OLED_RESET -1      
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===================================
// 2. ПИНЫ ДВИГАТЕЛЯ (Motor B) И СКОРОСТЬ
// ===================================
int IN3 = 4;
int IN4 = 3;
int ENB = 2; 
const int LOW_SPEED = 80; // Маленькая скорость

// ===================================
// 3. ПИНЫ УЛЬТРАЗВУКОВОГО ДАТЧИКА И ДИСТАНЦИЯ
// ===================================
const int trigPin = 10; 
const int echoPin = 11; 
const int stopDistance = 15; // НОВЫЙ ПОРОГ ОСТАНОВКИ: 15 см

// ===================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ===================================
long duration; 
int distance;  

// ===================================
// SETUP: Инициализация
// ===================================
void setup() {
  // Настройка двигателей и ультразвука (без изменений)
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  // Инициализация OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }
  display.clearDisplay();
}

// ===================================
// ФУНКЦИИ
// ===================================

int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void moveCar(String command) {
  if (command == "forward") {
    // Едем ВПЕРЕД по команде, но физически машинка едет НАЗАД
    // Мы инвертируем логику: (LOW, HIGH) -> (HIGH, LOW)
    digitalWrite(IN3, HIGH); // Было LOW
    digitalWrite(IN4, LOW);  // Было HIGH
    analogWrite(ENB, LOW_SPEED); 
  } else if (command == "stop") {
    // Полная остановка
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
  
  // Добавим команду "backward" для полноты, если "forward" теперь едет назад
  if (command == "backward") { 
    // Едем НАЗАД по команде, но физически машинка едет ВПЕРЕД (лицо машинки)
    digitalWrite(IN3, LOW); 
    digitalWrite(IN4, HIGH); 
    analogWrite(ENB, LOW_SPEED); 
  }
}

void showEmotion(String emotion, int dist) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); 
  
  if (emotion == "happy") {
    // Едем, все хорошо
    display.setTextSize(3); 
    display.setCursor(10, 5); 
    display.print("^_^"); // Радостная эмоция
    
    display.setTextSize(1); 
    display.setCursor(0, 40); 
    display.print("FWD! Dist:");
    display.setCursor(0, 50);
    display.print(String(dist) + " cm");

  } else if (emotion == "scared") {
    // Увидел препятствие (менее 15 см) - остановка и грустная эмоция
    display.setTextSize(3);
    display.setCursor(10, 5); 
    display.print(":("); // Грустная эмоция
    
    display.setTextSize(1);
    display.setCursor(0, 40); 
    display.print("STOP! Obstacle.");
    display.setCursor(0, 50);
    display.print(String(dist) + " cm");
  }

  display.display(); 
}

// ===================================
// LOOP: Основной цикл работы
// ===================================
void loop() {
  int currentDistance = calculateDistance();

  if (currentDistance > stopDistance || currentDistance == 0) {
    // Если расстояние > 15 см (или 0, что часто означает сбой/нет препятствия), едем
    moveCar("forward"); // Фактически едем назад, но по логике это "вперед"
    showEmotion("happy", currentDistance);
  } else {
    // Увидел препятствие (<= 15 см) - СТОП
    moveCar("stop");
    showEmotion("scared", currentDistance);
  }
  
  delay(100); 
}