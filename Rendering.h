#include <ArduinoJson.h>
#include <GxEPD2_4G_4G.h>
#include <GithubIcon.h>
#include <WifiIcon.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define BAT_TEST_PIN 35
GxEPD2_4G_4G<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)

uint16_t GetColor(int level)
{
  switch (level)
  {
  case 0:
    return GxEPD_BLACK;
  case 1:
    return GxEPD_DARKGREY;
  case 2:
    return GxEPD_LIGHTGREY;
  case 3:
    return GxEPD_WHITE;
  case 4:
    return GxEPD_WHITE;
  default:
    return GxEPD_BLACK;
  }
}

void drawCenterString(const String &buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);
    display.print(buf);
}

void initScreen() {
  display.init(115200);
  display.setRotation(1); // Landscape
  display.setTextColor(GxEPD_WHITE);
  display.setTextSize(1);

  // Set black background
  display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
}

void DrawHeader()
{
  // Draw Github icon
  display.drawBitmap(7, 8, GithubIcon, 16, 16, GxEPD_WHITE);

  // Draw username
  display.setCursor(29, 13);
  display.print("/HarryHighPants");

  // Draw battery percentage
  display.setCursor(225, 13);
  int adcValue = analogRead(BAT_TEST_PIN);
  float voltage = (adcValue / 4095.0) * 3.3 * 2;
  long percentage = map(voltage * 1000, 3200, 3900, 0, 100); // Use millivolts for mapping
  display.printf("%ld%%", percentage);
}

void drawCommitGraph(JsonArray contributions)
{
  initScreen();

  // GxEPD_WHITE, GxEPD_LIGHTGREY, GxEPD_DARKGREY, GxEPD_BLACK
  int columns = 17;
  int rows = 7;
  int total = columns * rows;
  int squareMargin = 2;
  int gridSize = display.width() / columns;
  int squareSize = gridSize - squareMargin;
  int borderThickness = 3;
  int yOffset = 24;
  int xOffset = 5;

  DrawHeader();

  // Draw squares
  for (int column = 0; column < columns; column++)
  {
    for (int row = 0; row < rows; row++)
    {
      int squareX = column * gridSize + xOffset + squareMargin;
      int squareY = row * gridSize + yOffset + squareMargin;
      int index = rows * column + row;
      int level = contributions[index].as<int>();

      // Set a partial window for the specific square area
      // display.setPartialWindow(squareX, squareY, squareSize, squareSize);
      display.fillRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GetColor(level));
      display.drawRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GetColor(level + 1));

      int isToday = level != -1 && (index == total - 1 || contributions[index + 1].as<int>() == -1);
      if (isToday)
      {
        display.drawRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GxEPD_WHITE);
      }
    }
  }
  display.displayWindow(0, 0, display.width(), display.height());
  display.hibernate();
}

void drawConfigModeScreen(){
  Serial.println("drawConfigModeScreen");
  initScreen();

  display.init(115200);
  display.setRotation(1); // Landscape
  display.setTextColor(GxEPD_WHITE);
  display.setTextSize(1);

  // Set black background
  display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);

  int centerX = display.width()/2;
  int centerY = display.height()/2;

  display.drawBitmap(centerX, centerX - 32, WifiIcon, 64,64, GxEPD_WHITE);

  display.setTextSize(2);
  drawCenterString("Config mode", centerX, centerY);

  display.setTextSize(1);
  drawCenterString("Connect to my wifi 'Contributions Screen'", centerX, centerY + 20);
  
  display.setTextSize(1);
  drawCenterString("Then navigate to 192.168.4.1 to configure me", centerX, centerY + 30);

  // Draw the updates
  display.displayWindow(0, 0, display.width(), display.height());
  display.hibernate();

}