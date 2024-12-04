# Weather-Display-with-ESP32-and-OLED

This project demonstrates how to build a simple weather display using an ESP32 microcontroller and an OLED screen. It fetches real-time weather data from the OpenWeatherMap API and displays the current temperature for a specified location.

## Features  
- 🌐 **Real-Time Weather Display**  
  Fetches live weather data for a specified location using the OpenWeatherMap API.  

- ✨ **Customizable Location**  
  Easily change the displayed location by modifying the `city` variable in the code.  

- 📶 **WiFi Connectivity**  
  Connects to your WiFi network to retrieve real-time weather data.  

- 🖥️ **OLED Display Integration**  
  Displays weather information on an OLED screen using the Adafruit SSD1306 library.  

- 🚨 **Error Handling**  
  Handles errors gracefully, such as failed API requests or invalid JSON responses.

## Configuration  

1. 📥 **Clone or Download**  
   Clone or download this repository to your local machine.  

2. 🛠️ **Open the Code**  
   Open the `.ino` file in the Arduino IDE.  

3. 🔧 **Replace Placeholders**  
   Update the following variables in the code:  
   - **WiFi credentials:** Replace `ssid` and `password` with your WiFi network details.  
   - **API Key:** Replace `[API_KEY]` with your OpenWeatherMap API key. You can obtain it [here](https://home.openweathermap.org/users/sign_up).  

4. 🌍 **Optional: Change Location**  
   Modify the `city` & `country` variable to set a different location. The default is `Berlin`, `de`.  
   Test your location with this example:  
