#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <StreamUtils.h>

extern char wifiSSID[50];
extern char wifiPassword[50];

// Try to connect to wifi using a list of credentials
bool TryConnectWifi()
{
    const unsigned long timeout = 6000; // 6 seconds timeout
    WiFi.begin(wifiSSID, wifiPassword);
    Serial.print("\nConnecting to Wifi: ");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        return true;
    }
    Serial.println("\nWiFi connection failed");
    return false;
}

JsonArray FetchContributionsData(JsonDocument &doc, String url)
{
    WiFiClientSecure client;
    HTTPClient http;
    client.setInsecure();

    Serial.println("Fetching contributions data from: " + url);
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