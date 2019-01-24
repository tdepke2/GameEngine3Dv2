#ifndef _GRAPHICSENGINE_H
#define _GRAPHICSENGINE_H

#include "PixelMap.h"
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class GraphicsEngine {
    public:
    const float PI = acos(-1.0f);
    
    void redraw(PixelMap& pixelMap, const Vector3f& camPosition, const Vector3f& camRotation);
    Vector2f graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, float x0, float y0, float z0);
};

#endif