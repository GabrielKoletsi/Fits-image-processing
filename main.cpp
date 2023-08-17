#include "fileio.h"
#include "Stars.h"
#include "renderer.h"
#include "ImageFilters.h"
#include "starDetectionAlgorithm.h"


SDL_Window* window = nullptr;
SDL_Renderer* render = nullptr;


int main(){

    std::ifstream* file = createFileStream();//Stores File Stream in var

    char* header = getHeader(file);//Stores Header

    std::vector<uint16_t> imageVec = getImage(file);//stores binary data
    std::vector<SDL_Color> colorVec = convertToColor(imageVec);

    std::vector<StarPixel> starPixelVec = VectorSDL_ColorToStarPixelFormat(colorVec);
    std::vector<SDL_Color> blurredImage = GaussianBlur(colorVec, X_AXIS, 1.0, 5);
    std::vector<Star> stars;
    addToStarClassVector(blurredImage, stars);
    findClosestStar(stars);
    linearHistogram(colorVec, 1.0);
    std::cout << "Number of stars detected: " << stars.size() << std::endl;
    

    initializeSDL();

    SDLTexture(colorVec, stars);

    destroySDL();


    delete[] header;
    delete file;
    
    return 0;
}