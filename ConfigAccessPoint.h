
#include <DNSServer.h>
#include <WebServer.h>
#include <ConfigHtml.h>

const char* configSSID = "Contributions Screen";
const char* configPassword = "HighPants";
DNSServer dnsServer;
WebServer server(80);

void sendConfigHtml() {
  server.send(200, "text/html", configHtml);
}

// Start the access point and the server
void startConfigServer() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(configSSID, configPassword);

  // By default DNSServer is started serving any "*" domain name. It will reply
  // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
  if (dnsServer.start()) {
    Serial.println("Started DNS server in captive portal-mode");
  } else {
    Serial.println("Err: Can't start DNS server!");
  }

  server.on("/", sendConfigHtml);
  server.onNotFound(sendConfigHtml);
  server.begin();
}

// Handle client connections
void handleConfigClient() {
  server.handleClient();
  delay(5);
}