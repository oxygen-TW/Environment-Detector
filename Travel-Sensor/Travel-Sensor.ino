#include <SoftwareSerial.h>
#include <DHT.h>
#include <pms.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define SensorDelayTime 1000
PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial BT(10, 11); // RX, TX
void setup(void)
{
  Serial.begin(9600);
  BT.begin(38400);
  dht.begin();

  pmsSelfCheck();
  /*BT.println(pmsx::pmsxApiVersion);
    BT.println("Sensor is prepared!");
    BT.println();*/

}

void RunDHTSensor(bool IsFahrenheit) {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature(IsFahrenheit);

  if (isnan(h) || isnan(t)) {
    BT.println(F("Failed to read from DHT sensor!"));
  }
  else {
    BT.print("Temperature=");
    BT.print(t);
    BT.println("℃");
    BT.print("Humidity=");
    BT.print(h);
    BT.println("%RH");
    BT.print("HeatIndex=");
    BT.print(dht.computeHeatIndex(t, h, IsFahrenheit));
    BT.println("℃");
    BT.println();
  }
}

void RunPmsSensor() {
  pmsx::PmsData data;
  auto status = pms.read(data);

  switch (status) {
    case pmsx::PmsStatus::OK: {
        auto view = data.raw;
        for (pmsx::PmsData::pmsIdx_t i = 0; i < view.getSize(); ++i) {
          BT.print(view.getName(i));
          //BT.print(" [");
          //BT.print(view.getMetric(i));
          //BT.print("] ");
          //            BT.print(" Level: ");
          //            BT.print(view.getLevel(i));
          BT.print(" | Φ: ");
          BT.print(view.getDiameter(i));
          BT.print(" -> ");
          BT.println(view.getValue(i));
        }
        break;
      }
    case pmsx::PmsStatus::NO_DATA:
      break;
    default:
      Serial.print("!!! Pms error: ");
      Serial.println(status.getErrorMsg());
  }
}
void loop(void)
{
  static char BlueToothCMD = '0';
  static bool IsFahrenheit = false;

  if (BT.available()) {
    BlueToothCMD = BT.read();
  }

  switch (BlueToothCMD) {
    case '1':
      RunDHTSensor(IsFahrenheit);
      BT.println();
      break;

    case '2':
      RunPmsSensor();
      BT.println();
      break;

    case '3':
      RunDHTSensor(IsFahrenheit);
      BT.println(F("_________________"));
      RunPmsSensor();
      break;

    case '4':
      BT.println(analogRead(A0));
      break;

    case 'a':
      IsFahrenheit = !IsFahrenheit;
      break;
  }
}

