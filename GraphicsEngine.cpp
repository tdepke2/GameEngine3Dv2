#include "GraphicsEngine.h"
#include <limits>
#include <utility>

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
    _screenPoints.clear();
    for (auto modelsIter = level.getModels().begin(); modelsIter != level.getModels().end(); ++modelsIter) {
        for (unsigned int i = 0; i < modelsIter->getPointsSize(); ++i) {
            _screenPoints.push_back(graphicsTransform(_pixelMap.getSize(), camRotation, modelsIter->getPoints()[i] - camPosition));
            if (!_screenPoints.back().tooNear && !_screenPoints.back().tooFar) {
                _pixelMap.setPixelWithCheck(_screenPoints.back().x, _screenPoints.back().y, Color::Green);
            }
        }
        
        for (unsigned int i = 0; i < modelsIter->getTrianglesSize(); ++i) {
            const Vector3f* ptrPointA = &modelsIter->getPoints()[modelsIter->getTriangles()[i].a];
            const Vector3f* ptrPointB = &modelsIter->getPoints()[modelsIter->getTriangles()[i].b];
            const Vector3f* ptrPointC = &modelsIter->getPoints()[modelsIter->getTriangles()[i].c];
            const ScreenPoint* ptrScreenA = &_screenPoints[modelsIter->getTriangles()[i].a];
            const ScreenPoint* ptrScreenB = &_screenPoints[modelsIter->getTriangles()[i].b];
            const ScreenPoint* ptrScreenC = &_screenPoints[modelsIter->getTriangles()[i].c];
            if (ptrScreenA->tooNear && ptrScreenB->tooNear && ptrScreenC->tooNear || ptrScreenA->tooFar && ptrScreenB->tooFar && ptrScreenC->tooFar) {
                continue;
            }
            
            // apply back-face culling.
            
            if (ptrScreenA->tooFar && !ptrScreenB->tooFar || ptrScreenB->tooNear && !ptrScreenA->tooNear) {    // Sort the points near to far.
                swap(ptrScreenA, ptrScreenB);
            }
            if (ptrScreenB->tooFar && !ptrScreenC->tooFar || ptrScreenC->tooNear && !ptrScreenB->tooNear) {
                swap(ptrScreenB, ptrScreenC);
            }
            if (ptrScreenA->tooFar && !ptrScreenB->tooFar || ptrScreenB->tooNear && !ptrScreenA->tooNear) {
                swap(ptrScreenA, ptrScreenB);
            }
            
            if (ptrScreenB->tooNear) {    // Check if two points are too near.
                if (ptrScreenC->tooFar) {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointB, *ptrPointC, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -FAR_PLANE) - camPosition);
                    ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointB, *ptrPointC, -FAR_PLANE) - camPosition);
                    fillTriangle(screenD, screenE, screenF);
                    fillTriangle(screenE, screenF, screenG);
                } else {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointB, *ptrPointC, -NEAR_PLANE) - camPosition);
                    fillTriangle(screenD, screenE, *ptrScreenC);
                }
            } else if (ptrScreenB->tooFar) {    // Check if two points are too far.
                if (ptrScreenA->tooNear) {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointB, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointB, -FAR_PLANE) - camPosition);
                    ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -FAR_PLANE) - camPosition);
                    fillTriangle(screenD, screenE, screenF);
                    fillTriangle(screenE, screenF, screenG);
                } else {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointB, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    fillTriangle(*ptrScreenA, screenD, screenE);
                }
            } else if (ptrScreenA->tooNear) {    // Check if one point too near.
                if (ptrScreenC->tooFar) {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointB, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointB, *ptrPointC, -FAR_PLANE) - camPosition);
                    ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -FAR_PLANE) - camPosition);
                    fillTriangle(screenD, screenE, *ptrScreenB);
                    fillTriangle(screenE, *ptrScreenB, screenF);
                    fillTriangle(screenE, screenF, screenG);
                } else {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointB, -NEAR_PLANE) - camPosition);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                    fillTriangle(screenD, screenE, *ptrScreenB);
                    fillTriangle(screenE, *ptrScreenB, *ptrScreenC);
                }
            } else if (ptrScreenC->tooFar) {    // Check if one point too far.
                ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointA, *ptrPointC, -NEAR_PLANE) - camPosition);
                ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, _findZIntersect(*ptrPointB, *ptrPointC, -NEAR_PLANE) - camPosition);
                fillTriangle(*ptrScreenA, *ptrScreenB, screenD);
                fillTriangle(*ptrScreenB, screenD, screenE);
            } else {    // Else, all points are between the viewing planes.
                fillTriangle(*ptrScreenA, *ptrScreenB, *ptrScreenC);
            }
        }
    }
}

ScreenPoint GraphicsEngine::graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, const Vector3f& point) {
    float x0 = point.x, y0 = point.y, z0 = point.z;
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
    
    bool tooNear = -z0 < NEAR_PLANE, tooFar = -z0 > FAR_PLANE;
    unsigned int zAdjusted;
    double zAdjustedDouble = static_cast<double>(-z0 - NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE) * numeric_limits<unsigned int>::max() + 0.5;
    if (zAdjustedDouble < 0.0) {
        zAdjusted = 0;
    } else if (zAdjustedDouble > static_cast<double>(numeric_limits<unsigned int>::max())) {
        zAdjusted = numeric_limits<unsigned int>::max();
    } else {
        zAdjusted = static_cast<unsigned int>(zAdjustedDouble);
    }
    
    z0 *= tan(90.0f * PI / 360.0f);
    x1 = (x0 / -z0 + 1.0f) * screenSize.x / 2.0f;
    y1 = (y0 / z0 + 1.0f) * screenSize.y / 2.0f;
    
    return ScreenPoint(static_cast<int>(x1 + 0.5f), static_cast<int>(y1 + 0.5f), zAdjusted, tooNear, tooFar);
}

void GraphicsEngine::fillTriangle(const ScreenPoint& pointA, const ScreenPoint& pointB, const ScreenPoint& pointC) {
    // Based on algorithm from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    Vector2i a(pointA.x, pointA.y), b(pointB.x, pointB.y), c(pointC.x, pointC.y);
    if (b.y < a.y) {    // Sort points by y-coordinate ascending.
        swap(a, b);
    }
    if (c.y < b.y) {
        swap(b, c);
    }
    if (b.y < a.y) {
        swap(a, b);
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

void GraphicsEngine::_horizontalLine(int x0, int x1, int y) {
    if (x0 > x1) {
        swap(x0, x1);
    }
    if (x0 < 0) {
        x0 = 0;
    }
    if (x1 >= static_cast<int>(_pixelMap.getSize().x)) {
        x1 = _pixelMap.getSize().x - 1;
    }
    while (x0 <= x1) {
        _pixelMap.setPixel(x0, y, Color::White);
        ++x0;
    }
}

void GraphicsEngine::_fillTriangleFlatBottom(const Vector2i& a, const Vector2i& b, const Vector2i& c) {    // Point a needs to be at the top.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeAB = static_cast<float>(a.x - b.x) / (a.y - b.y);
    float x0 = a.x + 0.5f, x1 = a.x + 0.5f;
    int y = a.y, yStop = (b.y < static_cast<int>(_pixelMap.getSize().y)) ? b.y : _pixelMap.getSize().y - 1;
    if (y < 0) {
        x0 += invSlopeAC * -y;
        x1 += invSlopeAB * -y;
        y = 0;
    }
    while (y <= yStop) {
        _horizontalLine(static_cast<int>(x0), static_cast<int>(x1), y);
        x0 += invSlopeAC;
        x1 += invSlopeAB;
        ++y;
    }
}

void GraphicsEngine::_fillTriangleFlatTop(const Vector2i& a, const Vector2i& b, const Vector2i& c) {    // Point c needs to be at the bottom.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeBC = static_cast<float>(b.x - c.x) / (b.y - c.y);
    float x0 = c.x + 0.5f, x1 = c.x + 0.5f;
    int y = c.y, yStop = (a.y >= 0) ? a.y : 0;
    if (y >= static_cast<int>(_pixelMap.getSize().y)) {
        x0 -= invSlopeAC * (y - _pixelMap.getSize().y + 1);
        x1 -= invSlopeBC * (y - _pixelMap.getSize().y + 1);
        y = _pixelMap.getSize().y - 1;
    }
    while (y > yStop) {
        _horizontalLine(static_cast<int>(x0), static_cast<int>(x1), y);
        x0 -= invSlopeAC;
        x1 -= invSlopeBC;
        --y;
    }
}

Vector3f GraphicsEngine::_findZIntersect(const Vector3f& a, const Vector3f& b, float z) {
    float u = (z - a.z) / (b.z - a.z);
    return Vector3f(a.x + u * (b.x - a.x), a.y + u * (b.y - a.y), z);
}