#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>


/// @brief Generates Gaussian Kernel
/// @param sigma Standard deviation
/// @param KernelDimension Dimension of kernel
/// @return Gaussian Kernel vector
std::vector<std::vector<double>> GaussianKernel(double sigma, int KernelDimension);


/// @brief Applies Gaussian filter. 
/// @param colorVec Vector of SDL_Color, containing the image to be blurred. 
/// @param sigma Standard deviation in the creation of the Kernel 
/// @param KernelDimension The size of the kernel used
/// @return SDL_Color vector with Gaussian filter applied
std::vector<SDL_Color> GaussianBlur(const std::vector<SDL_Color>& colorVec,const size_t x_axis, double sigma, int KernelDimension);


/// @brief uses box blur algorithm for convolution
/// @param colorVec vector storing SDL_Color elements. Image is loaded here with r, g, b, a values
/// @return image with box blur applied. Image is stored in SDL_Color vector
std::vector<SDL_Color> boxBlur(const std::vector<SDL_Color>& colorVec, size_t x_axis);

#endif