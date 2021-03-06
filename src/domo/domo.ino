#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
#include <WiFi101.h>
#include <UbidotsArduino.h>

int const DHT_PIN  = 5;
int const DHT_TYPE = DHT11;
int const FAN_PIN  = 4;

char ID_TEMP [] = "5b14aab6642ab6389cd81c04";
char ID_HUMI [] = "5b14ac57642ab63ac843b882";
char ID_FAN  [] = "5b0cc856642ab67ab668a90a";
char ID_AUTO [] = "5b16e732642ab60aa533763e";
char ID_LIMIT[] = "5b195b24642ab62cefb9ebf2";
char TOKEN   [] = "BBFF-J4H9tdvAFxGTcAdZkaZy7nEcfFznH9";

char const SSID[] = "agus";
char const PASS[] = "catalina";

unsigned long const WAIT_SEND    = 15UL * 1000 * 60; //15 minutes
unsigned long const WAIT_MEASURE = 15UL * 1000;      //15 seconds
unsigned long const WAIT_FAN     = 2UL  * 1000;      //2 seconds
unsigned long const WAIT_AUTO    = 2UL  * 1000;      //2 seconds
unsigned long const WAIT_USER    = 2UL  * 1000;      //2 seconds

int connection_status = WL_IDLE_STATUS;

unsigned long last_send    = 0UL;
unsigned long last_measure = 0UL;
unsigned long last_fan     = 0UL;
unsigned long last_auto    = 0UL;
unsigned long last_user    = 0UL;

Ubidots client(TOKEN);

DHT dht(DHT_PIN, DHT_TYPE);

float temperature = 0.0f;
float humidity    = 0.0f;

bool  fan_status        = LOW;
bool  auto_mode         = true;
float temperature_limit = 30.0f;

void setup() {
  Serial.begin(9600);

#ifdef DEBUG
  while(!Serial);
#endif

  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  if(WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG
    Serial.println("WiFi shield not present.");
#endif

    while (true);
  }

#ifdef DEBUG
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware.");
  }
#endif

  while(connection_status != WL_CONNECTED) {
#ifdef DEBUG
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SSID);
#endif

    connection_status = WiFi.begin(SSID, PASS);
    delay(1000);
  }
}

float read_temperature() {
  return dht.readTemperature();
}

float read_humidity() {
  return dht.readHumidity();
}

bool send_data(float temperature, float humidity) {
  client.add(ID_TEMP, temperature);
  client.add(ID_HUMI, humidity);
  return client.sendAll();
}

bool send_fan_status(bool fan_status) {
  client.add(ID_FAN, fan_status);
  return client.sendAll();
}

bool request_fan_status() {
  float* response_array = client.getValue(ID_FAN);

  if(response_array[0]) {
    return response_array[1];
  } else {
    return fan_status;
  }
}

bool request_auto_mode() {
  float* response_array = client.getValue(ID_AUTO);

  if(response_array[0]) {
    return response_array[1];
  } else {
    return auto_mode;
  }
}

float request_temperature_limit() {
  float* response_array = client.getValue(ID_LIMIT);

  if(response_array[0]) {
    return response_array[1];
  } else {
    return temperature_limit;
  }
}


void loop() {
  unsigned long now = millis();

  if(WAIT_MEASURE < now - last_measure) {
    temperature = read_temperature();
    humidity = read_humidity();
    last_measure = now;
  }

  if(WAIT_SEND < now - last_send) {
    if(send_data(temperature, humidity)) {
      last_send = now;
    }
  }

  if(WAIT_AUTO < now - last_auto) {
    auto_mode = request_auto_mode();
    last_auto = now;
  }

  if(auto_mode) {
    if(WAIT_FAN < now - last_fan) {
      temperature_limit = request_temperature_limit();
      fan_status = temperature >= temperature_limit ? HIGH : LOW;
      digitalWrite(FAN_PIN, fan_status);
      send_fan_status(fan_status);
      last_fan = now;
    }
  } else {
    if(WAIT_USER < now - last_user) {
      fan_status = request_fan_status();
      digitalWrite(FAN_PIN, fan_status);
      last_user = now;
    }
  }
}
