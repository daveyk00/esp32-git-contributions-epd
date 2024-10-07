#include <ArduinoJson.h>
#include <GxEPD2_4G_4G.h>
#include <GithubIcon.h>
#include <Fonts/FreeMonoBold12pt7b.h>

GxEPD2_4G_4G<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)
RTC_DATA_ATTR bool screenInitialized = false;

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

void InitContainer()
{
    Serial.println("RenderContainer");
    do
    {
        // Set background
        display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);

        // Draw Github icon
        display.drawBitmap(7, 8, GithubIcon, 16, 16, GxEPD_WHITE);

        // Draw username
        display.setCursor(29, 13);
        display.print("/HarryHighPants");

    } while (display.nextPage());
    screenInitialized = true;
}

void drawCommitGraph(JsonArray contributions)
{
    Serial.println("drawCommitGraph");
    display.init(115200);
    display.setRotation(1); // Landscape
    display.setTextColor(GxEPD_WHITE);
    display.setTextSize(1);

    delay(5000);

    if (!screenInitialized)
    {
        InitContainer();
        screenInitialized = true;
    }
    else
    {
        Serial.println("Skipping screen initialization.");
    }

    delay(5000);
    Serial.println("drawing squares");

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
    do
    {
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
    } while (display.nextPage());
    display.hibernate();
}