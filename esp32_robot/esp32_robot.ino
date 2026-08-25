#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// --- NOWE PINY I2C ---
#define I2C_SDA 21
#define I2C_SCL 19 // Przepnij SCK do tego pinu!

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool deviceConnected = false;
String currentEmotion = "IDLE";

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; }
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        String value = pChar->getValue();
        if (value.length() > 0) {
            currentEmotion = value;
        }
    }
};

void setup() {
    // Ręczne wymuszenie nowych pinów dla komunikacji I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    
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
    bool isBlinking = (time % 3000 < 200); // Mrugnięcie co 3 sekundy

    int w = 26; // Szerokość oka
    int h = 32; // Wysokość oka
    int leftX = 22; 
    int rightX = 128 - 22 - w; 
    int y = 16; 

    if (isBlinking) { h = 4; y = 30; } // Zamknięte oczy przy mruganiu

    if (emotion == "HAPPY") {
        // Radosne łuki (ucina dół czarnym prostokątem)
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillRect(0, y + 16, 128, 20, BLACK); 
    } 
    else if (emotion == "SAD") {
        // Smutne, opadające na zewnątrz oczy + animowana łza
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillTriangle(leftX, y-5, leftX+w+5, y+5, leftX, y+20, BLACK); 
        display.fillTriangle(rightX+w, y-5, rightX-5, y+5, rightX+w, y+20, BLACK);
        if(!isBlinking) display.fillCircle(leftX + w/2, y + h + ((time/50)%15), 3, WHITE); 
    } 
    else if (emotion == "ANGRY") {
        // Wściekłe, ścięte do środka brwi
        display.fillRoundRect(leftX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX, y, w, h, 8, WHITE);
        display.fillTriangle(leftX, y-5, leftX+w+10, y+16, leftX, y+16, BLACK);
        display.fillTriangle(rightX+w, y-5, rightX-10, y+16, rightX+w, y+16, BLACK);
    } 
    else if (emotion == "SURPRISED") {
        // Wielkie, okrągłe oczy z czarnymi źrenicami
        display.fillCircle(leftX + w/2, y + h/2, 16, WHITE);
        display.fillCircle(rightX + w/2, y + h/2, 16, WHITE);
        display.fillCircle(leftX + w/2, y + h/2, 6, BLACK);
        display.fillCircle(rightX + w/2, y + h/2, 6, BLACK);
    } 
    else if (emotion == "SCARED") {
        // Przestraszone, małe, trzęsące się oczka
        int shake = (time / 40) % 4; 
        display.fillRoundRect(leftX + shake - 10, y, 12, 12, 4, WHITE);
        display.fillRoundRect(rightX - shake + 10, y, 12, 12, 4, WHITE);
    } 
    else if (emotion == "CONFUSED") {
        // Zdezorientowany: jedno oko duże, drugie małe
        display.fillRoundRect(leftX, y, w, h, 8, WHITE); 
        display.fillRoundRect(rightX, y+12, w-10, h-12, 4, WHITE); 
    } 
    else if (emotion == "THINKING") {
        // Zamyślony: patrzy w górny prawy róg
        display.fillRoundRect(leftX + 8, y - 6, w, h, 8, WHITE); 
        display.fillRoundRect(rightX + 8, y - 6, w, h, 8, WHITE);
    } 
    else { 
        // IDLE: Swobodne rozglądanie się
        int offsetX = (time / 2000) % 2 == 0 ? 5 : -5; 
        display.fillRoundRect(leftX + offsetX, y, w, h, 8, WHITE);
        display.fillRoundRect(rightX + offsetX, y, w, h, 8, WHITE);
    }
    
    display.display();
}

void loop() {
    drawFace(currentEmotion);
    delay(50); // Krótkie opóźnienie dla płynności klatek
}