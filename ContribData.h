#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <StreamUtils.h>

struct WifiCredentials
{
    const char *ssid;
    const char *password;
};

// Store the last valid credentials in RTC memory
RTC_DATA_ATTR int lastConnectedIndex = -1;

// Try to connect to wifi using a list of credentials
bool TryConnectWifi(WifiCredentials credentials[], int numCredentials)
{
    const unsigned long timeout = 6000; // 6 seconds timeout
    for (int i = -1; i < numCredentials; i++)
    {
        if (i == lastConnectedIndex)
            continue;
        const int credentialIndex = i == -1 ? lastConnectedIndex : i;

        WiFi.begin(credentials[credentialIndex].ssid, credentials[credentialIndex].password);
        Serial.print("\nConnecting to Wifi: ");
        Serial.println(credentials[credentialIndex].ssid);
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout)
        {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("\nWiFi connected");
            lastConnectedIndex = credentialIndex;
            return true;
        }
    }
    Serial.println("\nAll WiFi connection attempts failed");
    return false;
}

JsonArray FetchContributionsData(JsonDocument &doc, String url)
{
    WiFiClientSecure client;
    HTTPClient http;
    client.setInsecure();

    http.begin(client, url);
    int httpCode = http.GET();
    Serial.printf("\nHTTP Response code: %d\n", httpCode);

    String payload = http.getString();
    Serial.println("Got response:");
    Serial.println(payload);

    // Parse response
    // 2 weeks = [1, 0, 3, 1, 4, 0, 0, 1, 0, 3, 1, 4, 0, -1, -1]
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        http.end();
        return JsonArray(); // Return an empty array if parsing fails
    }

    http.end();
    return doc.as<JsonArray>();
}