#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FILENAME "dss_search"
#define HEADER_SIZE 2880
#define X_AXIS 891
#define Y_AXIS 893


SDL_Window* window = nullptr;
SDL_Renderer* render = nullptr;

void initializeSDL(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL. Error: %s\n", SDL_GetError());
        return;
    }

    //generate the window
    window = SDL_CreateWindow("Fits Image Render", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, X_AXIS, Y_AXIS, SDL_WINDOW_SHOWN);

    if(!window){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window. Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    //generate the render
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!render){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer. Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    return;
}


//converts binary to RGB format understandable by SDL2
//NOTE: idk what I was thinking at this point, ignore
//function not being used
std::vector<uint16_t> binaryToRGB(std::vector<uint16_t> image){
    std::vector<uint16_t> RGBpixelData;
    int pixelColor;
    Uint32 pixelRGB;
    for(uint16_t pixelValue : image){
        pixelColor = pixelValue * 255; //convert from long double to int in range 0-255
        //Here i thought that using the same value for R,G and B would result in grayscale, apparently not lul
        pixelRGB = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGB24), pixelColor, pixelColor, pixelColor);
        RGBpixelData.push_back(pixelRGB);
    }


    //uncomment to return whatever abomination of a vector this function creates
    //return RGBpixelData;
    return image;
}

//creates the SDL texture to render it
void SDLTexture(std::vector<uint16_t> &imageVec){
    
    SDL_Texture* imageTexture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR4444, SDL_TEXTUREACCESS_STATIC, X_AXIS, Y_AXIS);
    SDL_UpdateTexture(imageTexture, nullptr, imageVec.data(), X_AXIS * sizeof(uint16_t));

    //clear render
    SDL_RenderClear(render);

    //Render texture
    SDL_RenderCopy(render, imageTexture, nullptr, nullptr);

    //update screen
    SDL_RenderPresent(render);

    //waits x ms before shutting down.
    SDL_Delay(2000);

    //free resources
    SDL_DestroyTexture(imageTexture);
}


void destroySDL(){
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}


//takes maximum value of stored pixels from 0.0 to 1
void normalizePixelValue(std::vector<uint16_t>& imageVec){
    //goes through vector to ensure all values are positive.
    for(int i = 0; i < (int) imageVec.size(); ++i){
        imageVec[i] = abs(imageVec[i]);
    }

    //finds maximum value
    uint16_t max = 0;
    for(int i = 0; i < (int) imageVec.size(); ++i){
        if (max < imageVec.size()){
            max = imageVec[i];
        }
    }

    //divides everything by the max pixel value to normalize the range between 0.0 and 1.0
    if(max != 0){
        for(int i = 0; i < (int) imageVec.size(); ++i){
            imageVec[i] = imageVec[i]/max;
        }
    }
}

std::ifstream* createFileStream(){

    //using pointers to be able to return the loaded file
    std::ifstream* file = new std::ifstream(FILENAME, std::ios::binary);

    if (file->is_open()){
        std::cout << "File loaded." << std::endl;
        return file;
    }else{
        std::cerr << "File not found." << std::endl;
        return nullptr;
    }
}

char* getHeader(std::ifstream* file){
    char* header = new char[HEADER_SIZE];//FITS header has a fixed length of 2880 uint16_ts

    file->read(header, HEADER_SIZE);
    
    return header;
}

std::vector<uint16_t> getImage(std::ifstream* file){
    //this is a very janky way of seeking to some point
    //to remove artifacts on top of the image and to
    //also center it
    //NEED PERMANENT SOLUTION AT END OF PROJECT
    file->seekg(12600, std::ios::beg);

    std::vector<uint16_t> imageVec;
    uint16_t pixel;
    while(file->read(reinterpret_cast<char*>(&pixel), sizeof(uint16_t))){
        imageVec.push_back(pixel);
    }

    return imageVec;
}


int main(){
    std::ifstream* file = createFileStream();//Stores File Stream in var

    char* header = getHeader(file);//Stores Header

    std::vector<uint16_t> imageVec = getImage(file);//stores binary data

    
    //std::vector<uint16_t> RGBpixelData = binaryToRGB(imageVec);//stores converted binary data to rgb

    initializeSDL();
    SDLTexture(imageVec);
    destroySDL();





    delete[] header;
    delete file;
    
    return 0;
}
