#include <FS.h>
#include <WiFi.h>
#include <WiFiManager.h> // Library for Wi-Fi provisioning
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#include <Preferences.h>  // Zum Speichern von Benutzereingaben

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiManager wifiManager;
Preferences prefs;  // Speicher für Benutzereingaben

// API URL und Zugangsdaten
String apiKey = "";
String city = ""; // Stadt bleibt leer, Benutzer gibt sie ein
String country = ""; // Land bleibt leer, Benutzer gibt es ein

const String baseApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=";

WebServer server(8080);  // Webserver auf Port 8080


void setup() {
  Serial.begin(115200);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Initialisierung der Verbindung
  if (!wifiManager.autoConnect("ESP32-Config", "password")) {
    Serial.println("Failed to connect, restarting...");
    delay(3000);
    ESP.restart();
  }

  // API-Key und Ort aus den gespeicherten Daten laden
  prefs.begin("weather-config", false);
  apiKey = prefs.getString("apiKey", "");
  city = prefs.getString("city", "");
  country = prefs.getString("country", "");
  
  // Wenn kein API-Key gesetzt ist, den Benutzer auffordern, diesen über die Web-App einzugeben.
  if (apiKey == "") {
    display.setCursor(40, 20);
    display.println("Enter API Key");
    display.display();
  }

  // Webserver einrichten
  setupWebServer();

  // Starte den Server
  server.begin();
  Serial.println("Server started");
}

void setupWebServer() {
  // Hauptseite
  server.on("/", HTTP_GET, []() {
    String html = "<html><body><h2>Weather Configuration</h2>";
    html += "<form method='POST' action='/saveConfig'>";
    html += "API Key: <input type='text' name='apiKey' value='" + apiKey + "'><br>";
    html += "City: <input type='text' name='city' value='" + city + "'><br>";
    html += "Country Code: <input type='text' name='country' value='" + country + "'><br>";
    html += "<input type='submit' value='Save Configuration'>";
    html += "</form>";

    // Fehleranzeige (optional)
    if (city == "" || country == "" || apiKey == "") {
      html += "<p style='color:red;'>Error: Missing or invalid configuration!</p>";
    }

    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Konfiguration speichern
  server.on("/saveConfig", HTTP_POST, []() {
    if (server.hasArg("apiKey") && server.hasArg("city") && server.hasArg("country")) {
      String newApiKey = server.arg("apiKey");
      String newCity = server.arg("city");
      String newCountry = server.arg("country");

      if (newApiKey.length() > 0 && newCity.length() > 0 && newCountry.length() == 2) {
        apiKey = newApiKey;
        city = newCity;
        country = newCountry;

        // Konfiguration speichern
        prefs.putString("apiKey", apiKey);
        prefs.putString("city", city);
        prefs.putString("country", country);

        String message = "<html><body><h2>Configuration Saved!</h2>";
        message += "<p>API Key, City, and Country have been saved.</p>";
        message += "</body></html>";
        server.send(200, "text/html", message);
      } else {
        server.send(400, "text/html", "Invalid data received. Please check your inputs.");
      }
    } else {
      server.send(400, "text/html", "Invalid data received.");
    }
  });
  } 

void loop() {
  // WebServer Anfragen verarbeiten
  server.handleClient();

  // Wetterdaten abrufen
  String weatherData = getWeatherData();  // Hier rufst du die Wetterdaten ab

  // Stadtnamen aus den gespeicherten Konfigurationsdaten holen
  String cityName = city;  // city wird durch die API übergeben und gespeichert

  // Display für die Wetterdaten
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Zeilenweise Ausgabe auf dem Display
  display.setCursor(40, 20);  // Erste Zeile: Stadtname
  display.println(cityName);  // Dynamischer Stadtnamen

  display.setCursor(40, 30); // Zweite Zeile: Temperatur
  display.println("Temp. (C):");

  display.setCursor(40, 40); // Dritte Zeile: Temperaturwert
  display.println(weatherData);  // Wetterdaten hier anzeigen (z.B. Temperatur)

  display.display();
  delay(10000); // Update every 10 seconds
}

String getWeatherData() {
  HTTPClient http;
  String weatherData = "Loading...";

  // Wenn city oder country leer ist, kann keine Anfrage gesendet werden
  if (city == "" || country == "" || apiKey == "") {
    return "Missing config!";
  }

  // Leerzeichen im Stadtnamen durch %20 ersetzen
  String encodedCity = city;
  encodedCity.replace(" ", "%20");

  // URL zusammensetzen
  String apiUrl = baseApiUrl + encodedCity + "," + country + "&appid=" + apiKey + "&units=metric";
  
  http.begin(apiUrl);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String response = http.getString(); 
    float temperature = extractTemperature(response);

    if (temperature != -999) {
      weatherData = String(temperature) + " C";
    } else {
      weatherData = "Error parsing data";
    }
  } else {
    weatherData = "Invalid location/API!";
    Serial.println("Error: HTTP response code " + String(httpResponseCode));
  }

  http.end();
  return weatherData;
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
