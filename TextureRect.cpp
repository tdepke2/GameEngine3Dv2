#include "TextureRect.h"
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace sf;

TextureRect::TextureRect() {
    _pixelArray = nullptr;
}

TextureRect::TextureRect(const string& path) {
    _pixelArray = nullptr;
    loadBMPFile(path);
}

TextureRect::~TextureRect() {
    for (unsigned int y = 0; y < _pixelArraySize.y; ++y) {
        delete[] _pixelArray[y];
    }
    delete _pixelArray;
}

const string& TextureRect::getFilename() const {
    return _filename;
}

const Vector2u& TextureRect::getSize() const {
    return _pixelArraySize;
}

const Color& TextureRect::getPixel(int x, int y) const {
    return _pixelArray[y][x];
}

const Color& TextureRect::getPixel(float u, float v) const {
    return _pixelArray[static_cast<int>(v * _pixelArraySize.y + 0.5f) % _pixelArraySize.y][static_cast<int>(u * _pixelArraySize.x + 0.5f) % _pixelArraySize.x];
}

void TextureRect::loadBMPFile(const string& path) {
    // Based on sources:
    // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
    // https://en.wikipedia.org/wiki/BMP_file_format
    // https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
    
    const unsigned int HEADER_SIZE = 54;
    _filename = path;
    ifstream imageFile(path, ios::binary);
    if (!imageFile.is_open()) {
        throw runtime_error(path + ": Unable to access texture file.");
    }
    
    char* header = new char[HEADER_SIZE];
    imageFile.read(header, HEADER_SIZE);
    uint32_t imageAddress = *reinterpret_cast<uint32_t*>(&header[0x0A]);
    uint32_t width = *reinterpret_cast<uint32_t*>(&header[0x12]);
    uint32_t height = *reinterpret_cast<uint32_t*>(&header[0x16]);
    uint16_t bitDepth = *reinterpret_cast<uint16_t*>(&header[0x1C]);
    if (header[0] != 'B' || header[1] != 'M') {
        throw runtime_error(path + ": Invalid texture file format (must be bitmap).");
    }
    if (bitDepth != 24 && bitDepth != 32) {
        throw runtime_error(path + ": The bit depth of " + to_string(bitDepth) + " is not supported.");
    }
    
    if (imageAddress - HEADER_SIZE > 0) {
        char* tempData = new char[imageAddress - HEADER_SIZE];
        imageFile.read(tempData, imageAddress - HEADER_SIZE);
        delete[] tempData;
    }
    unsigned int numChannels = bitDepth / 8;
    uint32_t imageSize = ((width * numChannels + 3) & ~3) * height;    // Round each row up to a multiple of 4 bytes.
    char* imageData = new char[imageSize];
    imageFile.read(imageData, imageSize);
    
    for (unsigned int y = 0; y < _pixelArraySize.y; ++y) {    // Delete old texture if it exists.
        delete[] _pixelArray[y];
    }
    delete _pixelArray;
    
    _pixelArray = new Color*[height];
    unsigned int index = 0;
    for (unsigned int y = 0; y < height; ++y) {
        _pixelArray[y] = new Color[width];
        for (unsigned int x = 0; x < width; ++x) {
            if (numChannels == 3) {
                _pixelArray[y][x] = Color(imageData[index + 2], imageData[index + 1], imageData[index]);
            } else {
                _pixelArray[y][x] = Color(imageData[index + 2], imageData[index + 1], imageData[index], imageData[index + 3]);
            }
            index += numChannels;
        }
        index = (index + 3) & ~3;
    }
    _pixelArraySize.x = width;
    _pixelArraySize.y = height;
    
    imageFile.close();
    delete[] header;
    delete[] imageData;
}