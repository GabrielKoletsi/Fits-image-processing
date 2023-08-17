#include "starDetectionAlgorithm.h"


bool checkPixelSurroundings(int x_, int y_, std::vector<SDL_Color>& colorVec){
    //compares the 8 pixels surrounding the pixel examined for being above threshold
    int valueSum = 0; //hold the sum of the surrounding pixel values
    uint8_t avgValue;
    if (y_ < 5 || y_ > (Y_AXIS - 5) || x_ < 5 || x_ > (X_AXIS - 5)){
        return false;
    }
    for (uint16_t y = y_ - 1; y < y_ + 2; ++y){
        for (uint16_t x = x_ - 1; x < x_ + 2; ++x){
            if(x != x_ && y != y_){
                valueSum += colorVec[y * X_AXIS + x].b;
            }
        }
    }
    avgValue = valueSum / 8;
    if(avgValue > THRESHOLD){return false;}
    return true;
}


int32_t returnStarIndex(int x, int y, std::vector<Star> &stars){
    //if the stars vector is empty, return -1
    if(stars.size() == 0){return -1;}
    uint32_t minDistance; //minimum distance between a pixel compared to a pixel belonging to a star
    uint16_t distance;
    uint16_t diffXsq;//the difference of the x coordinates squared
    uint16_t diffYsq;//the difference of the y coordinates squared 
    std::vector<StarPixel> starBlob;
    for (uint32_t i = 0; i < stars.size(); ++i){
        starBlob = stars[i].getStarBlob();
        minDistance = sqrt(X_AXIS * X_AXIS + Y_AXIS * Y_AXIS);//sets minimum distance to the maximum distance possible 
        for(uint64_t j = 0; j < starBlob.size(); ++j){    
            diffXsq = (x - starBlob[j].x) * (x - starBlob[j].x);
            diffYsq = (y - starBlob[j].y) * (y - starBlob[j].y);
            distance = sqrt(diffXsq + diffYsq);
            if(minDistance > distance){
                minDistance = distance;
            }
            if (minDistance < MINIMUM_DISTINCTION_DISTANCE){
                return i;
            }
        }
    }
    return -1;
} 


void addToStarClassVector(std::vector<SDL_Color> &colorVec, std::vector<Star>& stars){
    int32_t index;//holds return value of returnStarIndex
    int x,y;
    for (size_t i = 0; i < colorVec.size(); ++i){
        if ((i / X_AXIS) < 5 || (i / X_AXIS) > (Y_AXIS - 5) || (i % X_AXIS) < 5 || (i % X_AXIS) > (X_AXIS - 5)){
            
        }else{
            //if pixel intensity is above threshold
            if(colorVec[i].b > THRESHOLD){

                //and if the 8 surrounding pixels are also above threshold
                x = i % X_AXIS;
                y = i / X_AXIS;
                
                if(checkPixelSurroundings(x, y, colorVec)){
                    index = returnStarIndex(x, y, stars);
                    if(index == -1){
                        //creates new star object and adds pixel to it
                        stars.push_back(Star(StarPixel(x, y, colorVec[i])));
                    }else{
                        //adds pixel to already existing star object
                        stars[index].addPixel(StarPixel(x, y, colorVec[i]));
                    } 
                }

            }
        }
    }
}