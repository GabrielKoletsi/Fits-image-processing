#include "ImageFilters.h"


std::vector<std::vector<double>> GaussianKernel(double sigma, int KernelDimension){
    std::vector<std::vector<double>> GKernel;
    std::vector<double> vec;
    
    for(int j = 0; j < KernelDimension; ++j){
        for(int i = 0; i < KernelDimension; ++i){
            vec.push_back(0); 
        }
        GKernel.push_back(vec);
    }
    //iniatializing standard deviation
    double r, s = 2.0 * sigma * sigma;

    //sum is used for normalization
    double sum = 0.0;

    //generating 5x5 kernel
    for(int x = -2; x <= 2; ++x){
        for(int y = -2; y <= 2; ++y){
            r = sqrt(x * x + y * y);
            GKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += GKernel[x + 2][y + 2];
        }
    }

    for (int i = 0; i < 5; ++i){
        for (int j = 0; j < 5; ++j){
            GKernel[i][j] /= sum;
        }
    }

    return GKernel;
}


std::vector<SDL_Color> GaussianBlur(const std::vector<SDL_Color>& colorVec, const size_t x_axis, double sigma, int KernelDimension){
    const uint16_t BORDER_DISTANCE = 5;
    size_t valueSum;//sum of all surrounding pixel values
    uint8_t avgValue;//the average value of surrounding pixels using kernel
    double kernelSum;//the sum of all the kernel values
    std::vector<std::vector<double>> GKernel = GaussianKernel(sigma, KernelDimension);
    std::vector<SDL_Color> imageBlurred;
    SDL_Color color;
    uint16_t h, w;//indexes to keep track of kernel iterations

    for(size_t i = 0; i < colorVec.size(); ++i){
        valueSum = kernelSum = h = w = 0;

        //checks if i is not within 5 pixels of the edges
        if(i > (x_axis * BORDER_DISTANCE) && (i % x_axis) > BORDER_DISTANCE && i < ((i / x_axis - BORDER_DISTANCE) * x_axis) && (i % x_axis) < (x_axis - BORDER_DISTANCE)){
            
            for (uint16_t y = (i / x_axis) - 2; y < (i / x_axis) + 3; ++y){
                for (uint16_t x = (i % x_axis) - 2; x < (i % x_axis) + 3; ++x){
                    valueSum += colorVec[y * x_axis + x].b * GKernel[h][w];
                    kernelSum += GKernel[h][w];
                    ++w;
                }
                w = 0;
                ++h;
            }
            avgValue = valueSum / kernelSum;
            color.b = color.g = color.r = avgValue;
            color.a = 255;
            imageBlurred.push_back(color);
        }else{
            imageBlurred.push_back(colorVec[i]);
        }
    }
    return imageBlurred;

}


std::vector<SDL_Color> boxBlur(const std::vector<SDL_Color>& colorVec, size_t x_axis){
    size_t valueSum;//sum of all surrounding pixel values
    SDL_Color avgValue;//the average value of surrounding pixels
    uint16_t index;
    std::vector<SDL_Color> imageBlurred;
    avgValue.a = 0xFF;
    for(size_t i = 0; i < colorVec.size(); ++i){
        //checks if i is not within 5 pixels of the edges
        valueSum = 0;
        index = 0;
 
        if(i > (size_t) (x_axis * 5) && (i % x_axis) > 5 && i < ((i / x_axis - 5) * x_axis) && (int) (i % x_axis) < (int) (x_axis - 5)){
            for (uint16_t y = (i / x_axis) - 2; y < (i / x_axis) + 3; ++y){
                for (uint16_t x = (i % x_axis) - 2; x < (i % x_axis) + 3; ++x){
                    valueSum += colorVec[y * x_axis + x].b;
                    index+=1;
                }
            }
            avgValue.b = avgValue.r = avgValue.g = valueSum / index;
            imageBlurred.push_back(avgValue);
        }else{
            imageBlurred.push_back(colorVec[i]);
        }
    }
    return imageBlurred;
}