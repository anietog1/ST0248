#include <DHT.h>

#define DHT_PIN 0
#define FAN_PIN 0
/*             ^ Fix it with and delete this comment. */
#define DHT_TYPE DHT11

unsigned const WAIT_DECIDE = 60 * 1000 * 60;//60 minutes
unsigned const WAIT_SEND = 30 * 1000 * 60;//30 minutes
unsigned const WAIT_MEASURE = 15 * 1000; //15 seconds
unsigned const WAIT_FAN = 60 * 1000; //1 minute
unsigned const WAIT_USER = 5 * 1000; //5 seconds

char const SSID[] = "";
char const PASS[] = "";
/*                  ^ Fix it and delete this comment. */

DHT dht(DHT_PIN, DHT_TYPE);

unsigned last_send = 0;
unsigned last_user = 0;
unsigned last_measure = 0;
unsigned last_fan = 0;

bool fan_status = LOW;
bool can_decide = false;

float temperature = 0.0f;
float humidity = 0.0f;

void setup() {
  Serial.begin(9600);

  dht.begin();

  pinMode(FANPIN, OUT);
  digitalWrite(FANPIN, LOW);

  /* Set connection with database */
}

float read_temperature() {
  return dht.readTemperature();
}

float read_humidity() {
  return dht.readHumidity();
}

bool request_fan_status() {
  /* Implement this function.
   * Return by default the current status just in case the connection fails.
   * This in order to avoid changes in the fan status.
   */
  return fan_status;
}

bool send_data(float temperature, float humidity) {
  /* Send data, return true if successfull
   * ... and delete this comment.
   */
  return false;
}

bool send_fan_status(bool fan_status) {
  /*
   * Update fan status on server and return true if successfull.
   * ... and delete this comment.
   */
  return false;
}

void loop() {
  unsigned now = millis();

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

  if(WAIT_USER < now - last_user) {
    bool server_fan_status = request_fan_status();

    if(server_fan_status != fan_status) {
      fan_status = server_fan_status;
      digitalWrite(FAN_PIN, fan_status);
      last_user = now;
    }
  }

  if(WAIT_DECIDE < now - last_user && WAIT_FAN < now - last_fan) {
    bool last_fan_status = fan_status;
    bool server_fan_status = request_fan_status();

    fan_status = temperature > 30.0f ? HIGH : LOW;

    digitalWrite(FAN_PIN, fan_status);

    if(fan_status != server_fan_status) {
      send_fan_status();
    }

    if(fan_status != last_fan_status) {
      last_fan = now;
    }
  }
}
