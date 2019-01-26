#include "GraphicsEngine.h"

using namespace std;
using namespace sf;

const PixelMap& GraphicsEngine::getPixelMap() const {
    return _pixelMap;
}

void GraphicsEngine::init(const Vector2u& pixelSize, const Vector2u& regionSize) {
    _pixelMap.init(pixelSize, regionSize);
}

void GraphicsEngine::redraw(Level& level, const Vector3f& camPosition, const Vector3f& camRotation) {
    _pixelMap.clear();
    Vector2i a, b, c;
    for (auto modelsIter = level.getModels().begin(); modelsIter != level.getModels().end(); ++modelsIter) {
        for (unsigned int i = 0; i < modelsIter->getPointsSize(); ++i) {
            Vector3f relativePoint = modelsIter->getPoints()[i] - camPosition;
            Vector2f screenPoint = graphicsTransform(_pixelMap.getSize(), camRotation, relativePoint.x, relativePoint.y, relativePoint.z);
            _pixelMap.setPixel(screenPoint, Color::Green);
            
            if (i == 0) {
                a.x = screenPoint.x + 0.5f;
                a.y = screenPoint.y + 0.5f;
            } else if (i == 1) {
                b.x = screenPoint.x + 0.5f;
                b.y = screenPoint.y + 0.5f;
            } else if (i == 2) {
                c.x = screenPoint.x + 0.5f;
                c.y = screenPoint.y + 0.5f;
            }
        }
    }
    _fillTriangle(a, b, c);
}

Vector2f GraphicsEngine::graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, float x0, float y0, float z0) {
    float x1 = x0 * cos(camRotation.y) + z0 * sin(camRotation.y);
    float z1 = -x0 * sin(camRotation.y) + z0 * cos(camRotation.y);
    
    z0 = z1 * cos(camRotation.x) + y0 * sin(camRotation.x);
    /*if (z0 >= 0.0f) {
        return Vector2f(-5.0f, -5.0f);
    }*/
    float y1 = -z1 * sin(camRotation.x) + y0 * cos(camRotation.x);
    
    y0 = y1 * cos(camRotation.z) + x1 * sin(camRotation.z);
    x0 = -y1 * sin(camRotation.z) + x1 * cos(camRotation.z);
    
    float aspectRatio = static_cast<float>(screenSize.x) / screenSize.y;
    if (aspectRatio < 1.0f) {
        x0 /= aspectRatio;
    } else {
        y0 *= aspectRatio;
    }
    z0 *= tan(90.0f * PI / 360.0f);
    
    x1 = (x0 / -z0 + 1.0f) * screenSize.x / 2.0f;
    y1 = (y0 / z0 + 1.0f) * screenSize.y / 2.0f;
    
    return Vector2f(x1, y1);
}

void GraphicsEngine::_horizontalLine(int x0, int x1, int y) {
    if (x0 > x1) {
        int temp = x0;
        x0 = x1;
        x1 = temp;
    }
    while (x0 <= x1) {
        _pixelMap.setPixel(Vector2f(x0, y), Color::White);    // plz to optimize thx
        ++x0;
    }
}

void GraphicsEngine::_fillTriangleFlatBottom(const Vector2i& a, const Vector2i& b, const Vector2i& c) {    // Point a needs to be at the top.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeAB = static_cast<float>(a.x - b.x) / (a.y - b.y);
    float xLeft = a.x + 0.5f, xRight = a.x + 0.5f;
    for (int y = a.y; y <= b.y; ++y) {
        _horizontalLine(static_cast<int>(xLeft), static_cast<int>(xRight), y);
        xLeft += invSlopeAC;
        xRight += invSlopeAB;
    }
}

void GraphicsEngine::_fillTriangleFlatTop(const Vector2i& a, const Vector2i& b, const Vector2i& c) {    // Point c needs to be at the bottom.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeBC = static_cast<float>(b.x - c.x) / (b.y - c.y);
    float xLeft = c.x + 0.5f, xRight = c.x + 0.5f;
    for (int y = c.y; y > a.y; --y) {
        _horizontalLine(static_cast<int>(xLeft), static_cast<int>(xRight), y);
        xLeft -= invSlopeAC;
        xRight -= invSlopeBC;
    }
}

void GraphicsEngine::_fillTriangle(Vector2i a, Vector2i b, Vector2i c) {
    // Based on algorithm from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    if (b.y < a.y) {    // Sort points by y-coordinate ascending.
        Vector2i temp = b;
        b = a;
        a = temp;
    }
    if (c.y < b.y) {
        Vector2i temp = c;
        c = b;
        b = temp;
    }
    if (b.y < a.y) {
        Vector2i temp = b;
        b = a;
        a = temp;
    }
    
    if (b.y == c.y) {
        _fillTriangleFlatBottom(a, b, c);
    } else if (a.y == b.y) {
        _fillTriangleFlatTop(a, b, c);
    } else {
        Vector2i d(static_cast<int>(a.x + static_cast<float>(a.x - c.x) / (a.y - c.y) * (b.y - a.y)), b.y);
        _fillTriangleFlatBottom(a, b, d);
        _fillTriangleFlatTop(b, d, c);
    }
}