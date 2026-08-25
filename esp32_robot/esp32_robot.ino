#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h> 

#define I2C_SDA 21
#define I2C_SCL 19 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool deviceConnected = false;
String currentEmotion = "IDLE";
String lastEmotion = "IDLE";
long emotionStartTime = 0; 

Servo armLeft;
Servo armRight;
Servo baseServo;

#define PIN_SERVO_BASE 25
#define PIN_SERVO_LEFT 26
#define PIN_SERVO_RIGHT 27

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
            currentEmotion = value;
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
        int baseWiggle = sin(time / 150.0) * 20; 
        baseServo.write(90 + baseWiggle);
        if (elapsed < 2500) {
            int wave = sin(time / 150.0) * 40; 
            armLeft.write(110 + wave); 
            armRight.write(70 - wave); 
        } else {
            int bop = sin(time / 400.0) * 15;
            armLeft.write(60 + bop);
            armRight.write(120 - bop);
        }
    } 
    else if (emotion == "SAD") {
        int baseSigh = sin(time / 1000.0) * 15; 
        baseServo.write(90 + baseSigh);
        int sigh = sin(time / 800.0) * 15; 
        armLeft.write(limitL_Down + 15 + sigh); 
        armRight.write(limitR_Down - 15 - sigh); 
    } 
    else if (emotion == "ANGRY") {
        int baseShake = sin(time / 40.0) * 10; 
        baseServo.write(90 + baseShake);
        if (elapsed < 1500) {
            int shake = sin(time / 30.0) * 15;
            armLeft.write(100 + shake); 
            armRight.write(80 - shake);
        } else {
            int twitch = (elapsed % 3000 < 200) ? 10 : 0; 
            armLeft.write(90 + twitch);
            armRight.write(90 - twitch);
        }
    } 
    else if (emotion == "SCARED") {
        int baseShiver = sin(time / 20.0) * 15; 
        baseServo.write(90 + baseShiver);
        if (elapsed < 1000) {
            armLeft.write(limitL_Up - 10); 
            armRight.write(limitR_Up + 10);
        } else {
            int shiver = sin(time / 20.0) * 8;
            armLeft.write(130 + shiver); 
            armRight.write(50 - shiver);
        }
    }
    else if (emotion == "SURPRISED") {
        baseServo.write(90); 
        if (elapsed < 2000) {
            armLeft.write(limitL_Up - 20); 
            armRight.write(limitR_Up + 20);
        } else {
            armLeft.write(90);
            armRight.write(90);
        }
    }
    else if (emotion == "THINKING") {
        baseServo.write(90); 
        int scratchCycle = (elapsed / 500) % 4; 
        int scratchMotion = (scratchCycle <= 1) ? (sin(time / 150.0) * 15) : 0;
        armLeft.write(limitL_Up - 30 + abs(scratchMotion)); 
        armRight.write(limitR_Down); 
    } 
    else if (emotion == "CONFUSED") {
        baseServo.write(120); 
        int shrug = sin(elapsed / 400.0) * 20;
        if(shrug < 0) shrug = 0; 
        if(elapsed > 3000) shrug = 0; 
        armLeft.write(limitL_Down + 30 + shrug); 
        armRight.write(limitR_Down - 30 - shrug);
    } 
    else {
        baseServo.write(90);
        armLeft.write(limitL_Down); 
        armRight.write(limitR_Down); 
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
    armLeft.write(limitL_Down); 
    delay(200);
    armRight.write(limitR_Down);
    
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