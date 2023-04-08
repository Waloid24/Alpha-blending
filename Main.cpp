#include "Alpha-blend.hpp"

using namespace sf;

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

    MY_ASSERT (!background.saveToFile("Result.png"), "It is impossible to save the final image");

    return 0;
}