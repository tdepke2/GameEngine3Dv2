#ifndef _PIXELMAP_H
#define _PIXELMAP_H

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

// Based on TileMap code from https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
class PixelMap : public Drawable, public Transformable {
    public:
    virtual ~PixelMap();
    void init(const Vector2u& pixelSize, const Vector2u& regionSize);
    void setPixel(int x, int y, Color c);
    void setPixelWithCheck(int x, int y, Color c);
    const Vector2u& getSize() const;
    void clear(Color c = Color::Black);
    
    private:
    VertexArray _vertices;
    Vector2u _size;
    
    virtual void draw(RenderTarget& target, RenderStates states) const;
};

#endif