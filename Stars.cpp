#include "Stars.h"


StarPixel::StarPixel(){
    x = y = 0;
    pixelValue = {0, 0, 0, 0};
}

StarPixel::StarPixel(int px, int py, SDL_Color pixel){
    x = px;
    y = py;
    pixelValue = pixel;
}

StarPixel::StarPixel(int px, int py, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    x = px;
    y = py;
    pixelValue.r = r;
    pixelValue.g = g;
    pixelValue.b = b;
    pixelValue.a = a;
}


Star::Star(){
    sumX = sumY = 0;
    closestStar = nullptr;
}

Star::Star(StarPixel pixel){
    sumX = sumY = 0;
    addPixel(pixel);
    closestStar = nullptr;
}

size_t Star::getSize(){return starSize;}

std::vector<StarPixel> Star::getStarBlob(){return starBlob;}

void Star::updateSize(){starSize = starBlob.size();}

void Star::addPixel(StarPixel pixel){
    starBlob.push_back(pixel);
    updateSize();
    sumX += pixel.x;
    sumY += pixel.y;
    avgX = sumX / starSize;
    avgY = sumY / starSize;
}

void Star::printPixels(){
    for(StarPixel i : starBlob){
        std::cout << i.x << " " << i.y << " " << (uint8_t) i.pixelValue.b << std::endl;
    }
}

void Star::setClosestStar(Star* star){
    closestStar = star;
    if(this->closestStar != nullptr){
        distanceFromClosestStar = calculateDistance(this->avgX, this->avgY, closestStar->avgX, closestStar->avgY);
    }
}

Star* Star::getClosestStar(){
    return closestStar;
}

int Star::getDistanceFromClosestStar(){return distanceFromClosestStar;}


int calculateDistance(int x, int y, int px, int py){
    int diffXsq = (x - px) * (x - px);
    int diffYsq = (y - py) * (y - py);
    int distance = sqrt(diffXsq + diffYsq);
    return distance;
}


void findClosestStar(std::vector<Star>& stars){
    uint32_t minDistance;//Minimum distance between stars
    uint32_t x1, x2, y1, y2;
    uint32_t distance;
    for(uint32_t i = 0; i <  stars.size(); ++i){
        minDistance = 0xFFFF;
        for(uint32_t j = 0; j < stars.size(); ++j){
            if(&stars[i] != &stars[j]){
                x1 = stars[i].avgX, x2 = stars[j].avgX;
                y1 = stars[i].avgY, y2 = stars[j].avgY;

                distance = calculateDistance(x1, y1, x2, y2);
                if(minDistance >= distance){
                    minDistance = distance;
                    stars[i].setClosestStar(&stars[j]);
                }
            }
        }
    }
}