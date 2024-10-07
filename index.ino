#include <WiFi.h>
#include <ArduinoJson.h>
#include <Rendering.h>
#include <ContribData.h>

WifiCredentials credentials[] = {
    {"YourWifiSSID-2G", "password"},
    {"Hindsite Guest", "password"},
    {"Pixel_3588", "password"},
};

const int weeks = 17;
String url = "https://contributions-api.harryab.com/harryhighpants?weeks=" + String(weeks);
RTC_DATA_ATTR int lastContributions[weeks * 7];

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

  if (!haveContributionsChanged(contributions))
  {
    Serial.println("No changes in contributions");
    sleep();
  }

  storeContributions(contributions);
  drawCommitGraph(contributions);
  sleep();
}

void sleep()
{
  Serial.println("Going to sleep for 1 hour");
  esp_deep_sleep(3600e6); // 1 hour
  // esp_deep_sleep(10e6); // 10 seconds
}

void loop()
{
}

// Store the contributions in RTC memory
void storeContributions(JsonArray contributions)
{
  for (size_t i = 0; i < contributions.size() && i < sizeof(lastContributions) / sizeof(lastContributions[0]); i++)
  {
    lastContributions[i] = contributions[i].as<int>();
  }
}

// Compare new contributions with the last stored contributions
bool haveContributionsChanged(JsonArray contributions)
{
  for (size_t i = 0; i < contributions.size() && i < sizeof(lastContributions) / sizeof(lastContributions[0]); i++)
  {
    if (contributions[i].as<int>() != lastContributions[i])
    {
      return true;
    }
  }
  return false;
}
