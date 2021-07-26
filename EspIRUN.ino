/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
#include <BLE2902.h>
 


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

#define TMP_UUID            "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define PULSE_UUID          "a8985fda-51aa-4f19-a777-71cf52abba1e"
#define HUM_UUID            "e94f85c8-7f57-4dbd-b8d3-2b56e107ed60"

BLECharacteristic* pCharacteristicTMP = NULL;

BLECharacteristic* pCharacteristicPULSE = NULL;

BLECharacteristic* pCharacteristicHUM = NULL;

uint8_t count = 0;

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
  Serial.begin(115200);

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
  // put your main code here, to run repeatedly:
  count = count + 1;
  uint8_t countt = count/10+48;
  Serial.println(countt);
  //String test = "toto";
  //TMP
  pCharacteristicTMP->setValue(&countt, 1);
  pCharacteristicTMP->notify();
  //PULSE
  pCharacteristicPULSE->setValue("pulse");
  pCharacteristicPULSE->notify();
  //HUM
  pCharacteristicHUM->setValue("hum");
  pCharacteristicHUM->notify();
  Serial.println("notified");
  
    
  delay(2000);
}
