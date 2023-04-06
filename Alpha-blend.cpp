#include "Alpha-blend.hpp"

using namespace sf;

void blend (Image * background, Image * foreground, Vector2i FPosition);
static uint32_t mixColors (uint32_t FColorComp, uint32_t BColorComp, uint32_t FAlpha);
static void changeArrayNoAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                              uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                              uint32_t * dest, uint32_t X, uint32_t Y);
                              
int main (int argc, char* argv[])
{
    MY_ASSERT (argc != 5, "You need to enter 5 arguments: executable file, the name of the file with background"
                "(first) and the name of the file with foreground (second), the x-axis coordinate of foreground picture" 
                " and the y-axis coordinate of foreground picture");

    Image background;
    MY_ASSERT (background.loadFromFile (argv[1]) != true, "Unable to open file with background");

    Image foreground;
    MY_ASSERT (foreground.loadFromFile (argv[2]) != true, "Unable to open file with foreground");

    uint32_t xPos = strtol (argv[3], nullptr, 10);
    MY_ASSERT (errno == ERANGE, "The x-axis coordinate must have an unsigned integer value within the unsigned int");

    uint32_t yPos = strtol (argv[4], nullptr, 10);
    MY_ASSERT (errno == ERANGE, "The y-axis coordinate must have an unsigned integer value within the unsigned int");

    Vector2i FPosition (xPos, yPos);

    blend (&background, &foreground, FPosition);

    MY_ASSERT (!background.saveToFile("result.png"), "It is impossible to save the final image");

    return 0;
}

void blend (Image * background, Image * foreground, Vector2i FPosition)
{
    uint32_t X = FPosition.x;
    uint32_t Y = FPosition.y;

    uint32_t FWidth  = foreground->getSize().x;
    uint32_t FHeight = foreground->getSize().y;

    uint32_t BWidth  = background->getSize().x;
    uint32_t BHeight = background->getSize().y;

    printf ("BWidth = %u, BHeight = %u\n", BWidth, BHeight);

    const uint32_t * backGr = (const uint32_t *) background->getPixelsPtr();
    const uint32_t * foreGr = (const uint32_t *) foreground->getPixelsPtr();

    uint32_t * dest = (uint32_t *) backGr;

    #ifdef AVX
        changeArrayAvx (FWidth, FHeight, foreGr, BWidth, BHeight, backGr, dest, X, Y);
    #else
        changeArrayNoAvx (FWidth, FHeight, foreGr, BWidth, BHeight, backGr, dest, X, Y);
        
    #endif
}

static uint32_t mixColors (uint32_t FColorComp, uint32_t BColorComp, uint32_t FAlpha)
{
    return (FColorComp*FAlpha + BColorComp*(255 - FAlpha))/255;
}

static void changeArrayNoAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                              uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                              uint32_t * dest, uint32_t X, uint32_t Y)
{
    for (uint32_t y = 0; y < FHeight; y++)
    {
        for (uint32_t x = 0; x < FWidth; x++)
        {
            if (x + X >= BWidth || y + Y >= BHeight) continue;

            uint32_t FPixel = foreGr[x     + y*FWidth];
            uint32_t BPixel = backGr[x + X + (y+Y)*BWidth];

            uint32_t FAlpha = FPixel >> 24;
            uint32_t BAlpha = BPixel >> 24;

            uint32_t result = BAlpha << 24;

            for (int i = 0; i < 3; i++)
            {
                uint32_t FColorComp = ((0xFF << (i*8)) & FPixel) >> (i*8);
                uint32_t BColorComp = ((0xFF << (i*8)) & BPixel) >> (i*8);

                result += mixColors (FColorComp, BColorComp, FAlpha) << (8*i);
            }
            dest[x + X + (y+Y)*BWidth] = result;
        }
    }
}
