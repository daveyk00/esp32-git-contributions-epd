const char configHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Configuration Page</title>
</head>
<body>
  <h1>Device Configuration</h1>
  <form action="/save" method="POST">
    <label for="ssid">WiFi SSID:</label><br>
    <input type="text" id="ssid" name="ssid"><br><br>
    <label for="password">WiFi Password:</label><br>
    <input type="password" id="password" name="password"><br><br>
    <button type="submit">Save</button>
  </form>
</body>
</html>
)rawliteral";