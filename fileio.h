#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <vector>

#define HEADER_SIZE 2880
#define FILENAME "dss_search"

/// @brief Loads image data stream into file.
/// @return Pointer to file object with binary format.
std::ifstream* createFileStream();

/// @brief Gathers the header data.
/// @param file File stream
/// @return Header char
char* getHeader(std::ifstream* file);

/// @brief Reads binary data from file stream into unsigned 16bit integer vector.
/// @param file File stream
/// @return Unsigned 16bit integer vector containing image data.
std::vector<uint16_t> getImage(std::ifstream* file);

#endif