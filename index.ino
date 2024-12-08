#include <WiFi.h>
#include <ArduinoJson.h>
#include <Rendering.h>
#include <ContribData.h>
#include <ConfigAccessPoint.h>

#define BUTTON_PIN 39 // GPIO39 (IO39) is the pin connected to the button

WifiCredentials credentials[] = {
    {"Hindsite Guest", "password"},
    {"YourWifiSSID-2G", "password"},
    {"Pixel_3588", "password"},
};

const int weeks = 17;
String url = "https://contributions-api.harryab.com/harryhighpants?weeks=" + String(weeks);
RTC_DATA_ATTR int lastContributions[weeks * 7];
bool isConfigMode = false;

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0: // Wakeup caused by GPIO (external wakeup)
    Serial.println("Wakeup caused by external signal using GPIO (button press)");
    // Check for button hold
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      Serial.println("Continue holding to start config");
      delay(2000);
      if (digitalRead(BUTTON_PIN) == LOW)
      {
        Serial.println("Starting config server");
        isConfigMode = true;
        storeContributions(JsonArray());
        drawConfigModeScreen();
        startConfigServer();
        return;
      }
    }

    break;
  case ESP_SLEEP_WAKEUP_TIMER: // Wakeup caused by the timer
    Serial.println("Wakeup caused by timer (deep sleep duration complete)");
    break;
  default: // Other wakeup reasons
    Serial.printf("Wakeup not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }

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

void loop() {
  if (!isConfigMode) return;
  delay(100);
  handleConfigClient();
}

void sleep()
{
  // Set up GPIO39 as a wakeup source on LOW signal (when the button is pressed)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);

  Serial.println("Going to sleep for 1 hour");
  esp_deep_sleep(3600e6); // 1 hour
  // esp_deep_sleep(10e6); // 10 seconds
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
