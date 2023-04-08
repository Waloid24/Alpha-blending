#include "Alpha-blend.hpp"

using namespace sf;
const int NUM_ITERS = 100;

static uint32_t mixColors (uint32_t FColorComp, uint32_t BColorComp, uint32_t FAlpha);
static void changeArrayNoAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                              uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                              uint32_t * dest, uint32_t X, uint32_t Y);
static void changeArrayAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                            uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                            uint32_t * dest, uint32_t X, uint32_t Y);

void blend (Image * background, Image * foreground, Vector2i FPosition)
{
    uint32_t X = FPosition.x;
    uint32_t Y = FPosition.y;

    uint32_t FWidth  = foreground->getSize().x;
    uint32_t FHeight = foreground->getSize().y;

    uint32_t BWidth  = background->getSize().x;
    uint32_t BHeight = background->getSize().y;

    const uint32_t * backGr = (const uint32_t *) background->getPixelsPtr();
    const uint32_t * foreGr = (const uint32_t *) foreground->getPixelsPtr();

    uint32_t * dest = (uint32_t *) backGr;

    // double secs = 0;

    // for (int i = 0; i < NUM_ITERS; i++) // measurements
    // {
    //     clock_t start = clock();
        #ifdef AVX
            changeArrayAvx   (FWidth, FHeight, foreGr, BWidth, BHeight, backGr, dest, X, Y);
        #else
            changeArrayNoAvx (FWidth, FHeight, foreGr, BWidth, BHeight, backGr, dest, X, Y);
        #endif
    //     clock_t end = clock();
        
    //     secs += (double)(end-start)/CLOCKS_PER_SEC; // measurements
    // }
    // printf ("The time: %f seconds\n", secs);
}

static uint32_t mixColors (uint32_t FColorComp, uint32_t BColorComp, uint32_t FAlpha)
{
    return (FColorComp*FAlpha + BColorComp*(255 - FAlpha))/255;
}

static void changeArrayNoAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                              uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                              uint32_t * dest, uint32_t X, uint32_t Y)
{
    MY_ASSERT (foreGr == nullptr, "It is not impossible to access the array of pixels of the foreground");
    MY_ASSERT (backGr == nullptr, "It is not impossible to access the array of pixels of the background");

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

static void changeArrayAvx (uint32_t FWidth, uint32_t FHeight, const uint32_t * foreGr,
                            uint32_t BWidth, uint32_t BHeight, const uint32_t * backGr,
                            uint32_t * dest, uint32_t X, uint32_t Y)
{
    MY_ASSERT (foreGr == nullptr, "It is not impossible to access the array of pixels of the foreground");
    MY_ASSERT (backGr == nullptr, "It is not impossible to access the array of pixels of the background");

    const unsigned int I = 0xFFFFFFFF;
    const unsigned char Z = 0xFF;
    
    __m256i _0 = _mm256_set1_epi32 (0);
    const __m256i _I = _mm256_set1_epi16 (0xff);

    for (unsigned int y = 0; y < FHeight; y++)
    {
        for (unsigned int x = 0; x < FWidth; x += 4)
        {
            __m128i FPixel = _mm_load_si128 ((__m128i *) &(foreGr[(y*FWidth + x)]));
            __m128i BPixel = _mm_load_si128 ((__m128i *) &(backGr[(y+Y)*BWidth + x+X]));

            __m256i fr     = _mm256_cvtepu8_epi16 (FPixel);
            __m256i bk     = _mm256_cvtepu8_epi16 (BPixel);

            __m256i maskA  = _mm256_set_epi8 (
                                Z, 0x0e, Z, 0x0e, Z, 0x0e, Z, 0x0e,
                                Z, 0x06, Z, 0x06, Z, 0x06, Z, 0x06,
                                Z, 0x0e, Z, 0x0e, Z, 0x0e, Z, 0x0e,
                                Z, 0x06, Z, 0x06, Z, 0x06, Z, 0x06);

            __m256i a = _mm256_shuffle_epi8 (fr, maskA);

            fr = _mm256_mullo_epi16 (fr, a);
            bk = _mm256_mullo_epi16 (bk, _mm256_sub_epi16 (_I, a));

            __m256i sum = _mm256_add_epi16 (fr, bk);

            const __m256i moveSum = _mm256_set_epi8 (
                                0x0f, 0x0d, 0x0b, 0x09, 0x07, 0x05, 0x03, 0x01,
                                Z,   Z,   Z,   Z,   Z,   Z,   Z,   Z,
                                Z,   Z,   Z,   Z,   Z,   Z,   Z,   Z,
                                0x0f, 0x0d, 0x0b, 0x09, 0x07, 0x05, 0x03, 0x01);

            sum = _mm256_shuffle_epi8 (sum, moveSum);

            __m128i color = _mm_add_epi64(_mm256_extracti128_si256(sum, 1), _mm256_castsi256_si128(sum));

            _mm_store_si128 ((__m128i *) &(backGr[(y+Y)*BWidth + x+X]), color);
        }
    }
}