#include <GxEPD2_4G_4G.h>
#include <GithubIcon.h>
#include <Fonts/FreeMonoBold12pt7b.h>

GxEPD2_4G_4G<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)

void drawCommitGraph(JsonArray contributions)
{
    display.init(115200);
    display.setRotation(1); // Landscape
    display.setTextColor(GxEPD_WHITE);
    display.setTextSize(1);

    // GxEPD_WHITE, GxEPD_LIGHTGREY, GxEPD_DARKGREY, GxEPD_BLACK
    int columns = 17;
    int rows = 7;
    int total = columns * rows;
    int squareMargin = 2;
    int gridSize = display.width() / columns;
    int yOffset = 24;
    int xOffset = 5;
    do
    {
        // Set bg
        display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);

        // Github icon
        display.drawBitmap(7, 8, GithubIcon, 16, 16, GxEPD_WHITE);

        // Draw username
        display.setCursor(29, 13);
        display.print("/HarryHighPants");

        // Draw squares
        for (int column = 0; column < columns; column++)
        {
            for (int row = 0; row < rows; row++)
            {
                int squareX = column * gridSize + xOffset;
                int squareY = row * gridSize + yOffset;
                int index = rows * column + row;
                int level = contributions[index].as<int>();
                int isToday = level != -1 && (index == total - 1 || contributions[index + 1].as<int>() == -1);
                uint16_t color = GetColor(level);

                display.fillRoundRect(squareX + squareMargin, squareY + squareMargin, gridSize - squareMargin, gridSize - squareMargin, 3, color);
                display.drawRoundRect(squareX + squareMargin, squareY + squareMargin, gridSize - squareMargin, gridSize - squareMargin, 3, GetColor(level + 1));
                if (isToday)
                {
                    display.drawRoundRect(squareX + squareMargin, squareY + squareMargin, gridSize - squareMargin, gridSize - squareMargin, 3, GxEPD_WHITE);
                }
            }
        }

    } while (display.nextPage());
    display.hibernate();
}

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