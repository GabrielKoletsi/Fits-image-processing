#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>

const char* FILENAME = "ngc1316o.fit";
const int HEADER_SIZE = 2880;//header size in bytes
const int WINDOW_X = 440;
const int WINDOW_Y = 300;

SDL_Window* window = nullptr;
SDL_Renderer* render = nullptr;

void initializeSDL(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL. Error: %s\n", SDL_GetError());
        return;
    }

    //generate the window
    window = SDL_CreateWindow("Fits Image Render", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_SHOWN);

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
std::vector<Uint32> binaryToRGB(std::vector<long double> image){
    std::vector<Uint32> RGBpixelData;
    int pixelColor;
    Uint32 pixelRGB;
    for(long double pixelValue : image){
        pixelColor = pixelValue * 255; //convert from long double to int in range 0-255
        pixelRGB = SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGB24), pixelColor, pixelColor, pixelColor);
        RGBpixelData.push_back(pixelRGB);
    }

    return RGBpixelData;
}

//creates the SDL texture to render it
void SDLTexture(std::vector<Uint32> &RGBpixelData){
    
    SDL_Surface* imageSurface = SDL_CreateRGBSurfaceFrom(&RGBpixelData[0], WINDOW_X, WINDOW_Y, 16, WINDOW_X * 3, 0, 0, 0, 0);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(render, imageSurface);

    //clear render
    SDL_RenderClear(render);

    //Render texture
    SDL_RenderCopy(render, imageTexture, nullptr, nullptr);

    //update screen
    SDL_RenderPresent(render);

    //waits x ms before shutting down.
    SDL_Delay(30000);

    //free resources
    SDL_FreeSurface(imageSurface);
    SDL_DestroyTexture(imageTexture);
}


void destroySDL(){
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}


//takes maximum value of stored pixels from 0.0 to 1
void normalizePixelValue(std::vector<long double>& image){
    //goes through vector to ensure all values are positive.
    for(int i = 0; i < (int) image.size(); ++i){
        image[i] = abs(image[i]);
    }

    //finds maximum value
    long double max = 0;
    for(int i = 0; i < (int) image.size(); ++i){
        if (max < image.size()){
            max = image[i];
        }
    }

    //divides everything by the max pixel value to normalize the range between 0.0 and 1.0
    if(max != 0){
        for(int i = 0; i < (int) image.size(); ++i){
            image[i] = image[i]/max;
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
        std::cout << "File not found." << std::endl;
        return nullptr;
    }
}

char* getHeader(std::ifstream* file){
    char* header = new char[HEADER_SIZE];//FITS header has a fixed length of 2880 bytes

    file->read(header, HEADER_SIZE);
    
    return header;
}

std::vector<long double> getImage(std::ifstream* file){
    file->seekg(0, std::ios::end);
    //std::streamsize dataSize = file->tellg();
    file->seekg(0, std::ios::beg);


    std::vector<long double> data;
    long double temp;

    while(file->read(reinterpret_cast<char*>(&temp), sizeof(temp))){
        data.push_back(temp);
    }

    return data;
}


//creates vector for testing purposes
std::vector<long double> testImageGen(){
    std::vector<long double> image;
    image.push_back(10);
    for (int i = 1; i < 512 * 512; ++i){
        image.push_back(0);
    }
    return image;
}


int main(){
    std::ifstream* file = createFileStream();//Stores File Stream in var
    
    char* header = getHeader(file);//Stores Header
    std::vector<long double> image = getImage(file);//stores binary data
    
    //uncomment to use testing function
    //std::vector<Uint32> RGBpixelData = binaryToRGB(testImageGen());
    std::vector<Uint32> RGBpixelData = binaryToRGB(image);//stores converted binary data to rgb

    

    



    std::cout << "Max index  ,  " << image[15000] << "  ,  "<< std::endl;

    initializeSDL();
    SDLTexture(RGBpixelData);


    delete[] header;
    delete file;
    
    return 0;
}