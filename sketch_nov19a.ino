#include <dht11.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include <HTTPClient.h>

// DC-Motor Pinbelegung
#define MOTOR_A 18
#define MOTOR_B 19

// DHT11 Sensor-Pin
#define DHT11PIN 4

// ThingSpeak / TalkBack Konfiguration
#define CHANNEL_ID 2752270
#define CHANNEL_API_KEY "2SGXBEOTJANOVJJ5"
#define TALKBACK_ID 53588
#define TALKBACK_API_KEY "Q3PY4HC274XOUH7L"

// WiFi Konfiguration
#define WIFI_TIMEOUT_MS 20000
#define WIFI_NETWORK "FRITZ!Repeater 2400" 
#define WIFI_PASSWORD "29031969Ralfb"

WiFiClient client;
dht11 DHT11;

// Global persistent target temperature – Initialwert (z. B. 25°C)
float lastTargetTemperature = 25;

// Heizungszustand
bool isHeating = false;       // Aktuelle Heizungsanfrage: true = Heizung EIN (z. B. Ventil öffnen)
bool previousHeating = false; // Vorheriger Zustand

// Verbindet mit dem WLAN
void connectToWifi(){
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
    Serial.print(".");
    delay(100);
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println(" Failed");
  }
  else{
    Serial.print(" Connected, IP: ");
    Serial.println(WiFi.localIP());
  }
}

// Liest die Zieltemperatur über TalkBack ab und aktualisiert den globalen Wert,
// falls ein neuer, gültiger (ungleich 0) Wert geliefert wird.
float getTargetTemperatureFromTalkBack() {
  HTTPClient http;
  String url = "https://api.thingspeak.com/talkbacks/" + String(TALKBACK_ID) +
               "/commands/execute?api_key=" + String(TALKBACK_API_KEY);
  
  http.begin(url);
  int httpCode = http.GET(); // GET-Anfrage senden

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) { 
      String payload = http.getString(); // Antwort des Servers
      if (payload.length() > 0) {
        float newTarget = payload.toFloat();
        // Nur aktualisieren, wenn der neue Wert ungleich 0 ist
        if (newTarget != 0) {
          lastTargetTemperature = newTarget;
          Serial.print("New target temperature from TalkBack: ");
          Serial.println(lastTargetTemperature);
        }
      }
    } else {
      Serial.print("HTTP-Error Message: ");
      Serial.println(httpCode);
    }
  } else {
    Serial.print("Error while connecting: ");
    Serial.println(http.errorToString(httpCode));
  } 
  http.end(); 
  return lastTargetTemperature;
}

// Steuert den DC-Motor:
// Je nach Heizungszustand (Temperatur zu niedrig / zu hoch) wird der Motor für einen
// kurzen Impuls in die entsprechende Richtung aktiviert.
void toggleHeatingMotor(){
  if(isHeating != previousHeating){
    if(isHeating){
      Serial.println("Temperatur zu niedrig -> Heizung EIN (Ventil öffnen)");
      // Motor in Richtung "Öffnen" drehen:
      digitalWrite(MOTOR_A, LOW);
      digitalWrite(MOTOR_B, HIGH);
      delay(15000); // 5 Sekunden drehen
      digitalWrite(MOTOR_A, LOW);
      digitalWrite(MOTOR_B, LOW);
    }
    else{
      Serial.println("Temperatur zu hoch -> Heizung AUS (Ventil schließen)");
      // Motor in Richtung "Schließen" drehen:
      digitalWrite(MOTOR_A, HIGH);
      digitalWrite(MOTOR_B, LOW);
      delay(15000); // 5 Sekunden drehen
      digitalWrite(MOTOR_A, LOW);
      digitalWrite(MOTOR_B, LOW);
    }
    previousHeating = isHeating; 
  }
}

void setup() {
  Serial.begin(9600);
  
  // Initialisiere die Motor-Pins als Ausgänge
  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);

  connectToWifi();
  
  // ThingSpeak initialisieren
  ThingSpeak.begin(client);
  Serial.println("ThingSpeak initialized successfully.");
}

void loop() {
  Serial.println();
  
  // Hole die Zieltemperatur über TalkBack – der Rückgabewert ist persistent,
  // d. h. es wird der zuletzt empfangene (nicht 0) Wert verwendet.
  float targetTemperature = getTargetTemperatureFromTalkBack(); 
  Serial.print("Target Temperature: ");
  Serial.println(targetTemperature);
  
  // Sensor auslesen
  int chk = DHT11.read(DHT11PIN);
  float temperature = DHT11.temperature;
  float humidity = DHT11.humidity;

  // Sensorwerte ausgeben
  Serial.print("Humidity (%): ");
  Serial.println(humidity, 2);
  Serial.print("Temperature (C): ");
  Serial.println(temperature, 2);

  // Sende Daten an ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, targetTemperature);
  ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);

  // Bestimme den Heizungszustand:
  // Bei zu niedriger Temperatur schalten wir die Heizung ein (Ventil öffnen),
  // bei zu hoher Temperatur schalten wir sie aus (Ventil schließen).
  if(temperature < targetTemperature){
    isHeating = true;
  }
  else{
    isHeating = false;
  }

  // Falls sich der Zustand ändert, den Motor in die entsprechende Richtung
  // für die angegebene Zeit aktivieren.
  toggleHeatingMotor();

  // Warte 20 Sekunden bis zur nächsten Messung
  delay(120000);
}
