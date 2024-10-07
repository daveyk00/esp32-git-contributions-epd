#include <WiFi.h>
#include <ArduinoJson.h>
#include <Rendering.h>
#include <ContribData.h>

WifiCredentials credentials[] = {
    {"YourWifiSSID-2G", "password"},
    {"Hindsite Guest", "password"},
    {"Pixel_3588", "password"},
};

const char *url = "https://contributions-api.harryab.com/harryhighpants?weeks=17";

void setup()
{
  Serial.begin(115200);
  bool connected = TryConnectWifi(credentials, sizeof(credentials) / sizeof(credentials[0]));
  if (!connected)
  {
    // TODO: show wifi error icon on screen
    Serial.println("Failed to connect to wifi");
    sleep();
  }

  JsonDocument doc;
  JsonArray contributions = FetchContributionsData(doc, url);
  if (contributions.size() == 0)
  {
    // TODO: show json error icon on screen
    Serial.println("Failed to fetch contributions data");
    sleep();
  }

  drawCommitGraph(contributions);
  sleep();
}

void sleep()
{
  Serial.println("Going to sleep for 1 hour");
  // esp_deep_sleep(3600e6); // 1 hour
  esp_deep_sleep(10e6);
}

void loop()
{
}
