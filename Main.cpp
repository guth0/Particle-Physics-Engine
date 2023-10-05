// use "./build/main" in terminal to run

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "Head.h"
#include "Solver.h"

constexpr uint16_t windowHeight = 1000;
constexpr uint16_t windowWidth = 1000;
constexpr float RADIUS = 10;
constexpr float restitution = 0.9f; // % energy conserved through collision
const uint8_t winBallHeight = ceil(windowHeight / RADIUS);
const uint8_t winBallWidth = ceil(windowWidth / RADIUS);

int main()
{
    // Set up window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Particle Simulation");
    constexpr uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);
    // Set up winwow

    // Setup system parameters
    ParticleSystem particleSystem;

    particleSystem.gravity.x = 0.0f;
    particleSystem.gravity.y = 9.81f * 10; // 9.81 m/s

    particleSystem.setDamping(0.5f);
    particleSystem.setRestitution(0.9f);
    // Setup system parameters

    // Spawn Particles
    float hue = 0.0f;
    for (int i = 0; i < 200; i++)
    {
        particleSystem.addParticle(i * RADIUS / 2, i * RADIUS / 2, RADIUS);
        particleSystem.particles[i].applyImpulse(300.0f, 0.0f);

        sf::Color color = HSVtoRGB(hue, 1.0f, 1.0f);
        particleSystem.particles[i].setColor(color);

        hue += 0.005f;
        if (hue > 1.0f)
        {
            hue -= 1.0f;
        }
    }
    // Spawn Particles

    sf::Clock clock;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event)) // goes through event stack, checks if each event is the window closing
        {
            if (event.type == sf::Event::Closed)
                window.close();
            break;
        }

        float deltaTime = clock.restart().asSeconds();

        // float fps = 1.0f / deltaTime;
        // std::cout << "\nFPS: " << fps << " --- BALLS: " << i << std::endl;

        // Update particle system
        // Process events and update particle positions here
        particleSystem.update(deltaTime);

        window.clear();

        // Draw particles
        // Loop through particles and draw them at their positions
        // using particleShape and window.draw()
        particleSystem.draw(window);

        window.display();
    }

    return 0;
}
