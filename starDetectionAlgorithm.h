#ifndef STARDETECTIONALGORITHM_H
#define STARDETECTIONALGORITHM_H

#include "Stars.h"
#include "renderer.h"

#define THRESHOLD 70
#define MINIMUM_DISTINCTION_DISTANCE 15


/// @brief Checks the surrounding pixels of the pixel coordinates entered to see if they are above THRESHOLD.
/// @param x_ X coordinate of pixel to be checked.
/// @param y_ Y coordinate of pixel to be checked.
/// @param colorVec Vector of SDL_Color object containing image data.
/// @return True if surrounding pixels are above threshold, otherwise false.
bool checkPixelSurroundings(int x_, int y_, std::vector<SDL_Color>& colorVec);

/// @brief Determines if the pixel belongs to a star object that is already constructed.
/// @param x X coordinate of pixel to be checked.
/// @param y Y coordinate of pixel to be checked.
/// @param stars Vector containing all Star objects.
/// @return If star is found, returns index of star in vector of Star objects, otherwise returns -1.
int32_t returnStarIndex(int x, int y, std::vector<Star> &stars);

/// @brief Checks if a pixel could be part of a star, if it is then it appends it to one of the existing Star objects or creates a new one.
/// @param colorVec Vector of SDL_Color object, contains image.
/// @param stars Vector of Star objects.
void addToStarClassVector(std::vector<SDL_Color> &colorVec, std::vector<Star>& stars);

#endif