#ifndef STARS_H
#define STARS_H

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>


//contains the data of a pixel of a star
class StarPixel{
    public:
        //Default constructor
        StarPixel();

        //Overloaded constructor for use when SDL_Color object is already available
        StarPixel(int px, int py, SDL_Color pixel);

        //overloaded constructor for use when SDL_Color object isn't directly available.
        StarPixel(int px, int py, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

        int x;
        int y;
        SDL_Color pixelValue;
};


/// @brief Star class contains information about a star detected in the image;
class Star{
    public: 
    int avgX, avgY;

    /// @brief Default constructor
    Star();
    
    /// @brief Overloaded constructor, adds a Star Pixel to the star object
    /// @param pixel Pixel that belongs to star object
    Star(StarPixel pixel);

    /// @brief Get star size (in pixels contained).
    /// @return Number of pixels.
    size_t getSize();
    
    /// @brief Getter for the vector containing pixels of the star object.
    /// @return Vector containing pixel data that belong to the star object.
    std::vector<StarPixel> getStarBlob();

    /// @brief Updates the star size.
    void updateSize();

    /// @brief Adds pixel to the Star object.
    /// @param pixel Data of pixel to be added. 
    void addPixel(StarPixel pixel);

    /// @brief Prints the data of every pixel belonging to Star object to the terminal for debugging purposes.
    void printPixels();

    /// @brief Stores the memory address of the closest star and initializes distanceFromClosestStar
    /// @param star Star object
    void setClosestStar(Star* star);

    /// @brief Getter for closest star address.
    /// @return Memory address of closest star. 
    Star* getClosestStar();

    /// @brief Getter for the distance from closest star.
    /// @return Distance from closest star.
    int getDistanceFromClosestStar();


    private:
        std::vector<StarPixel> starBlob;
        size_t starSize;//stores size of starBlob vector
        uint64_t sumX;//used to measure the average value of x
        uint64_t sumY;//used to measure the average value of y
        Star* closestStar;//stores the address of the closest star
        int distanceFromClosestStar;
        
};


/// @brief Takes two points and find the distance between them in pixels.
/// @param x 
/// @param y 
/// @param px 
/// @param py 
/// @return Distance between two points.
int calculateDistance(int x, int y, int px, int py);

/// @brief Finds and assigns the closest star for every star object.
/// @param stars 
void findClosestStar(std::vector<Star>& stars);

#endif