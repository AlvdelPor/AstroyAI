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
    bool isBlinking = (time % 3000 < 200); // Szybkie mrugnięcie co 3 sekundy

    int eyeW = 28;  // Szerokość oka
    int eyeH = 34;  // Wysokość oka
    int leftX = 20; // Pozycja lewego oka
    int rightX = 128 - 20 - eyeW; // Pozycja prawego oka
    int y = 15;     // Pozycja w pionie

    if (isBlinking) { 
        eyeH = 4; // Zamknięte oczy (kreski)
        y = 30; 
    }

    if (emotion == "HAPPY") {
        // Szczęśliwe, wygięte w łuk oczy
        display.fillRoundRect(leftX, y, eyeW, eyeH, 8, WHITE);
        display.fillRoundRect(rightX, y, eyeW, eyeH, 8, WHITE);
        display.fillRect(0, y + 15, 128, 20, BLACK); // Ucinamy dół prostokątem
    }
    else if (emotion == "ANGRY") {
        // Groźne, ścięte do środka brwi
        display.fillRoundRect(leftX, y, eyeW, eyeH, 8, WHITE);
        display.fillRoundRect(rightX, y, eyeW, eyeH, 8, WHITE);
        display.fillTriangle(leftX, y-5, leftX+eyeW+10, y+15, leftX, y+15, BLACK);
        display.fillTriangle(rightX+eyeW, y-5, rightX-10, y+15, rightX+eyeW, y+15, BLACK);
    }
    else { 
        // IDLE / THINKING / SAD - rozglądanie się
        int offsetX = (time / 1500) % 2 == 0 ? 6 : -6; 
        if (emotion == "THINKING") offsetX = 0; // Skupienie na wprost

        display.fillRoundRect(leftX + offsetX, y, eyeW, eyeH, 8, WHITE);
        display.fillRoundRect(rightX + offsetX, y, eyeW, eyeH, 8, WHITE);
    }
    display.display();
}

void loop() {
    drawFace(currentEmotion);
    delay(50); // Krótkie opóźnienie dla płynności klatek
}