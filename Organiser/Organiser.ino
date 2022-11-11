// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX, to use less code and ram
//#include <GFX.h>

#include "secrets.h"

#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram

#if defined (ESP8266)
#include <ESP8266WiFi.h>
#define USE_BearSSL true
#endif

#include <WiFiClient.h>
//#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include "SymbolMono18pt7b.h"

const int httpPort  = 80;
const int httpsPort = 443;
const String serverPath = "http://192.168.1.204:7887/api/";

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

#define BACKGROUND_COLOUR GxEPD_WHITE
#define FOREGROUND_COLOUR GxEPD_BLACK

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("GxEPD2_WiFi_Example");

  display.init(115200);

  #ifdef RE_INIT_NEEDED
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA); // switch off AP
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.disconnect();
#endif

  if (!WiFi.getAutoConnect() || ( WiFi.getMode() != WIFI_STA) || ((WiFi.SSID() != WIFI_SSID) && String(WIFI_SSID) != "........"))
  {
    Serial.println();
    Serial.print("WiFi.getAutoConnect()=");
    Serial.println(WiFi.getAutoConnect());
    Serial.print("WiFi.SSID()=");
    Serial.println(WiFi.SSID());
    WiFi.mode(WIFI_STA); // switch off AP
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }
  int ConnectTimeout = 30; // 15 seconds
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    if (--ConnectTimeout <= 0)
    {
      Serial.println();
      Serial.println("WiFi connect timeout");
      return;
    }
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());

  display.fillScreen(BACKGROUND_COLOUR);

  drawScreen();
  ESP.deepSleep(3600e6); // 1 Hour
  //ESP.deepSleep(86400e6); // 1 Day
}

void loop() {};

void drawScreen() 
{
  Serial.println("Running Draw Screen");
  display.setRotation(1);
  display.setTextColor(FOREGROUND_COLOUR);
  display.setFullWindow();
  display.fillScreen(BACKGROUND_COLOUR);

  //Get Data
  DynamicJsonDocument doc(8192);
  callApi("Organiser/displayInfo", &doc);

  // Heading
  String date = doc["headerDate"];
  Serial.println(date);
  display.setFont(&FreeMonoBold24pt7b);

  display.fillRect(0,0,480,40, FOREGROUND_COLOUR);

  display.setTextColor(BACKGROUND_COLOUR);
  writeCenteredText(0, 480, 35, date);
  if (date.startsWith("7 August")) {
    drawSymbol(0,   38, EMOJI_METAL, 0, 1, BACKGROUND_COLOUR);
    drawSymbol(440, 38, EMOJI_METAL, 0, 1, BACKGROUND_COLOUR);
  }
  display.setTextColor(FOREGROUND_COLOUR);

  // Weather
  // 3 Cols: 5, 160, 320, 475; 
  display.setFont(&FreeMonoBold9pt7b);

  writeHeading(55, "Weather");
  
  JsonArray forecast = doc["weatherReports"].as<JsonArray>();
  
  JsonObject day = forecast[0].as<JsonObject>();
  
  writeCenteredText(5, 160, 70, "Today");
  drawSymbol(70, 105, selectWeatherSymbol(day["condition"]), 0, 1.5);
  writeCenteredText(5, 160, 120, day["condition"]);
  writeCenteredText(5, 160, 135, day["temperature"] + "c");
  
  String calToday = day["datetime"];
  String calTodayStart = calToday.substring(0,10) + "T00:00:00.000Z";
  String calTodayEnd = calToday.substring(0,10) + "T23:59:59.000Z";

  day = forecast[1].as<JsonObject>();
  
  writeCenteredText(160, 320, 70, "Tomorrow");
  drawSymbol(225, 105, selectWeatherSymbol(day["condition"]), 0, 1);
  writeCenteredText(160, 320, 120, day["condition"]);
  writeCenteredText(160, 320, 135, day["temperature"] + "c");
    
  String calTom = day["datetime"];
  String calTomStart = calTom.substring(0,10) + "T00:00:00.000Z";
  String calTomEnd = calTom.substring(0,10) + "T23:59:59.000Z";

  day = forecast[2].as<JsonObject>();
  
  writeCenteredText(320, 475, 70, "Overmorrow");
  drawSymbol(385, 105, selectWeatherSymbol(day["condition"]), 0, 1);
  writeCenteredText(320, 475, 120, day["condition"]);
  writeCenteredText(320, 475, 135, day["temperature"] + "c");

  // Calendar
  writeHeading(155, "Today's Agenda");
  int16_t starty = 175;
  printCalendar(doc["calendarEntriesToday"].as<JsonArray>(), &starty);

  writeHeading(starty + 10, "Tomorrow's Agenda");
  starty = starty + 30;
  printCalendar(doc["calendarEntriesTomorrow"].as<JsonArray>(), &starty);

  display.setCursor(5, 645);
  String accessed = doc["accessedTime"];
  display.print("Last Updated: " + accessed);
  
  // Write
  display.display(false);
  Serial.println("Done");
}

void writeCenteredText(int16_t startx, int16_t endx, int16_t y, String text) 
{
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center bounding box by transposition of origin:
    uint16_t x = startx + (((endx - startx) - tbw) / 2);
    display.setCursor(x, y);
    display.print(text);
}

void writeHeading(int16_t y, String text)
{
  display.setCursor(5, y);
  display.print(text);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);

  display.drawFastHLine((10+tbw), y, display.width() - tbw - 15, FOREGROUND_COLOUR);
}

void printCalendar(JsonArray items, int16_t *y)
{
  if (items.size() == 0) {
      drawSymbol(5, *y + 17, EMOJI_HAPPY, 0, 1);
      display.setCursor(40, *y + 8);
      display.print("Nothing Planned!");
      *y = *y + 42;
  }
  
  for (JsonObject item : items) {
    Serial.println(((String)item["summary"]));

    //Serial.println("- Display");
    display.setCursor(5, *y);
    display.print(((String)item["display"]));
       
    *y = *y + 15;
  }
  Serial.println("Done");
}

void callApi(String endpoint, DynamicJsonDocument *doc) 
{
  callRestApi(serverPath, endpoint, doc);
}

void callRestApi(String apiUri, String endpoint, DynamicJsonDocument *doc)
{
  WiFiClient client;
  HTTPClient http;

  String path = apiUri + endpoint; 
  Serial.println(path);
  
  // Your Domain name with URL path or IP address with path
  http.begin(client, path.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(http.getSize());

    // The API is sending some wierd characters to start... Skip those!
    for (int i = 0; i <= 3; i++) {
      Serial.print((char)client.read());
    }

    DeserializationError error = deserializeJson(*doc, *http.getStreamPtr());

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

void drawSymbol(int16_t x, int16_t y, uint8_t c, uint16_t bg, uint8_t Size)
{
  drawSymbol(x, y, c, bg, Size, FOREGROUND_COLOUR);
}

void drawSymbol(int16_t x, int16_t y, uint8_t c, uint16_t bg, uint8_t Size, uint16_t Colour)
{
  display.setFont(&SymbolMono18pt7b);
  display.drawChar(x,y,c,Colour,bg,Size);
  display.setFont(&FreeMonoBold9pt7b);
}

uint8_t selectWeatherSymbol(String weather) 
{
  if (weather == "sunny")             {return WEATHER_SUNNY;}
  else if (weather == "cloudy")       {return WEATHER_CLOUDY;}
  else if (weather == "partlycloudy") {return WEATHER_PARTLY_CLOUDY;}
  else if (weather == "rainy")        {return WEATHER_RAINY;}
  else if (weather == "fog")          {return WEATHER_FOG;}
  else if (weather == "snowy")        {return WEATHER_SNOWY;}
  else if (weather == "lightning")    {return WEATHER_THUNDER;}
  else if (weather == "windy")        {return WEATHER_WINDY;}
  else if (weather == "clear-night")  {return WEATHER_SUNNY;}
  else if (weather == "pouring")      {return WEATHER_RAINY;}
  else                                {return EMOJI_UNHAPPY;}
}
