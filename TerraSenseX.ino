#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <Adafruit_BMP085.h>      // Libreria per sensore BMP085 (pressione atmosferica)
#include <Adafruit_MQ135.h>       // Libreria per sensore MQ135 (qualità dell'aria)
#include <Adafruit_SI1145.h>      // Libreria per sensore UV
// Includi altre librerie specifiche per i tuoi sensori

// Definizione sensori
DHT dht(2, DHT22);                 // DHT22 su pin 2 (umidità e temperatura)
Adafruit_BMP085 bmp;               // BMP085 (pressione atmosferica)
Adafruit_MQ135 mq135 = Adafruit_MQ135(A0); // MQ135 su A0 (qualità dell'aria)
Adafruit_SI1145 uv = Adafruit_SI1145();    // Sensore UV

// Parametri WiFi
const char* ssid = "NOME_RETE";
const char* password = "PASSWORD_RETE";
const char* server = "http://your-server.com/data";

// Variabili per dati raccolti
float temperature, humidity, pressure, airQuality, uvIndex;

// Funzione per configurazione WiFi
void setupWiFi() {
    Serial.begin(115200);
    delay(10);
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

// Setup iniziale
void setup() {
    Serial.begin(9600);
    
    // Inizializza i sensori
    dht.begin();
    if (!bmp.begin()) {
        Serial.println("BMP085 initialization failed!");
    }
    if (!uv.begin()) {
        Serial.println("UV sensor initialization failed!");
    }
    
    setupWiFi(); // Connessione WiFi
}

// Funzione per lettura sensori ambientali
void readEnvironmentalSensors() {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    pressure = bmp.readPressure();
    airQuality = mq135.getPPM();
    uvIndex = uv.readUV();
    
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
    } else {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" *C, Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    }
    
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" Pa");
    
    Serial.print("Air Quality (CO2 PPM): ");
    Serial.print(airQuality);
    Serial.println(" PPM");
    
    Serial.print("UV Index: ");
    Serial.print(uvIndex);
    Serial.println();
}

// Funzione per invio dati al server
void sendDataToServer() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        if (client.connect(server, 80)) {
            client.print("GET /data?");
            client.print("temperature="); client.print(temperature);
            client.print("&humidity="); client.print(humidity);
            client.print("&pressure="); client.print(pressure);
            client.print("&airQuality="); client.print(airQuality);
            client.print("&uvIndex="); client.print(uvIndex);
            client.println(" HTTP/1.1");
            client.println("Host: your-server.com");
            client.println("Connection: close");
            client.println();
            client.stop();
        } else {
            Serial.println("Connection to server failed");
        }
    }
}

// Loop principale
void loop() {
    readEnvironmentalSensors();  // Leggi i dati dai sensori ambientali
    sendDataToServer();          // Invia i dati al server
    delay(60000);                // Attendi un minuto prima della prossima lettura
}
