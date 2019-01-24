#include "GraphicsEngine.h"

using namespace std;
using namespace sf;

void GraphicsEngine::redraw(PixelMap& pixelMap, const Vector3f& camPosition, const Vector3f& camRotation) {
    Vector3f point1(0.0f - camPosition.x, 0.0f - camPosition.y, 0.0f - camPosition.z);
    Vector3f point2(1.0f - camPosition.x, 0.0f - camPosition.y, 0.0f - camPosition.z);
    Vector3f point3(1.0f - camPosition.x, 0.0f - camPosition.y, 1.0f - camPosition.z);
    Vector3f point4(0.0f - camPosition.x, 0.0f - camPosition.y, 1.0f - camPosition.z);
    Vector3f point5(0.0f - camPosition.x, 1.0f - camPosition.y, 0.0f - camPosition.z);
    Vector3f point6(1.0f - camPosition.x, 1.0f - camPosition.y, 0.0f - camPosition.z);
    Vector3f point7(1.0f - camPosition.x, 1.0f - camPosition.y, 1.0f - camPosition.z);
    Vector3f point8(0.0f - camPosition.x, 1.0f - camPosition.y, 1.0f - camPosition.z);
    
    pixelMap.clear();
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point1.x, point1.y, point1.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point2.x, point2.y, point2.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point3.x, point3.y, point3.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point4.x, point4.y, point4.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point5.x, point5.y, point5.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point6.x, point6.y, point6.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point7.x, point7.y, point7.z), Color::Green);
    pixelMap.setPixel(graphicsTransform(pixelMap.getSize(), camRotation, point8.x, point8.y, point8.z), Color::Green);
}

Vector2f GraphicsEngine::graphicsTransform(const Vector2u& screenSize, const Vector3f& camRotation, float x0, float y0, float z0) {
    float x1 = x0 * cos(camRotation.y) + z0 * sin(camRotation.y);
    float z1 = -x0 * sin(camRotation.y) + z0 * cos(camRotation.y);
    
    z0 = z1 * cos(camRotation.x) + y0 * sin(camRotation.x);
    if (z0 > 0.0f) {
        return Vector2f(-5.0f, -5.0f);
    }
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