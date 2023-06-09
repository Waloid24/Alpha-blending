#ifndef ALPHA_BLEND_HPP
#define ALPHA_BLEND_HPP

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <immintrin.h>
#include <stdlib.h>
#include <time.h>
#include "MY_ASSERT.h"

void blend (sf::Image * background, sf::Image * foreground, sf::Vector2i FPosition);

#endif