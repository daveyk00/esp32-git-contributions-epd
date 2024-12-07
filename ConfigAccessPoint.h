
#include "ConfigHtml.h"

const char* configSSID = "Github Contributions";
const char* configPassword = "HightPants";
WiFiServer server(80);

// Start the access point and the server
void startConfigServer() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(configSSID, configPassword);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

// Handle client connections
void handleConfigClient() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        currentLine += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 2) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Send your "Hello World" HTML response
            client.println("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
            client.println("<body><h1>Hello World</h1></body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }


  // Serial.println("Checking for client...");
  // WiFiClient client = server.available();
  // if (client) {
  //   Serial.println("Client connected!");
  //   String request = client.readStringUntil('\r');
  //   Serial.println("Request received:");
  //   Serial.println(request);
  //   client.flush();

  //   if (request.indexOf("GET / ") >= 0) {
  //     Serial.println("Serving HTML content...");
  //     client.println("HTTP/1.1 200 OK");
  //     client.println("Content-Type: text/html");
  //     client.println("Connection: close");
  //     client.println();
  //     client.print(configHtml); // Serve the HTML content
  //   }

  //   client.stop();
  //   Serial.println("Client disconnected.");
  // }
}