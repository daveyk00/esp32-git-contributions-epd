
#include <DNSServer.h>
#include <WebServer.h>
#include <ConfigHtml.h>
#include <ConfigSavedHtml.h>

const char* configSSID = "Contributions Screen";
const char* configPassword = "HighPants";
DNSServer dnsServer;
WebServer server(80);
extern const int totalDays;

extern char username[50];
extern char wifiSSID[50];
extern char wifiPassword[50];
extern int syncInterval; // Hours
extern char apiUrl[100];
extern int lastContributions[];

void sendConfigHtml() {
  // Inject the stored values into the HTML
  String loadedConfigHtml = String(configHtml);
  loadedConfigHtml.replace("{{username}}", username);
  loadedConfigHtml.replace("{{wifi-ssid}}", wifiSSID);
  loadedConfigHtml.replace("{{wifi-password}}", wifiPassword);
  loadedConfigHtml.replace("{{interval}}", String(syncInterval));
  loadedConfigHtml.replace("{{url}}", apiUrl);
  server.send(200, "text/html", loadedConfigHtml);
}

void startConfigServer() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(configSSID, configPassword);

  // By default DNSServer is started serving any "*" domain name. It will reply
  // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
  dnsServer.start();

  server.on("/", sendConfigHtml);
  server.on("/submit", []() {
    // Read the form values from the param and store them in RTC memory
    String updatedUsername = server.arg("username");
    updatedUsername.trim(); 
    if (strcmp(username, updatedUsername.c_str()) != 0) {
      strcpy(username, updatedUsername.c_str());
      memset(lastContributions, 0, totalDays * 4); // 4 bytes per int
    }
    strcpy(wifiSSID, server.arg("wifi-ssid").c_str());
    strcpy(wifiPassword, server.arg("wifi-password").c_str());
    syncInterval = server.arg("interval").toInt();
    strcpy(apiUrl, server.arg("url").c_str());
    
    Serial.println("Saved updated config");
    server.send(200, "text/html", configSavedHtml);
    delay(100); // Wait for the response to be sent

    // Stop the server and DNS
    server.stop();
    dnsServer.stop();
    Serial.println("restarting");
    esp_deep_sleep(1e6);
  });
  server.onNotFound(sendConfigHtml);

  server.begin();
}

void handleConfigClient() {
  server.handleClient();
}