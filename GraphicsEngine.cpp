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
            if (ptrScreenA->tooNear || ptrScreenB->tooNear || ptrScreenC->tooNear || ptrScreenA->tooFar || ptrScreenB->tooFar || ptrScreenC->tooFar) {    // #################################################
                continue;
            }
            
            // apply back-face culling.
            
            /*if (ptrScreenA->tooFar && !ptrScreenB->tooFar || ptrScreenB->tooNear && !ptrScreenA->tooNear) {    // Sort the points near to far.
                swap(ptrPointA, ptrPointB);
                swap(ptrScreenA, ptrScreenB);
            }
            if (ptrScreenB->tooFar && !ptrScreenC->tooFar || ptrScreenC->tooNear && !ptrScreenB->tooNear) {
                swap(ptrPointB, ptrPointC);
                swap(ptrScreenB, ptrScreenC);
            }
            if (ptrScreenA->tooFar && !ptrScreenB->tooFar || ptrScreenB->tooNear && !ptrScreenA->tooNear) {
                swap(ptrPointA, ptrPointB);
                swap(ptrScreenA, ptrScreenB);
            }
            
            if (ptrScreenB->tooNear) {    // Check if two points are too near.
                ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -NEAR_PLANE);
                ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointB - camPosition, *ptrPointC - camPosition, -NEAR_PLANE);
                if (ptrScreenC->tooFar) {
                    ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                    ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointB - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                    fillTriangle(modelsIter->getTriangles()[i], screenD, screenE, screenF);
                    fillTriangle(modelsIter->getTriangles()[i], screenE, screenF, screenG);
                } else {
                    fillTriangle(modelsIter->getTriangles()[i], screenD, screenE, *ptrScreenC);
                }
            } else if (ptrScreenB->tooFar) {    // Check if two points are too far.
                ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointB - camPosition, -FAR_PLANE);
                ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                if (ptrScreenA->tooNear) {
                    ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointB - camPosition, -NEAR_PLANE);
                    ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -NEAR_PLANE);
                    fillTriangle(modelsIter->getTriangles()[i], screenD, screenE, screenF);
                    fillTriangle(modelsIter->getTriangles()[i], screenE, screenF, screenG);
                } else {
                    fillTriangle(modelsIter->getTriangles()[i], *ptrScreenA, screenF, screenG);
                }
            } else if (ptrScreenA->tooNear) {    // Check if one point too near.
                ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointB - camPosition, -NEAR_PLANE);
                ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -NEAR_PLANE);
                if (ptrScreenC->tooFar) {
                    ScreenPoint screenF = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointB - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                    ScreenPoint screenG = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                    fillTriangle(modelsIter->getTriangles()[i], screenD, screenE, *ptrScreenB);
                    fillTriangle(modelsIter->getTriangles()[i], screenE, *ptrScreenB, screenF);
                    fillTriangle(modelsIter->getTriangles()[i], screenE, screenF, screenG);
                } else {
                    fillTriangle(modelsIter->getTriangles()[i], screenD, screenE, *ptrScreenB);
                    fillTriangle(modelsIter->getTriangles()[i], screenE, *ptrScreenB, *ptrScreenC);
                }
            } else if (ptrScreenC->tooFar) {    // Check if one point too far.
                ScreenPoint screenD = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointA - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                ScreenPoint screenE = graphicsTransform(_pixelMap.getSize(), camRotation, *ptrPointB - camPosition, *ptrPointC - camPosition, -FAR_PLANE);
                fillTriangle(modelsIter->getTriangles()[i], *ptrScreenA, *ptrScreenB, screenD);
                fillTriangle(modelsIter->getTriangles()[i], *ptrScreenB, screenD, screenE);
            } else {    // Else, all points are between the viewing planes.*/
                // Need to fix this before adding back plane culling #########################################################################################################################################
                ShaderPoint a(ptrScreenA->x, ptrScreenA->y, ptrScreenA->z, modelsIter->getTriangles()[i].aColor, modelsIter->getTriangles()[i].aTexture.x, modelsIter->getTriangles()[i].aTexture.y);
                ShaderPoint b(ptrScreenB->x, ptrScreenB->y, ptrScreenB->z, modelsIter->getTriangles()[i].bColor, modelsIter->getTriangles()[i].bTexture.x, modelsIter->getTriangles()[i].bTexture.y);
                ShaderPoint c(ptrScreenC->x, ptrScreenC->y, ptrScreenC->z, modelsIter->getTriangles()[i].cColor, modelsIter->getTriangles()[i].cTexture.x, modelsIter->getTriangles()[i].cTexture.y);
                fillTriangle(&a, &b, &c, modelsIter->getTriangles()[i].texture);
            //}
        }
    }
    
    for (unsigned int y = 0; y < Model::textures.back().getSize().y / 2; ++y) {
        for (unsigned int x = 0; x < Model::textures.back().getSize().x / 2; ++x) {
            _pixelMap.setPixelWithCheck(x, y, Model::textures.back().getPixel(static_cast<float>(x * 2) / Model::textures.back().getSize().x, static_cast<float>(y * 2) / Model::textures.back().getSize().y));
        }
    }
}

ScreenPoint GraphicsEngine::graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, const Vector3f& point) {
    return _scalingTransform(screenSize, _rotationTransform(camRotation, point));
}

ScreenPoint GraphicsEngine::graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, Vector3f pointA, Vector3f pointB, float zTarget) {
    pointA = _rotationTransform(camRotation, pointA);
    pointB = _rotationTransform(camRotation, pointB);
    float u = (zTarget - pointA.z) / (pointB.z - pointA.z);
    return _scalingTransform(screenSize, Vector3f(pointA.x + u * (pointB.x - pointA.x), pointA.y + u * (pointB.y - pointA.y), zTarget));
}

void GraphicsEngine::fillTriangle(const ShaderPoint* a, const ShaderPoint* b, const ShaderPoint* c, const TextureRect* texture) {
    // Based on algorithm from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    if (b->y < a->y) {    // Sort points by y-coordinate ascending.
        swap(a, b);
    }
    if (c->y < b->y) {
        swap(b, c);
    }
    if (b->y < a->y) {
        swap(a, b);
    }
    
    if (b->y == c->y) {
        _fillTriangleFlatBottom(*a, *b, *c, texture);
    } else if (a->y == b->y) {
        _fillTriangleFlatTop(*a, *b, *c, texture);
    } else {
        ShaderPoint d;    // Need more calculations in here ######################################################################
        float newX = a->x + static_cast<float>(a->x - c->x) / (a->y - c->y) * (b->y - a->y);
        d.x = static_cast<float>(newX);    // This may need rounding instead of direct cast.
        d.y = b->y;
        d.xTex = a->xTex + (c->xTex - a->xTex) * static_cast<float>(newX - a->x) / (c->x - a->x);
        d.yTex = a->yTex + (c->yTex - a->yTex) * static_cast<float>(d.y - a->y) / (c->y - a->y);
        _fillTriangleFlatBottom(*a, *b, d, texture);
        _fillTriangleFlatTop(*b, d, *c, texture);
    }
}

Vector3f GraphicsEngine::_rotationTransform(const Vector3f& camRotation, Vector3f point) {
    Vector3f point2;
    point2.x = point.x * cos(camRotation.y) + point.z * sin(camRotation.y);
    point2.z = -point.x * sin(camRotation.y) + point.z * cos(camRotation.y);
    
    point.z = point2.z * cos(camRotation.x) + point.y * sin(camRotation.x);
    point2.y = -point2.z * sin(camRotation.x) + point.y * cos(camRotation.x);
    
    point.y = point2.y * cos(camRotation.z) + point2.x * sin(camRotation.z);
    point.x = -point2.y * sin(camRotation.z) + point2.x * cos(camRotation.z);
    
    return point;
}

ScreenPoint GraphicsEngine::_scalingTransform(const Vector2u& screenSize, Vector3f point) {
    float aspectRatio = static_cast<float>(screenSize.x) / screenSize.y;
    if (aspectRatio < 1.0f) {
        point.x /= aspectRatio;
    } else {
        point.y *= aspectRatio;
    }
    
    bool tooNear = -point.z < NEAR_PLANE, tooFar = -point.z > FAR_PLANE;
    unsigned int zAdjusted;
    double zAdjustedDouble = static_cast<double>(-point.z - NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE) * numeric_limits<unsigned int>::max() + 0.5;
    if (zAdjustedDouble < 0.0) {
        zAdjusted = 0;
    } else if (zAdjustedDouble > static_cast<double>(numeric_limits<unsigned int>::max())) {
        zAdjusted = numeric_limits<unsigned int>::max();
    } else {
        zAdjusted = static_cast<unsigned int>(zAdjustedDouble);
    }
    
    point.z *= tan(90.0f * PI / 360.0f);
    point.x = (point.x / -point.z + 1.0f) * screenSize.x / 2.0f;
    point.y = (point.y / point.z + 1.0f) * screenSize.y / 2.0f;
    
    return ScreenPoint(static_cast<int>(point.x + 0.5f), static_cast<int>(point.y + 0.5f), zAdjusted, tooNear, tooFar);
}

void GraphicsEngine::_horizontalLine(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture, int x0, int x1, int y) {
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
        float s = static_cast<float>(x0 - c.x) / (b.x - c.x), t = static_cast<float>(y - a.y) / (b.y - a.y);
        _pixelMap.setPixel(x0, y, texture->getPixel(a.xTex + (c.xTex + (b.xTex - c.xTex) * s - a.xTex) * t, a.yTex + (c.yTex + (b.yTex - c.yTex) * s - a.yTex) * t));
        ++x0;
    }
}

void GraphicsEngine::_fillTriangleFlatBottom(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture) {    // Point a needs to be at the top.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeAB = static_cast<float>(a.x - b.x) / (a.y - b.y);
    float x0 = a.x + 0.5f, x1 = a.x + 0.5f;
    int y = a.y, yStop = (b.y < static_cast<int>(_pixelMap.getSize().y)) ? b.y : _pixelMap.getSize().y - 1;
    if (y < 0) {
        x0 += invSlopeAC * -y;
        x1 += invSlopeAB * -y;
        y = 0;
    }
    while (y <= yStop) {
        _horizontalLine(a, b, c, texture, static_cast<int>(x0), static_cast<int>(x1), y);
        x0 += invSlopeAC;
        x1 += invSlopeAB;
        ++y;
    }
}

void GraphicsEngine::_fillTriangleFlatTop(const ShaderPoint& a, const ShaderPoint& b, const ShaderPoint& c, const TextureRect* texture) {    // Point c needs to be at the bottom.
    float invSlopeAC = static_cast<float>(a.x - c.x) / (a.y - c.y), invSlopeBC = static_cast<float>(b.x - c.x) / (b.y - c.y);
    float x0 = c.x + 0.5f, x1 = c.x + 0.5f;
    int y = c.y, yStop = (a.y >= 0) ? a.y : 0;
    if (y >= static_cast<int>(_pixelMap.getSize().y)) {
        x0 -= invSlopeAC * (y - _pixelMap.getSize().y + 1);
        x1 -= invSlopeBC * (y - _pixelMap.getSize().y + 1);
        y = _pixelMap.getSize().y - 1;
    }
    while (y > yStop) {
        _horizontalLine(a, b, c, texture, static_cast<int>(x0), static_cast<int>(x1), y);
        x0 -= invSlopeAC;
        x1 -= invSlopeBC;
        --y;
    }
}