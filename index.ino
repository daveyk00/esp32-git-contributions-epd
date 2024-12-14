#include <WiFi.h>
#include <ArduinoJson.h>
#include <Rendering.h>
#include <ContribData.h>
#include <ConfigAccessPoint.h>

#define BUTTON_PIN 39 // GPIO39 (IO39) is the pin connected to the button
const int weeks = 17;
const int totalDays = weeks*7;

// RTC memory
RTC_DATA_ATTR bool hasInitialised = false;
RTC_DATA_ATTR char username[50] = "HarryHighPants";
RTC_DATA_ATTR char wifiSSID[50] = "YourWifiSSID-2G";
RTC_DATA_ATTR char wifiPassword[50] = "password";
RTC_DATA_ATTR int syncInterval = 4; // Hours
RTC_DATA_ATTR char apiUrl[100] = "https://contributions-api.harryab.com/";
RTC_DATA_ATTR int lastContributions[totalDays] = {};
RTC_DATA_ATTR bool showingWifiError = false;
RTC_DATA_ATTR bool showingFetchError = false;

String url = String(apiUrl) + String(username) + "?weeks=" + String(weeks);
const int configModeTimeout = 20 * 60 * 1000; // 20 minutes
unsigned long configStartMillis = 0;
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
    if (isHoldingButton())
    {
      enterConfigMode();
      return;
    }
    break;
  case ESP_SLEEP_WAKEUP_TIMER: // Wakeup caused by the timer
    Serial.println("Wakeup caused by timer (deep sleep duration complete)");
    break;
  default: // Other wakeup reasons
    Serial.printf("Wakeup not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }

  bool connected = TryConnectWifi();
  if (!connected)
  {
    if(!showingWifiError){
      Serial.println("Failed to connect to wifi");
      showingWifiError = true;
      drawCommitGraph(lastContributions, showingWifiError, showingFetchError);
    }
    sleep();
  }
  showingWifiError = false;

  JsonDocument doc;
  JsonArray contributions = FetchContributionsData(doc, url);
  if (contributions.size() == 0)
  {
    if(!showingFetchError){
      Serial.println("Failed to fetch contributions data");
      showingFetchError = true;
      drawCommitGraph(lastContributions, showingWifiError, showingFetchError);
    }
    sleep();
  }
  showingFetchError = false;

  if (!haveContributionsChanged(contributions))
  {
    Serial.println("No changes in contributions");
    sleep();
  }

  storeContributions(contributions);
  drawCommitGraph(lastContributions, showingWifiError, showingFetchError);
  sleep();
}

void loop() {
  delay(100);
  if (!isConfigMode) return;
  if (isHoldingButton())
  {
    exitConfigMode();
    return;
  }
  // Check if 20 minutes have passed
  if (millis() - configStartMillis > configModeTimeout) {
    Serial.println("Config mode timeout");
    exitConfigMode();
    return;
  }
  handleConfigClient();
}

void sleep()
{
  // Set up GPIO39 as a wakeup source on LOW signal (when the button is pressed)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);

  Serial.println("Going to sleep for " + String(syncInterval) + " hours");
  esp_deep_sleep(syncInterval * 3600e6); // 1 hour
  // esp_deep_sleep(10e6); // 10 seconds
}

bool isHoldingButton()
{
  if (digitalRead(BUTTON_PIN) != LOW) return false;
  delay(2000);
  return digitalRead(BUTTON_PIN) == LOW;
}

void exitConfigMode()
{
  Serial.println("Exiting config mode");
  isConfigMode = false;
  drawCommitGraph(lastContributions, showingWifiError, showingFetchError);
  sleep();
}

void enterConfigMode()
{
  Serial.println("Starting config server");
  isConfigMode = true;
  showingWifiError = false;
  showingFetchError = false;
  drawConfigModeScreen();
  startConfigServer();
  configStartMillis = millis();
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
