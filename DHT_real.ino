// %c++
#include <DHT.h>
#include <DHT_U.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>


#define DHTPIN D4
#define ICQMCU A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h; // Humidity
float t; // Temperature
int humidity; // Humidity of the soil
float ht; // Humidity of the soil
const char* ssid = "youpilab_fibre"; // YoupiLab WiFi SSID
const char* password = "i_l@v3_yl2021Fibre"; // YoupiLab password



AsyncWebServer server(80); // Create a web server on port 80
unsigned long previousMillis = 0; // Store the last time the data was sent
void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}



void initDHT() {
  dht.begin();
  Serial.println("DHT11 initialized");
}
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows
void initLCD() {
    Wire.begin(SDA, SCL);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Sensor");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
}
void displayData(float t, float h, float ht) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print("dgC");
    lcd.setCursor(15, 0);
    lcd.print("H");
    lcd.setCursor(0, 1);
    lcd.print("um: ");
    lcd.print(h);
    lcd.print(" ");
    lcd.print(ht);
    lcd.print(" %");
}
void setup() {
    Serial.begin(115200);
    initLCD();
    initDHT();
    initWiFi();
    
    if (!LittleFS.begin()) {
        Serial.println("Erreur de montage du système de fichiers LittleFS");
        return;
    }

    server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request){
        //Serial.println("Page d'accueil demandée");
        request->send(LittleFS, "/index.html", "text/html"); // Charger la page HTML
    });

    server.serveStatic("/", LittleFS, "/"); // Charger le CSS, JS, and image files
    server.serveStatic("/favicon.ico", LittleFS, "/img/iconazertyuiop.png"); // Charger l'icône de la page

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        if (isnan(h) || isnan(t)) {
            //Serial.println("Capteur DHT11 non fonctionnel");
            request->send(500, "text/plain", "Erreur de lecture du capteur DHT11");
            return;
        }
        //Serial.println("Lecture du capteur DHT11 réussie");
        String json = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + ", \"sol_humidity\": " + String(ht) + "}";
        request->send(200, "application/json", json);  
    });
    server.begin();
}


void loop() {
    if(millis() - previousMillis >= 2000) { // Send data every 2 seconds
        previousMillis = millis();
        h = dht.readHumidity();
        t = dht.readTemperature();
        humidity =  analogRead(ICQMCU);
        ht = map(humidity, 0, 1023, 0, 100); // Convert analog reading to percentage
        displayData(t, h, ht);
    }
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
    }
}
