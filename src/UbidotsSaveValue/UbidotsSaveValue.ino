/**
 * Using these libraries:
 * Ubidots, Remember modify this library (adding WiFi101.h) for mkr1000:
 * https://github.com/ubidots/ubidots-arduino-wifi
 * WiFi101, to conecting with wifi net:
 * https://github.com/arduino-libraries/WiFi101
 */

#include <SPI.h>
#include <WiFi101.h>
#include <UbidotsArduino.h>
#include <DHT.h>


#define ID_TEMP  "5b14aab6642ab6389cd81c04"  //Temperature
#define ID_HUMI "5b14ac57642ab63ac843b882"    //Humidity
#define ID_FAN "5b0cc856642ab67ab668a90a"  //Fan
#define TOKEN  "BBFF-J4H9tdvAFxGTcAdZkaZy7nEcfFznH9"  // Ubidots TOKEN

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);



char ssid[] = "FLIA RESTREPO"; //  your network SSID (name)
char pass[] = "zafiro5821";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

Ubidots client(TOKEN);



float get_temperature() {
  return dht.readTemperature();
}

float read_humidity() {
  return dht.readHumidity();
}

bool send_data(float temperature, float humidity) {
   client.add(ID_TEMP, temperature);
  client.add(ID_HUMI, humidity);
  client.sendAll();
}



void setup() {
 Serial.begin(9600);
  dht.begin();
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 1 seconds for connection:
    delay(1000);
  }

}
void loop() {


  float temperature = get_temperature();
  float humidity = read_humidity();
  send_data(temperature, humidity);

   //Getting data
  float *responseArray;
  float value;
  responseArray = client.getValue(ID_FAN);
  // The library return a verify float in the first position, 0 when
  // it doesn't get the value and 1 when it get the value
  // in the second position is the value
  if (responseArray[0] == 1) {
    value = responseArray[1];
    if (value) {
       digitalWrite(LED_BUILTIN, HIGH);   
      Serial.println("Ventilador encendido");
    } else {
       digitalWrite(LED_BUILTIN, LOW) ;
      Serial.println("Ventilador apagado");

    }
  }



  delay(500);

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 1 seconds for connection:
    delay(1000);
  }
}





