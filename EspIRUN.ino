#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
#include <BLE2902.h>

#define LEDPIN 16
#define NUMPIXELS 9
#define PULSEPIN 15



Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);


#define DHTPIN 4     
 

#define DHTTYPE DHT11   // DHT 11
int lightPin = 0;
int isOn = 0;

DHT dht(DHTPIN, DHTTYPE);


int averageTreshold = 0 ;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

#define TMP_UUID            "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define PULSE_UUID          "a8985fda-51aa-4f19-a777-71cf52abba1e"
#define HUM_UUID            "e94f85c8-7f57-4dbd-b8d3-2b56e107ed60"

BLECharacteristic* pCharacteristicTMP = NULL;

BLECharacteristic* pCharacteristicPULSE = NULL;

BLECharacteristic* pCharacteristicHUM = NULL;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};


void setup() {
  strip.begin();
  strip.show();
  Serial.begin(115200);
  dht.begin();

  int l = analogRead(lightPin);
  for(int i=0;i<5;i++){
    averageTreshold = averageTreshold + l;
    delay(200);
  }
  averageTreshold /= 5;

  BLEDevice::init("iRUN");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);
  //TMP
  pCharacteristicTMP = pService->createCharacteristic(
                                         TMP_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pCharacteristicTMP->addDescriptor(new BLE2902());
  pCharacteristicTMP->setCallbacks(new MyCallbacks());

  //PULSE
  pCharacteristicPULSE = pService->createCharacteristic(
                                         PULSE_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
                                       );
  
  pCharacteristicPULSE->addDescriptor(new BLE2902());
  pCharacteristicPULSE->setCallbacks(new MyCallbacks());

  //HUM
  pCharacteristicHUM = pService->createCharacteristic(
                                         HUM_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pCharacteristicHUM->addDescriptor(new BLE2902());
  pCharacteristicHUM->setCallbacks(new MyCallbacks());
  
  pService->start();

  // ----- Advertising

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  BLEAdvertisementData adv;
  adv.setName("iRUN");
  //adv.setCompleteServices(BLEUUID(SERVICE_UadUID));
  pAdvertising->setAdvertisementData(adv);

  BLEAdvertisementData adv2;
  //adv2.setName(devName.c_str());
  adv2.setCompleteServices(BLEUUID(SERVICE_UUID));
  //Serial.println("BLEUUID" + BLEUUID(SERVICE_UUID));
  pAdvertising->setScanResponseData(adv2);

  pAdvertising->start();
}

void loop() {
 
  uint8_t h = dht.readHumidity();
  uint8_t t = dht.readTemperature();

  int l = analogRead(lightPin);
  int p = analogRead(PULSEPIN);
  Serial.println(p);
  
  int lightTreshold = 700 ;
 

  //TMP
  //uint8_t tmp1d = t+48;
  //uint8_t tmp2d = t%10+48;
  char tmpString[8];
  dtostrf(t, 1, 2, tmpString);
  pCharacteristicTMP->setValue(tmpString);
  pCharacteristicTMP->notify();
  //PULSE
  p = p/30;
  char pulseString[8];
  dtostrf(p, 1, 2, pulseString);
  pCharacteristicPULSE->setValue(pulseString);
  pCharacteristicPULSE->notify();
  //HUM
  char humString[8];
  dtostrf(h, 1, 2, humString);
  pCharacteristicHUM->setValue(humString);
  pCharacteristicHUM->notify();


  delay(10);

    
    
//    if(isOn==1){
//      lightTreshold += 130;
//    }

    if(l<lightTreshold){
    //if(l<lightTreshold){
        for(int i=0;i<NUMPIXELS;i++){
        strip.setPixelColor(i, 50, 50, 50);
        strip.show();
        delay(15);
        }
        isOn = 1;
    }
    else{
        for(int i=0;i<NUMPIXELS;i++){
        strip.setPixelColor(i, 0, 0, 0);
        strip.show();
        delay(15);
        }
        isOn = 0;
    }
    
    
  
}
