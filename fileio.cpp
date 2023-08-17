#include "fileio.h"

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