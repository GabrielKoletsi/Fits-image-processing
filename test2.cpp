#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FILENAME "dss_search"
#define HEADER_SIZE 2880
#define X_AXIS 891
#define Y_AXIS 893
#define THRESHOLD 25000
#define MINIMUM_DISTINCTION_DISTANCE 15 //minimum distance of two star center coordinates to be considered different stars  

SDL_Window* window = nullptr;
SDL_Renderer* render = nullptr;


//contains the data of a pixel of a star
struct StarPixel{
    int x;
    int y;
    uint16_t intensity;
};

//coordinates of star object center
struct Center{
    uint16_t avgX;//stores the average x value of every pixel
    uint16_t avgY;//stores the average y value of every pixel
};

//contains data of a star
class Star{
    public: 
    Center centerPixel;
    Star(){
        sumX = sumY = 0;
    }
    
    Star(StarPixel pixel){
        sumX = sumY = 0;
        addPixel(pixel);
    }

    size_t getSize(){return starSize;}
    
    void updateSize(){starSize = starBlob.size();}

    void addPixel(StarPixel pixel){
        starBlob.push_back(pixel);
        updateSize();
        sumX += pixel.x;
        sumY += pixel.y;
        centerPixel.avgX = sumX / starSize;
        centerPixel.avgY = sumY / starSize;
    }

    void printPixels(){
        for(StarPixel i : starBlob){
            std::cout << i.x << " " << i.y << " " << i.intensity << std::endl;
        }
    }

    private:
        std::vector<StarPixel> starBlob;
        size_t starSize;//stores size of starBlob vector
        uint64_t sumX;//used to measure the average value of x
        uint64_t sumY;//used to measure the average value of y
        
};

//returns a vectors of StarPixel Format
std::vector<StarPixel> UintConvertToStarPixelFormat(std::vector<uint16_t> imageVec){
    std::vector<StarPixel> stars;
    StarPixel Spixel;
    for (size_t i = 0; i < imageVec.size(); ++i){
        Spixel.intensity = imageVec[i];
        Spixel.x = i % X_AXIS;
        Spixel.y = i / X_AXIS;
        stars.push_back(Spixel);
    }
    return stars;
}

//checks for neighboring pixels above threshold
//if one is found under threshold then returns false
//else returns true
bool checkPixelSurroundings(StarPixel& pixel, std::vector<StarPixel>& imageVec){
    //compares the 8 pixels surrounding the pixel examined for being above threshold
    if (pixel.y < 5 || pixel.y > (Y_AXIS - 5) || pixel.x < 5 || pixel.x > (X_AXIS - 5)){
        return false;
    }
    for (uint16_t y = pixel.y - 3; y < pixel.y + 4; ++y){
        for (uint16_t x = pixel.x - 3; x < pixel.x + 4; ++x){
            if (imageVec[y * X_AXIS + x].intensity < THRESHOLD){
                return false;
            }
        }
    }
    return true;
}


//This checks the distance of a validated pixel from any star object centers.
//if distance is larger than MINIMUM_DISTINCTION_DISTANCE then a new star object is created.
//if there are no stars a star object is created.
//returns index if close to another star object.
//returns -1 if is far or vector is empty.
//NOTE: returns on first object found closest to center. This may be a problem if two stars are close to each other.
int32_t returnStarIndex(StarPixel& Spixel, std::vector<Star> &stars){
    //if the stars vector is empty, return -1
    if(stars.size() == 0){return -1;}
    uint16_t distance;
    uint16_t diffXsq;//the difference of the x coordinates squared
    uint16_t diffYsq;//the difference of the y coordinates squared 
    for (uint32_t i = 0; i < stars.size(); ++i){
        diffXsq = (Spixel.x - stars[i].centerPixel.avgX) * (Spixel.x - stars[i].centerPixel.avgX);
        diffYsq = (Spixel.y - stars[i].centerPixel.avgY) * (Spixel.y - stars[i].centerPixel.avgY);
        distance = sqrt(diffXsq + diffYsq);
        if (distance < MINIMUM_DISTINCTION_DISTANCE){
            return i;
        }
    }
    return -1;
    
}

void addToStarClassVector(std::vector<StarPixel> &starPixelVec, std::vector<Star>& stars){
    int32_t index;//holds return value of returnStarIndex
    for (size_t i = 0; i < starPixelVec.size(); ++i){
        //if pixel intensity is above threshold
        if(starPixelVec[i].intensity > THRESHOLD){
            //and if the 8 surrounding pixels are also above threshold
            if(checkPixelSurroundings(starPixelVec[i], starPixelVec)){
                index = returnStarIndex(starPixelVec[i], stars);
                if(index == -1){
                    //creates new star object and adds pixel to it
                    stars.push_back(Star(starPixelVec[i]));
                }else{
                    //adds pixel to already existing star object
                    stars[index].addPixel(starPixelVec[i]);
                } 
            }

        }
    }
}

std::vector<std::vector<double>> gaussianKernel(){
    int KernelDimension = 5;
    std::vector<std::vector<double>> GaussianKernel;
    std::vector<double> vec = {0, 0, 0, 0, 0};
    
    for(int i = 0; i < KernelDimension; ++i){
        GaussianKernel.push_back(vec); 
    }
    
    //iniatializing standard deviation
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;

    //sum is used for normalization
    double sum = 0.0;

    //generating 5x5 kernel
    for(int x = -2; x <= 2; ++x){
        for(int y = -2; y <= 2; ++y){
            r = sqrt(x * x + y * y);
            GaussianKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += GaussianKernel[x + 2][y + 2];
        }
    }

    for (int i = 0; i < 5; ++i){
        for (int j = 0; j < 5; ++j){
            GaussianKernel[i][j] /= sum;
        }
    }

    return GaussianKernel;
}

std::vector<uint16_t> gaussianBlur(const std::vector<StarPixel>& imageVec){
    const uint16_t BORDER_DISTANCE = 5;
    size_t valueSum;//sum of all surrounding pixel values
    uint16_t avgValue;//the average value of surrounding pixels using kernel
    double kernelSum;//the sum of all the kernel values
    std::vector<std::vector<double>> GKernel = gaussianKernel();
    std::vector<uint16_t> imageBlurred;
    uint16_t h, w;//indexes to keep track of kernel iterations

    for(size_t i = 0; i < imageVec.size(); ++i){
        valueSum = kernelSum = h = w = 0;

        //checks if i is not within 5 pixels of the edges
        if(i > (X_AXIS * BORDER_DISTANCE) && (i % X_AXIS) > BORDER_DISTANCE && i < ((Y_AXIS - BORDER_DISTANCE) * X_AXIS) && (i % X_AXIS) < (X_AXIS - BORDER_DISTANCE)){
            
            for (uint16_t y = imageVec[i].y - 2; y < imageVec[i].y + 3; ++y){
                for (uint16_t x = imageVec[i].x - 2; x < imageVec[i].x + 3; ++x){
                    valueSum += imageVec[y * X_AXIS + x].intensity * GKernel[h][w];
                    kernelSum += GKernel[h][w];
                    ++w;
                }
                w = 0;
                ++h;
            }
            avgValue = valueSum / kernelSum;
            imageBlurred.push_back(avgValue);
        }else{
            imageBlurred.push_back(imageVec[i].intensity);
        }
    }
    return imageBlurred;

}


//this works really poorly. 
std::vector<uint16_t> boxBlur(const std::vector<StarPixel>& imageVec){
    size_t valueSum;//sum of all surrounding pixel values
    uint16_t avgValue;//the average value of surrounding pixels
    uint16_t index;
    std::vector<uint16_t> imageBlurred;
    for(size_t i = 0; i < imageVec.size() - 2; ++i){
        //checks if i is not within 5 pixels of the edges
        valueSum = 0;
        index = 0;
 
        if(i > (X_AXIS * 10) && (i % X_AXIS) > 10 && i < ((Y_AXIS - 10) * X_AXIS) && (i % X_AXIS) < (X_AXIS - 10)){
            for (uint16_t y = imageVec[i].y - 1; y < imageVec[i].y + 2; ++y){
                for (uint16_t x = imageVec[i].x - 1; x < imageVec[i].x + 2; ++x){
                    valueSum += imageVec[y * X_AXIS + x].intensity;
                    index+=1;
                }
            }
            avgValue = valueSum / index;
            imageBlurred.push_back(avgValue);
        }else{
            imageBlurred.push_back(imageVec[i].intensity);
        }
    }
    return imageBlurred;
}


std::vector<uint16_t> averageRogueValues(std::vector<uint16_t>& imageVec){
/*     const int BORDER_DISTANCE = 5;
    const int MAX_VALUE_DIFFERENCE = 7000;
    uint64_t pixelValueSum;
    uint16_t avgPixelValue;
    std::vector<uint16_t> averagedVec;
    uint16_t min;

    for(uint64_t i = 0; i < imageVec.size(); ++i){
        min = 65535;
        if(i > (X_AXIS * BORDER_DISTANCE) && (i % X_AXIS) > BORDER_DISTANCE && i < ((Y_AXIS - BORDER_DISTANCE) * X_AXIS) && (i % X_AXIS) < (X_AXIS - BORDER_DISTANCE)){
        pixelValueSum = 0;
            for(uint64_t y = (i / X_AXIS) - 1; y < (i / X_AXIS) + 2; ++y){
                for(uint64_t x = (i % X_AXIS) - 1; x < (i % X_AXIS) + 2; ++x){
                    if( (y * X_AXIS + x) != i ){
                        //pixelValueSum += imageVec[y * X_AXIS + x];
                        if(imageVec[y*X_AXIS + x] < min){
                            min = imageVec[y*X_AXIS + x];
                        }
                    }
                }
            }
            avgPixelValue = pixelValueSum / 8;
            if(imageVec[i] - min > MAX_VALUE_DIFFERENCE){
                std::cout << imageVec[i] << " " << min << std::endl;
                imageVec[i] = min;
            }else{
                //averagedVec.push_back(imageVec[i]);
            }
        }else{
            //averagedVec.push_back(imageVec[i]);
        }
    } */
    return imageVec;
}


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

    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to intitialize TTF. Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    return;
}


//x_axis x coordinate of center
//y_axis y coordinate of center
//width width of circle in pixels
void drawCircle(int x_axis, int y_axis, int radius){
    uint32_t x,y;
    uint32_t avgX, avgY = 0;
    uint32_t totalPoints = 0;
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    for(int i = 0; i < 360; ++i){
        x = x_axis + radius * cos(i * M_PI / 180);
        y = y_axis + radius * sin(i * M_PI / 180);
        SDL_RenderDrawPoint(render, x, y);
        SDL_RenderDrawPoint(render, x + 1,y);
        SDL_RenderDrawPoint(render, x,y+1);
    }   
}

void circleStars(std::vector<Star> stars){
    for (Star i : stars){
        drawCircle(i.centerPixel.avgX, i.centerPixel.avgY, 10);
    } 

}

void createPixelInfoRect(TTF_Font* font, SDL_Color color, int x, int y) { 
    SDL_Rect rect;
    rect.x = x + 20;
    rect.y = y;
    rect.w = 100; // Rectangle width
    rect.h =25;  // Rectangle height

    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    std::string pixelInfo = "X: " + std::to_string(x) + " Y: " + std::to_string(y);
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, pixelInfo.c_str(), color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(render, surfaceMessage);
    SDL_RenderCopy(render, Message, nullptr, &rect);
}

//creates the SDL texture to render it
void SDLTexture(std::vector<uint16_t> &imageVec, std::vector<Star> stars, SDL_Color textColor){
    TTF_Font* Sans = TTF_OpenFont("Pixellettersfull-BnJ5.ttf", 12);

    std::vector<SDL_Color> colorVec;
    SDL_Color color;
    for(int y = 0; y < Y_AXIS; ++y){    
        for (int x = 0; x < X_AXIS; ++x){
            color.b = color.g = color.r = imageVec[y * X_AXIS + x];
            color.a = 255;
            colorVec.push_back(color);
        }
    }
        
    SDL_Surface* imageSurface = SDL_CreateRGBSurfaceWithFormatFrom(&colorVec[0], X_AXIS, Y_AXIS, 32, X_AXIS * 4, SDL_PIXELFORMAT_ABGR8888);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(render, imageSurface);
    

    //update screen;
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
            quit = true;
            }else if(event.type == SDL_MOUSEMOTION){
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

                SDL_RenderClear(render);

                SDL_RenderCopy(render, imageTexture, nullptr, nullptr);

                circleStars(stars);

                createPixelInfoRect(Sans, textColor, mouseX, mouseY);

                SDL_RenderPresent(render);

            }
        }
    }
}


void destroySDL(){
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
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
    file->seekg(HEADER_SIZE * 5, std::ios::beg);

    
    std::vector<uint16_t> imageVec;
    uint16_t pixel;
    while(file->read(reinterpret_cast<char*>(&pixel), sizeof(uint16_t))){
        imageVec.push_back(pixel);
    }

    return imageVec;
}

/* std::vector<uint16_t> getImage(){
    fitsfile* file;
    int status = 0;
    fits_open_file(&file, FILENAME, READONLY, &status);

    int bitpix;
    int naxis;
    long int naxes[2]; //naxes[0] = x_axis, naxes[1] = y_axis. naxes[2] somehow stores some big number, even though it's out of range and the size of the array is 16bits after running the function
    int maxdim = 2;

    
    fits_get_img_param(file, maxdim, &bitpix, &naxis, naxes, &status);
    long int x_axis = naxes[0], y_axis = naxes[1];


    std::vector<uint16_t> fits_data(x_axis * y_axis);
    fits_read_img(file, bitpix, 1, x_axis * y_axis, nullptr, fits_data.data(), nullptr, &status);

    //fits file close
    fits_close_file(file, &status);

    return fits_data;
} */


std::vector<uint16_t> minMaxValues(std::vector<uint16_t> imageVec){
    

    return imageVec;
}


int main(){
    SDL_Color green;
    green.r = green.b = 0;
    green.g = green.a = 255;
    uint64_t t = time(0);

    std::ifstream* file = createFileStream();//Stores File Stream in var

    char* header = getHeader(file);//Stores Header

    std::vector<uint16_t> imageVec = getImage(file);//stores binary data

    std::vector<StarPixel> starPixelVec = UintConvertToStarPixelFormat(imageVec);
    std::vector<uint16_t> blurredImage = gaussianBlur(starPixelVec);
    std::vector<Star> stars;
    addToStarClassVector(starPixelVec, stars);
    std::vector<Star>::iterator i = stars.begin();
    
    while(i != stars.end()){
        if(i->getSize() < 15){
            stars.erase(i);
            i = stars.begin();
        }
        ++i;
    }

    std::cout<<stars[0].centerPixel.avgX << " " << stars[0].centerPixel.avgY << std::endl;

    /* for(Star i : stars){
        std::cout << i.getSize() << std::endl;
    }
 */
    std::cout << "Number of stars detected: " << stars.size() << std::endl;



    initializeSDL();
    
    SDLTexture(imageVec, stars, green);

    std::cout << "Time to render: " << time(0) - t << std::endl;

    destroySDL();






    delete[] header;
    delete file;
    
    return 0;
}