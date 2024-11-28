#include <Wire.h>
#include <Adafruit_INA219.h>
#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "TU_SSID";
const char* password = "TU_CONTRASEÑA_WIFI";
const unsigned long channelID = TU_CANAL_ID;
const char* writeAPIKey = "TU_API_KEY";

// Definir pines para el sensor SCT013 y sensor de vibraciones
const int sct013Pin = A0;  // SCT013 a pin analógico
const int vibrationPin = 32; // Sensor de vibraciones a pin digital

WiFiClient client;
Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  ThingSpeak.begin(client);
  
  if (!ina219.begin()) {
    Serial.println("Error al encontrar INA219");
    while (1);
  }
}

void loop() {
  // Leer el sensor SCT013 (corriente)
  int sct013Value = analogRead(sct013Pin);

  // Leer el sensor de vibraciones
  int vibrationValue = digitalRead(vibrationPin);

  // Leer los valores de INA219 (voltaje y corriente)
  float shuntVoltage = ina219.getShuntVoltage_mV();
  float busVoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  // Enviar los datos a ThingSpeak
  ThingSpeak.setField(1, sct013Value);
  ThingSpeak.setField(2, vibrationValue);
  ThingSpeak.setField(3, busVoltage);
  ThingSpeak.setField(4, shuntVoltage);
  ThingSpeak.setField(5, current_mA);

  int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (httpCode == 200) {
    Serial.println("Datos enviados con éxito");
  } else {
    Serial.println("Error al enviar datos");
  }

  delay(20000); // Enviar cada 20 segundos
}
}
