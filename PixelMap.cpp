#include "PixelMap.h"
#include <cassert>

using namespace std;
using namespace sf;

PixelMap::~PixelMap() {}

void PixelMap::init(const Vector2u& pixelSize, const Vector2u& regionSize) {
    _size.x = (regionSize.x + pixelSize.x - 1) / pixelSize.x;
    _size.y = (regionSize.y + pixelSize.y - 1) / pixelSize.y;
    _vertices.setPrimitiveType(Quads);
    _vertices.resize(_size.x * _size.y * 4);
    
    for (unsigned int y = 0; y < _size.y; ++y) {
        for (unsigned int x = 0; x < _size.x; ++x) {
            Vertex* currentQuad = &_vertices[(y * _size.x + x) * 4];
            currentQuad[0].position = Vector2f(static_cast<float>(x * pixelSize.x), static_cast<float>(y * pixelSize.y));
            currentQuad[1].position = Vector2f((x + 1.0f) * pixelSize.x, static_cast<float>(y * pixelSize.y));
            currentQuad[2].position = Vector2f((x + 1.0f) * pixelSize.x, (y + 1.0f) * pixelSize.y);
            currentQuad[3].position = Vector2f(static_cast<float>(x * pixelSize.x), (y + 1.0f) * pixelSize.y);
            
            if ((x + y) % 2 == 0) {
                currentQuad[0].color = Color(10, 10, 10);
                currentQuad[1].color = Color(10, 10, 10);
                currentQuad[2].color = Color(10, 10, 10);
                currentQuad[3].color = Color(10, 10, 10);
            } else {
                currentQuad[0].color = Color(15, 15, 15);
                currentQuad[1].color = Color(15, 15, 15);
                currentQuad[2].color = Color(15, 15, 15);
                currentQuad[3].color = Color(15, 15, 15);
            }
        }
    }
}

void PixelMap::setPixel(int x, int y, Color c) {
    assert(x >= 0 && x < static_cast<int>(_size.x) && y >= 0 && y < static_cast<int>(_size.y));
    Vertex* currentQuad = &_vertices[(y * _size.x + x) * 4];
    currentQuad[0].color = c;
    currentQuad[1].color = c;
    currentQuad[2].color = c;
    currentQuad[3].color = c;
}

void PixelMap::setPixelWithCheck(int x, int y, Color c) {
    if (x >= 0 && x < static_cast<int>(_size.x) && y >= 0 && y < static_cast<int>(_size.y)) {
        setPixel(x, y, c);
    }
}

const Vector2u& PixelMap::getSize() const {
    return _size;
}

void PixelMap::clear(Color c) {
    for (unsigned int y = 0; y < _size.y; ++y) {
        for (unsigned int x = 0; x < _size.x; ++x) {
            Vertex* currentQuad = &_vertices[(y * _size.x + x) * 4];
            
            if ((x + y) % 2 == 0) {
                currentQuad[0].color = Color(10, 10, 10);
                currentQuad[1].color = Color(10, 10, 10);
                currentQuad[2].color = Color(10, 10, 10);
                currentQuad[3].color = Color(10, 10, 10);
            } else {
                currentQuad[0].color = Color(15, 15, 15);
                currentQuad[1].color = Color(15, 15, 15);
                currentQuad[2].color = Color(15, 15, 15);
                currentQuad[3].color = Color(15, 15, 15);
            }
        }
    }
}

void PixelMap::draw(RenderTarget& target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(_vertices, states);
}