#ifndef _TEXTURERECT_H
#define _TEXTURERECT_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class TextureRect {
    public:
    TextureRect();
    TextureRect(const string& path);
    ~TextureRect();
    const string& getFilename() const;
    const Vector2u& getSize() const;
    const Color& getPixel(int x, int y) const;
    const Color& getPixel(float u, float v) const;
    void loadBMPFile(const string& path);
    
    private:
    string _filename;
    Vector2u _pixelArraySize;
    Color** _pixelArray;
};

#endif