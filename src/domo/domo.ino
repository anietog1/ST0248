#include <DHT.h>

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float get_temperature() {
  return dht.readTemperature();
}

float read_humidity() {
  return dht.readHumidity();
}

bool send_data(float temperature, float humidity) {
  /* Some code. */
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  //Set connection with database
}

void loop() {
  float temperature = get_temperature();
  float humidity = read_humidity();
  send_data(temperature, humidity);
}
