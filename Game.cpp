#include "Game.h"
#include "GraphicsEngine.h"
#include "Level.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;

const float Game::FPS_CAP = 60.0f;
const float Game::PI = acos(-1.0f);
Game::State Game::state = State::uninitialized;
mt19937 Game::mainRNG;

int Game::start() {
    RenderWindow window;
    try {
        // Game setup.
        assert(state == State::uninitialized);
        state = State::playing;
        mainRNG.seed(static_cast<unsigned long>(chrono::high_resolution_clock::now().time_since_epoch().count()));
        window.create(VideoMode(500, 500), "Game Engine 3D v2");
        
        GraphicsEngine graphicsEngine;
        graphicsEngine.init(Vector2u(5, 5), window.getSize());
        Level level;
        level.load("levels/level0.csv");
        //level.save("levels/level0cpy.csv");
        Vector3f camPosition(0.0f, 0.0f, 5.0f), camRotation(0.0f, 0.0f, 0.0f);
        Vector2i mousePosition(0, 0);
        View view(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
        Clock mainClock, fpsClock;    // The mainClock keeps track of elapsed frame time, fpsClock is used to count frames per second.
        int fpsCounter = 0;
        
        /*VertexArray triangle(Triangles, 3);
        triangle[0].position = Vector2f(10.0f, 10.0f);
        triangle[1].position = Vector2f(100.0f, 10.0f);
        triangle[2].position = Vector2f(100.0f, 100.0f);
        triangle[0].color = Color::White;
        triangle[1].color = Color::Black;
        triangle[2].color = Color(0, 0, 0, 0);
        triangle[0].texCoords = Vector2f(0.0f, 0.0f);
        triangle[1].texCoords = Vector2f(125.0f, 0.0f);
        triangle[2].texCoords = Vector2f(125.0f, 125.0f);
        Texture texture;
        if (!texture.loadFromFile("test.png")) {
            cout << "Unable to load texture." << endl;
            system("pause");
            return -1;
        }
        window.draw(triangle, &texture);*/
        
        while (state != State::exiting) {
            // Re-draw objects.
            graphicsEngine.redraw(level, camPosition, camRotation);
            window.clear ();
            window.setView(view);
            window.draw(graphicsEngine.getPixelMap());
            window.display();
            
            while (mainClock.getElapsedTime().asSeconds() < 1.0f / FPS_CAP) {}    // Slow down simulation if the current FPS is greater than the FPS cap.
            float deltaTime = mainClock.restart().asSeconds();    // Change in time since the last frame.
            
            if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {    // Calculate FPS.
                window.setTitle("Game Engine 3D v2 (FPS: " + to_string(fpsCounter) + ", pos = (" + to_string(camPosition.x) + ", " + to_string(camPosition.y) + ", " + to_string(camPosition.z) + "), rot = (" + to_string(camRotation.x) + ", " + to_string(camRotation.y) + ", " + to_string(camRotation.z) + ") )");
                fpsClock.restart();
                fpsCounter = 0;
            } else {
                ++fpsCounter;
            }
            
            // Update objects.
            Event event;
            while (window.pollEvent(event)) {    // Process events.
                if (event.type == Event::MouseMoved) {
                    if (Mouse::isButtonPressed (Mouse::Left)) {
                        camRotation.y += (event.mouseMove.x - mousePosition.x) * 0.01f;
                        if (camRotation.y >= PI * 2.0f) {
                            camRotation.y -= PI * 2.0f;
                        } else if (camRotation.y <= -PI * 2.0f) {
                            camRotation.y += PI * 2.0f;
                        }
                        
                        camRotation.x += (event.mouseMove.y - mousePosition.y) * 0.01f;
                        if (camRotation.x > PI / 2.0f) {
                            camRotation.x = PI / 2.0f;
                        } else if (camRotation.x < -PI / 2.0f) {
                            camRotation.x = -PI / 2.0f;
                        }
                    }
                    
                    mousePosition.x = event.mouseMove.x;
                    mousePosition.y = event.mouseMove.y;
                } else if (event.type == Event::Resized) {
                    graphicsEngine.init(Vector2u(5, 5), window.getSize());
                    view.reset(FloatRect(Vector2f(0.0f, 0.0f), Vector2f(window.getSize())));
                } else if (event.type == Event::Closed) {
                    window.close();
                    state = State::exiting;
                }
            }
            
            // Check for keypress.
            float deltaX = 0.0f, deltaZ = 0.0f;
            if (Keyboard::isKeyPressed (Keyboard::W)) {
                deltaZ -= 5.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::S)) {
                deltaZ += 5.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::D)) {
                deltaX += 5.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::A)) {
                deltaX -= 5.0f * deltaTime;
            }
            camPosition.x += deltaX * cos(camRotation.y) - deltaZ * sin(camRotation.y);
            camPosition.z += deltaX * sin(camRotation.y) + deltaZ * cos(camRotation.y);
            
            if (Keyboard::isKeyPressed (Keyboard::Space)) {
                camPosition.y += 5.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::LShift)) {
                camPosition.y -= 5.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::Q)) {
                camRotation.z -= 1.0f * deltaTime;
            }
            if (Keyboard::isKeyPressed (Keyboard::E)) {
                camRotation.z += 1.0f * deltaTime;
            }
        }
    } catch(exception& ex) {    // Catch any exceptions here and handle them as a game crash.
        window.close();
        
        cout << "\n*******************************************" << endl;
        cout << "Oh noes! The game crashed unexpectedly. X_X" << endl;
        cout << "\n- Details -" << endl;
        cout << "  Exception thrown: " << ex.what() << endl;
        cout << "(Press enter)" << endl;
        cout << "*******************************************" << endl;
        cin.get();
        
        return -1;
    }
    
    return 0;
}

int Game::randomInteger(int min, int max) {
    uniform_int_distribution<int> minMaxRange(min, max);
    return minMaxRange(mainRNG);
}

int Game::randomInteger(int n) {
    return randomInteger(0, n - 1);
}