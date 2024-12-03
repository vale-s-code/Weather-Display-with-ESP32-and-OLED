/*
 * File:   Home_Challange_003_VS.ino
 * Author: Valentin S. 
 *
 * About: 
 * Before you execute the code and load it onto your device, fill in the placeholders with your data.
 * The API for open weather can be created via a cokeless account here: 
 *
 * Created on 02.12.2024, 18:35
 * 
 * Extra:
 * You can easily change the location of the displayed weather.
 * To do this, you only need to replace the default location berlin with a desired location. 
 * Before doing this, you can test whether the location even exists by making a test query with the following link:
 * 
 * https://api.openweathermap.org/data/2.5/weather?q=[default location]&APPID=[API - KEY]
 *
 * https://api.openweathermap.org/data/2.5/weather?q=berlin,de&APPID=swgb3590t6hergbu
 * https://api.openweathermap.org/data/2.5/weather?q=London,uk&APPID=swgb3590t6hergbu
 *
 */
 
#include <WiFi.h> // Für ESP32
#include <HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wifi credentials
const char* ssid = "[replace with your credentials]";  
const char* password = "[replace with your credentials]"; 

// OpenWeatherMap API-Data
const String apiKey = "[replace with your credentials]"; 
const String city = "Berlin,de"; 
const String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&APPID=" + apiKey + "&units=metric";

// OLED-Display- Settings
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Wifi 
  Serial.println("Mit WLAN verbinden...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // for debugging on serial monitor
  Serial.println("\nVerbunden mit WLAN");
  Serial.println("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println("Fehler beim Initialisieren des OLED-Displays");
    while (true); 
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // OpenWeatherMap API 
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString(); 
      float temperature = extractTemperature(response); 

      // Temperature --> OLED-Display 
      display.clearDisplay();
      display.setTextSize(1);

      
      display.setCursor(40, 20);
      display.print("Wetter in");

      display.setCursor(40, 30);
      display.println("Berlin:");

      display.setCursor(40, 40);
      if (temperature != -999) {
        display.print("Temp: ");

        display.setCursor(40, 50);
        display.print(temperature);
         
        display.print(" C");
      } else {
        display.println("Error parsing data");
      }
      display.display();

      // for debugging on serial monitor
      Serial.println("Temp:");

      Serial.println(temperature);
    } else {
      Serial.print("Fehler bei der HTTP-Anfrage, Code: ");
      Serial.println(httpResponseCode);
    }

    http.end(); 
  } else {
    Serial.println("Nicht mit WLAN verbunden.");
  }

  delay(600000); 
}

float extractTemperature(String response) {
  int tempIndex = response.indexOf("\"temp\":"); 
  if (tempIndex != -1) {
    int start = tempIndex + 7; 
    int end = response.indexOf(',', start); 
    if (end != -1) {
      String tempStr = response.substring(start, end);
      return tempStr.toFloat(); 
    }
  }
  return -999; 
}
