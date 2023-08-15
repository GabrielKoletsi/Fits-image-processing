#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FILENAME "dss_search"
#define HEADER_SIZE 2880
#define X_AXIS 891
#define Y_AXIS 893
#define THRESHOLD 80
#define MINIMUM_DISTINCTION_DISTANCE 30 //minimum distance of two star center coordinates to be considered different stars  

SDL_Window* window = nullptr;
SDL_Renderer* render = nullptr;


//contains the data of a pixel of a star
class StarPixel{
    public:
        StarPixel(){
            x = y = 0;
            pixelValue = {0, 0, 0, 0};
        }

        StarPixel(int px, int py, SDL_Color pixel){
            x = px;
            y = py;
            pixelValue = pixel;
        }

        StarPixel(int px, int py, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
            x = px;
            y = py;
            pixelValue.r = r;
            pixelValue.g = g;
            pixelValue.b = b;
            pixelValue.a = a;
        }

        int x;
        int y;
        SDL_Color pixelValue;
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
            std::cout << i.x << " " << i.y << " " << i.pixelValue.b << std::endl;
        }
    }

    private:
        std::vector<StarPixel> starBlob;
        size_t starSize;//stores size of starBlob vector
        uint64_t sumX;//used to measure the average value of x
        uint64_t sumY;//used to measure the average value of y
        
};

//returns a vectors of StarPixel Format
std::vector<StarPixel> VectorSDL_ColorToStarPixelFormat(std::vector<SDL_Color> colorVec){
    std::vector<StarPixel> stars;
    StarPixel Spixel;
    for (size_t i = 0; i < colorVec.size(); ++i){
        Spixel.pixelValue = colorVec[i];
        Spixel.x = i % X_AXIS;
        Spixel.y = i / X_AXIS;
        stars.push_back(Spixel);
    }
    return stars;
}



//checks for neighboring pixels above threshold
//if one is found under threshold then returns false
//else returns true
bool checkPixelSurroundings(int x_, int y_, std::vector<SDL_Color>& colorVec){
    //compares the 8 pixels surrounding the pixel examined for being above threshold
    if (y_ < 5 || y_ > (Y_AXIS - 5) || x_ < 5 || x_ > (X_AXIS - 5)){
        return false;
    }
    for (uint16_t y = y_ - 3; y < y_ + 4; ++y){
        for (uint16_t x = x_ - 3; x < x_ + 4; ++x){
            if (colorVec[y * X_AXIS + x].b < THRESHOLD){
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
int32_t returnStarIndex(int x, int y, std::vector<Star> &stars){
    //if the stars vector is empty, return -1
    if(stars.size() == 0){return -1;}
    uint16_t distance;
    uint16_t diffXsq;//the difference of the x coordinates squared
    uint16_t diffYsq;//the difference of the y coordinates squared 
    for (uint32_t i = 0; i < stars.size(); ++i){
        diffXsq = (x - stars[i].centerPixel.avgX) * (x - stars[i].centerPixel.avgX);
        diffYsq = (y - stars[i].centerPixel.avgY) * (y - stars[i].centerPixel.avgY);
        distance = sqrt(diffXsq + diffYsq);
        if (distance < MINIMUM_DISTINCTION_DISTANCE){
            return i;
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

std::vector<SDL_Color> gaussianBlur(const std::vector<SDL_Color>& colorVec){
    const uint16_t BORDER_DISTANCE = 5;
    size_t valueSum;//sum of all surrounding pixel values
    uint8_t avgValue;//the average value of surrounding pixels using kernel
    double kernelSum;//the sum of all the kernel values
    std::vector<std::vector<double>> GKernel = gaussianKernel();
    std::vector<SDL_Color> imageBlurred;
    SDL_Color color;
    uint16_t h, w;//indexes to keep track of kernel iterations

    for(size_t i = 0; i < colorVec.size(); ++i){
        valueSum = kernelSum = h = w = 0;

        //checks if i is not within 5 pixels of the edges
        if(i > (X_AXIS * BORDER_DISTANCE) && (i % X_AXIS) > BORDER_DISTANCE && i < ((Y_AXIS - BORDER_DISTANCE) * X_AXIS) && (i % X_AXIS) < (X_AXIS - BORDER_DISTANCE)){
            
            for (uint16_t y = (i / X_AXIS) - 2; y < (i / X_AXIS) + 3; ++y){
                for (uint16_t x = (i % X_AXIS) - 2; x < (i % X_AXIS) + 3; ++x){
                    valueSum += colorVec[y * X_AXIS + x].b * GKernel[h][w];
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


 //this works really poorly. 
/* std::vector<uint16_t> boxBlur(const std::vector<StarPixel>& imageVec){
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
}  */


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

void createPixelInfoRect(TTF_Font* font, SDL_Color color, int x, int y, std::vector<SDL_Color>& colorVec) { 
    SDL_Rect rect1;
    SDL_Rect rect2;
    if(X_AXIS - x < 120){
        rect1.x = rect2.x = x - 120;
    }else{
        rect1.x = rect2.x = x + 20;
    }
    if(Y_AXIS - y < 44){
        rect1.y = Y_AXIS - 44;
        rect2.y = Y_AXIS - 44;
    }else{
        rect1.y = y;
        rect2.y = y + 25;
    }
    rect1.w = 100; // Rectangle width
    rect1.h = rect2.h = 25;  // Rectangle height

    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    std::string intensity = std::to_string(colorVec[y * X_AXIS + x].b);
    std::string pixelInfo = "X: " + std::to_string(x) + " Y: " + std::to_string(y);
    
    rect2.w = intensity.size() * 10;

    SDL_Surface* intensitySurface = TTF_RenderText_Solid(font, intensity.c_str(), color);
    SDL_Texture* intensityMessage = SDL_CreateTextureFromSurface(render, intensitySurface);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, pixelInfo.c_str(), color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(render, surfaceMessage);
    SDL_RenderCopy(render, Message, nullptr, &rect1);
    SDL_RenderCopy(render, intensityMessage, nullptr, &rect2);
}

std::vector<SDL_Color> convertToColor(std::vector<uint16_t>& imageVec){
    std::vector<SDL_Color> colorVec;
    SDL_Color color;
    for(int y = 0; y < Y_AXIS; ++y){    
        for (int x = 0; x < X_AXIS; ++x){
            color.b = color.g = color.r = (uint8_t)imageVec[y * X_AXIS + x];
            color.a = 255;
            colorVec.push_back(color);
        }
    }
    return colorVec;
}

//creates the SDL texture to render it
void SDLTexture(std::vector<SDL_Color> &colorVec, std::vector<Star> stars, SDL_Color textColor){
    TTF_Font* Sans = TTF_OpenFont("Pixellettersfull-BnJ5.ttf", 12);
        
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

                createPixelInfoRect(Sans, textColor, mouseX, mouseY,colorVec);

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



std::vector<uint16_t> minMaxValues(std::vector<uint16_t> imageVec){
    

    return imageVec;
}


int main(){
    SDL_Color green;
    green.r = green.b = 0;
    green.g = green.a = 255;

    std::ifstream* file = createFileStream();//Stores File Stream in var

    char* header = getHeader(file);//Stores Header

    std::vector<uint16_t> imageVec = getImage(file);//stores binary data
    std::vector<SDL_Color> colorVec = convertToColor(imageVec);

    std::vector<StarPixel> starPixelVec = VectorSDL_ColorToStarPixelFormat(colorVec);
    std::vector<SDL_Color> blurredImage = gaussianBlur(colorVec);
    std::vector<Star> stars;
    addToStarClassVector(colorVec, stars);
    std::vector<Star>::iterator i = stars.begin();
    
/*     while(i != stars.end()){
        if(i->getSize() < 15){
            stars.erase(i);
            i = stars.begin();
        }
        ++i;
    } */

    std::cout << "Number of stars detected: " << stars.size() << std::endl;


    initializeSDL();

    SDLTexture(colorVec, stars, green);

    destroySDL();


    delete[] header;
    delete file;
    
    return 0;
}
