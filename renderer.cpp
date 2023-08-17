#include "renderer.h"



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


void drawCircle(int x_axis, int y_axis, int radius){
    uint32_t x,y;
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    for(int i = 0; i < 360; ++i){
        x = x_axis + radius * cos(i * M_PI / 180);
        y = y_axis + radius * sin(i * M_PI / 180);
        SDL_RenderDrawPoint(render, x, y);
        SDL_RenderDrawPoint(render, x + 1,y);
        SDL_RenderDrawPoint(render, x,y+1);
    }   
}



void printText(int x_, int y_, std::string text, TTF_Font* font){
    SDL_Rect rect;
    rect.x = x_ - 5;
    rect.y = y_ - 25;
    rect.h = 25;
    rect.w = text.size() * 10;

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), green);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(render, textSurface);
    SDL_RenderCopy(render, textTexture, nullptr, &rect);
}


void circleStars(std::vector<Star> stars, TTF_Font* font){
    for (int i = 0; i < (int) stars.size(); ++i){
        
        drawCircle(stars[i].avgX, stars[i].avgY, 10);
        SDL_RenderDrawLine(render, stars[i].avgX, stars[i].avgY, stars[i].getClosestStar()->avgX, stars[i].getClosestStar()->avgY);
        printText(stars[i].avgX, stars[i].avgY, std::to_string(i), font);
    } 
}


void createPixelInfoRect(int x, int y, std::vector<SDL_Color>& colorVec, TTF_Font* font) { 
    SDL_Rect rect1;
    SDL_Rect rect2;
    if(X_AXIS - x < 120){
        rect1.x = rect2.x = x - 120;
    }else{
        rect1.x = rect2.x = x + 20;
    }
    if(Y_AXIS - y < 43){
        rect1.y = Y_AXIS - 43;
        rect2.y = Y_AXIS - 19;
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

    SDL_Surface* intensitySurface = TTF_RenderText_Solid(font, intensity.c_str(), green);
    SDL_Texture* intensityMessage = SDL_CreateTextureFromSurface(render, intensitySurface);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, pixelInfo.c_str(), green);
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


void linearHistogram(std::vector<SDL_Color>& colorVec, double F){
    if(F == 1){
        SDL_Color color;
        color.a = 255;
        for(uint64_t i = 0; i < colorVec.size(); ++i){
            if(colorVec[i].b * F > 0xFF){
                colorVec[i].b = colorVec[i].g = colorVec[i].r = 0xFF;
            }else{
                color.b = color.g = color.r = (uint8_t)colorVec[i].b * F;
                colorVec[i] = color;
            }
        }
    }
}


void SDLTexture(std::vector<SDL_Color> &colorVec, std::vector<Star> stars){
    TTF_Font* font = TTF_OpenFont("fonts/Pixellettersfull-BnJ5.ttf", 12);

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

                circleStars(stars, font);

                createPixelInfoRect(mouseX, mouseY,colorVec, font);

                SDL_RenderPresent(render);

            }
        }
    }

    SDL_FreeSurface(imageSurface);
}


void destroySDL(){
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}


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