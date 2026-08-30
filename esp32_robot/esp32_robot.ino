#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h> 

#define I2C_SDA 8
#define I2C_SCL 9 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool deviceConnected = false;
String currentEmotion = "IDLE";
String lastEmotion = "IDLE";
String displayText = ""; // Nowa zmienna przechowująca tekst do wyświetlenia
long emotionStartTime = 0; 

Servo armLeft;
Servo armRight;
Servo baseServo;

#define PIN_SERVO_BASE 0
#define PIN_SERVO_LEFT 1 
#define PIN_SERVO_RIGHT 2 

void moveLeft(int angle) { armLeft.write(180 - angle); }
void moveRight(int angle) { armRight.write(180 - angle); }

int limitL_Down = 20;  
int limitL_Up = 160;   
int limitR_Down = 160; 
int limitR_Up = 20;    

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; }
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        String value = pChar->getValue();
        value.trim(); 
        if (value.length() > 0) {
            // SPRAWDZANIE NOWEGO PROTOKOŁU TEKSTOWEGO
            if (value.startsWith("Z:") || value.startsWith("M:") || value.startsWith("W:")) {
                currentEmotion = "TEXT";
                displayText = value.substring(2); // Odcina przedrostek, zostawia samą wartość
            } else {
                currentEmotion = value; // Standardowe emocje twarzy
            }
        }
    }
};

void updateServos(String emotion) {
    long time = millis(); 
    
    if (emotion != lastEmotion) {
        emotionStartTime = time;
        lastEmotion = emotion;
    }
    
    long elapsed = time - emotionStartTime; 

    if (emotion == "HAPPY") {
        int baseWiggle = (elapsed < 3500) ? (sin(time / 250.0) * 20) : 0; 
        baseServo.write(90 + baseWiggle);
        if (elapsed < 2500) {
            int wave = sin(time / 250.0) * 40; 
            moveLeft(110 + wave); 
            moveRight(70 - wave); 
        } else if (elapsed < 3500) {
            int bop = sin(time / 500.0) * 15;
            moveLeft(60 + bop);
            moveRight(120 - bop);
        } else {
            moveLeft(limitL_Down); 
            moveRight(limitR_Down);
        }
    } 
    else if (emotion == "SAD") {
        int baseSigh = (elapsed < 4000) ? (sin(time / 1200.0) * 15) : 0; 
        baseServo.write(90 + baseSigh);
        int sigh = (elapsed < 4000) ? (sin(time / 1000.0) * 15) : 0; 
        moveLeft(limitL_Down + 15 + sigh); 
        moveRight(limitR_Down - 15 - sigh); 
    } 
    else if (emotion == "ANGRY") {
        int baseShake = (elapsed < 3000) ? (sin(time / 60.0) * 10) : 0; 
        baseServo.write(90 + baseShake);
        if (elapsed < 1500) {
            int shake = sin(time / 50.0) * 15;
            moveLeft(100 + shake); 
            moveRight(80 - shake);
        } else if (elapsed < 3000) {
            int twitch = (elapsed % 3000 < 300) ? 10 : 0; 
            moveLeft(90 + twitch);
            moveRight(90 - twitch);
        } else {
            moveLeft(limitL_Down); 
            moveRight(limitR_Down);
        }
    } 
    else if (emotion == "SCARED") {
        int baseShiver = (elapsed < 3000) ? (sin(time / 40.0) * 15) : 0; 
        baseServo.write(90 + baseShiver);
        if (elapsed < 1000) {
            moveLeft(limitL_Up - 10); 
            moveRight(limitR_Up + 10);
        } else if (elapsed < 3000) {
            int shiver = sin(time / 40.0) * 8;
            moveLeft(130 + shiver); 
            moveRight(50 - shiver);
        } else {
            moveLeft(limitL_Down); 
            moveRight(limitR_Down);
        }
    }
    else if (emotion == "SURPRISED") {
        baseServo.write(90); 
        if (elapsed < 2000) {
            moveLeft(limitL_Up - 20); 
            moveRight(limitR_Up + 20);
        } else {
            moveLeft(90);
            moveRight(90);
        }
    }
    else if (emotion == "THINKING") {
        int basePan = (elapsed < 5000) ? (sin(time / 1000.0) * 30) : 0;
        baseServo.write(90 + basePan); 
        
        int scratchCycle = (elapsed / 600) % 4; 
        int scratchMotion = (scratchCycle <= 1 && elapsed < 5000) ? (sin(time / 200.0) * 15) : 0;
        
        if (elapsed < 5000) {
            moveLeft(limitL_Up - 30 + abs(scratchMotion)); 
            moveRight(limitR_Down); 
        } else {
            moveLeft(limitL_Down);
            moveRight(limitR_Down);
        }
    } 
    else if (emotion == "CONFUSED") {
        int shrugBase = (elapsed < 3000) ? 120 : 90;
        baseServo.write(shrugBase); 
        int shrug = sin(elapsed / 500.0) * 20;
        if(shrug < 0) shrug = 0; 
        if(elapsed > 3000) shrug = 0; 
        moveLeft(limitL_Down + 30 + shrug); 
        moveRight(limitR_Down - 30 - shrug);
    } 
    else {
        // IDLE (Spoczynek) ORAZ TRYB TEKSTOWY
        baseServo.write(90);
        moveLeft(limitL_Down); 
        moveRight(limitR_Down); 
    }
}

void setup() {
    Wire.begin(I2C_SDA, I2C_SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    
    armLeft.setPeriodHertz(50);
    armRight.setPeriodHertz(50);
    baseServo.setPeriodHertz(50);
    
    armLeft.attach(PIN_SERVO_LEFT, 500, 2400);
    armRight.attach(PIN_SERVO_RIGHT, 500, 2400);
    baseServo.attach(PIN_SERVO_BASE, 500, 2400);
    
    baseServo.write(90);
    delay(200);
    moveLeft(limitL_Down); 
    delay(200);
    moveRight(limitR_Down);
    
    BLEDevice::init("Astroy");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE
                      );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
}

void drawFace(String emotion) {
    display.clearDisplay();
    
    // OBSŁUGA WYŚWIETLANIA TEKSTU (Zegar, Minutnik, Pogoda)
    if (emotion == "TEXT") {
        display.setTextSize(3); 
        display.setTextColor(WHITE);
        
        // Magia centrowania tekstu Adafruit GFX
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(displayText, 0, 0, &x1, &y1, &w, &h);
        display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
        
        display.print(displayText);
        display.display();
        return; // Zakończ funkcję, żeby nie rysować twarzy
    }

    long time = millis();
    bool isBlinking = (time % 3000 < 200);

    int w = 26; 
    int h = 32; 
    int leftX = 22; 
    int rightX = 128 - 22 - w; 
    int y = 16; 

    if (isBlinking) { h = 4; y = 30; }

    if (emotion == "HAPPY") {
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillRect(0, y + 16, 128, 20, BLACK); 
    } 
    else if (emotion == "SAD") {
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillTriangle(leftX, y-5, leftX+w+5, y+5, leftX, y+20, BLACK); 
        display.fillTriangle(rightX+w, y-5, rightX-5, y+5, rightX+w, y+20, BLACK);
        if(!isBlinking) display.fillCircle(leftX + w/2, y + h + ((time/50)%15), 3, WHITE); 
    } 
    else if (emotion == "ANGRY") {
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillTriangle(leftX, y-5, leftX+w+10, y+16, leftX, y+16, BLACK);
        display.fillTriangle(rightX+w, y-5, rightX-10, y+16, rightX+w, y+16, BLACK);
    } 
    else if (emotion == "SURPRISED") {
        display.fillCircle(leftX + w/2, y + h/2, 16, WHITE);
        display.fillCircle(rightX + w/2, y + h/2, 16, WHITE);
        display.fillCircle(leftX + w/2, y + h/2, 6, BLACK);
        display.fillCircle(rightX + w/2, y + h/2, 6, BLACK);
    } 
    else if (emotion == "SCARED") {
        int shake = (time / 40) % 4; 
        display.fillRoundRect(leftX + shake - 10, y, 12, 12, 4, WHITE);
        display.fillRoundRect(rightX - shake + 10, y, 12, 12, 4, WHITE);
    } 
    else if (emotion == "CONFUSED") {
        display.fillRoundRect(leftX, y, w, h, 8, WHITE); 
        display.fillRoundRect(rightX, y+12, w-10, h-12, 4, WHITE); 
    } 
    else if (emotion == "THINKING") {
        display.fillRoundRect(leftX + 8, y - 6, w, h, 8, WHITE); 
        display.fillRoundRect(rightX + 8, y - 6, w, h, 8, WHITE);
    } 
    else { 
        int offsetX = (time / 2000) % 2 == 0 ? 5 : -5; 
        display.fillRoundRect(leftX + offsetX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX + offsetX, y, w, h, 8, WHITE);
    }
    
    display.display();
}

void loop() {
    drawFace(currentEmotion);
    updateServos(currentEmotion); 
    delay(30); 
}