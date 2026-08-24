#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LED_PIN 2
bool deviceConnected = false;
String currentEmotion = "IDLE";

// Unikalne adresy dla naszego Bluetooth
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; }
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) {
        // POPRAWKA: W nowej bibliotece getValue() zwraca od razu typ String
        String value = pChar->getValue();
        if (value.length() > 0) {
            currentEmotion = value;
        }
    }
};

void setup() {
    pinMode(LED_PIN, OUTPUT);
    
    // Inicjalizacja ekranu
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    
    // Inicjalizacja Bluetooth
    BLEDevice::init("Robot_Oliwii");
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
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    // Proste minki tekstowe do testów
    if(emotion == "HAPPY") { display.setCursor(35, 25); display.print("^___^"); }
    else if(emotion == "SAD") { display.setCursor(35, 25); display.print("T___T"); }
    else if(emotion == "ANGRY") { display.setCursor(35, 25); display.print(">___<"); }
    else if(emotion == "THINKING") { display.setCursor(35, 25); display.print("O___-"); }
    else if(emotion == "CONFUSED") { display.setCursor(35, 25); display.print("@___@"); }
    else if(emotion == "SURPRISED") { display.setCursor(35, 25); display.print("O___O"); }
    else if(emotion == "SCARED") { display.setCursor(35, 25); display.print("!___!"); }
    else { 
        // Stan IDLE z animacją lekkiego ruchu
        int yOffset = (millis() / 600) % 2 == 0 ? 2 : -2;
        display.setCursor(35, 25 + yOffset);
        display.print("o___o");
    }
    display.display();
}

void pulseLED() {
    // Matematyczny efekt gładkiego "oddychania" diody
    float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
    analogWrite(LED_PIN, val);
}

void loop() {
    if(currentEmotion == "IDLE") pulseLED();
    else analogWrite(LED_PIN, 0); // Wyłącz led podczas pokazywania emocji
    
    drawFace(currentEmotion);
    delay(50);
}