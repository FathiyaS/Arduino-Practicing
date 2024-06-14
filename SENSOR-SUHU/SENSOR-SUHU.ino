#include <DHT.h>
#include <DHT_U.h>

#include "DHT.h"
DHT dht(2, DHT11);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000);
  float kelembaban = dht.readHumidity();
  float suhu = dht.readTemperature();

  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println(F("Gagal Baca Data DHT sensor!"));
    return;
  }

  Serial.print("Kelembaban: ");
  Serial.println(kelembaban);
  Serial.print("Suhu: ");
  Serial.println(suhu);
}