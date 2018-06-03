#include <DHT.h>

#define DHT_PIN 0
/*             ^ Fix it with and delete this comment. */
#define DHT_TYPE DHT11

#define FAN_PIN 0
/*             ^ Fix it with and delete this comment. */

unsigned long const WAIT_DECIDE  = 60UL * 1000 * 60;//60 minutes
unsigned long const WAIT_SEND    = 30UL * 1000 * 60;//30 minutes
unsigned long const WAIT_MEASURE = 15UL * 1000;     //15 seconds
unsigned long const WAIT_FAN     = 60UL * 1000 * 5; //5 minutes
unsigned long const WAIT_USER    = 5UL  * 1000;     //5 seconds

unsigned long last_send = 0UL;
unsigned long last_user = 0UL;
unsigned long last_measure = 0UL;
unsigned long last_fan = 0UL;

DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0.0f;
float humidity    = 0.0f;
bool fan_status   = LOW;
bool can_decide   = true;

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  
  /* Set the connection with the platform. */
}

float read_temperature() {
  return dht.readTemperature();
}

float read_humidity() {
  return dht.readHumidity();
}

bool request_fan_status() {
  /* Implement this function, it returns the value of the fan variable in
   * the server. It's made for allowing the user turn on/off the fan.
   *
   * Return by default the current status just in case the connection fails.
   * This in order to avoid changes in the fan status.
   */
  return fan_status;
}

bool send_data(float temperature, float humidity) {
  /* Send data to the server, return true if successfull. */
  return false;
}

bool send_fan_status(bool fan_status) {
  /* Update fan status on server and return true if successfull. 
   * This one is made for allowing the system to take decisions about the fan's status, while
   * noticing the user.
   */
  return false;
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
      send_fan_status(fan_status);
    }

    if(fan_status != last_fan_status) {
      last_fan = now;
    }
  }
}
