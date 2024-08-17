#include <WiFiNINA.h>
#include "ThingSpeak.h"
#include "DHT.h"

// WiFi credentials
char ssid[] = "OnePlus";
char pass[] = "pt123456789";
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient client;

// ThingSpeak details
unsigned long myChannelNumber = 2628618; 
const char * myWriteAPIKey = "BZ71Y5JSP0IH2EY0";

// DHT sensor setup
#define DHTPIN 2
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  dht.begin();
  ThingSpeak.begin(client);

  // Connect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

void loop() {
  // Read humidity and temperature data from DHT22 sensor
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);
  float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
  float heatIndexF = dht.computeHeatIndex(tempF, humidity);

  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print the data to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(tempC);
  Serial.print("°C ");
  Serial.print(tempF);
  Serial.print("°F  Heat index: ");
  Serial.print(heatIndexC);
  Serial.print("°C ");
  Serial.print(heatIndexF);
  Serial.println("°F");

  // Write the data to ThingSpeak
  ThingSpeak.setField(1, tempC);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, heatIndexC);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  // it is used to check the http request 200 means it is successfull
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(60000); // Wait for 60 seconds to update again
}
