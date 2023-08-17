#ifndef RENDERER_H
#define RENDERER_H

#include "Stars.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "fileio.h"

#define X_AXIS 891
#define Y_AXIS 893

extern SDL_Window* window;
extern SDL_Renderer* render;
const SDL_Color green = {0, 255, 0, 255};

/// @brief Initializes SDL, SDL_Window, SDL_Renderer and TTF while checking for errors
void initializeSDL();

/// @brief draws red circle 
/// @param x_axis x coordinate of center of circle
/// @param y_axis y coordinate of center of circle 
/// @param radius radius of circle
void drawCircle(int x_axis, int y_axis, int radius);

/// @brief Renders the text contained in the string "text"
/// @param x_ x coordinate used
/// @param y_ y coordinate used
/// @param text text to render
/// @param font font to use for the text
/// @param color color of text
void printText(int x_, int y_, std::string text, TTF_Font* font);


/// @brief Calls drawCircle() and printText() by iterating through the stars vector
/// @param stars the vector containing Star objects
/// @param font TTF font for rendering the index of the star
/// @param color color used for rendering the text
void circleStars(std::vector<Star> stars, TTF_Font* font);

/// @brief Renders pixel coordinates and intensity for the pixel hovered over with the mouse.
/// @param x X coordinate of the pixel.
/// @param y Y coordinate of the pixel.
/// @param colorVec Vector of SDL_Color objects (image). 
/// @param font Font to be used when rendering the text.
void createPixelInfoRect(int x, int y, std::vector<SDL_Color>& colorVec, TTF_Font* font);


std::vector<SDL_Color> convertToColor(std::vector<uint16_t>& imageVec);


/// @brief Linear histogram to change the lighting levels of the image.
/// @param colorVec vector containing image in SDL_Color format
/// @param F value by which to multiply all values
/// @note Alters the vector passed as a parameter itself. If F = 1 then no action is taken.
void linearHistogram(std::vector<SDL_Color>& colorVec, double F);

/// @brief Creates image texture and handles all other rendering happening.
/// @param colorVec Image stored in vector of SDL_Color objects.
/// @param stars Vector containing detected stars.
void SDLTexture(std::vector<SDL_Color> &colorVec, std::vector<Star> stars);


void destroySDL();

/// @brief Converts a vector of SDL_Color objects to StarPixel.
/// @param colorVec Vector of SDL_Color objects, contains image.
/// @return vector of StarPixel objects.
std::vector<StarPixel> VectorSDL_ColorToStarPixelFormat(std::vector<SDL_Color> colorVec);

#endif