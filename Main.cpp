// use "./build/main" in terminal to run
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "system.h"
#include "renderer.h"

constexpr uint16_t window_height = 850;
constexpr uint16_t window_width = window_height * 1512 / 982;
const sf::Color background_color = {0, 0, 0};
constexpr uint8_t RADIUS = 5;
const std::pair<uint8_t, uint8_t> gridSize = std::make_pair(ceil(window_width / (RADIUS * 2)), ceil(window_height / (RADIUS * 2)));

static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * M_PI);
    const float b = sin(t + 0.66f * 2.0f * M_PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

int main()
{
    // Set up window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Particle Simulation");
    constexpr uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);
    // Set up window

    // Setup system parameters
    ParticleSystem particleSystem;

    // const float constraint_radius = std::min(window_width, window_height) * (.9f / 2);
    // particleSystem.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, constraint_radius);

    particleSystem.setSubStepsCount(8);
    particleSystem.setSimulationUpdateRate(frame_rate);

    particleSystem.resizeGrid(gridSize);

    particleSystem.setAttractionFactor(5.0f);

    const sf::Vector2i window_resolution = {window_width, window_height};
    particleSystem.setConstraintBuffer(window_resolution, 20);

    particleSystem.setCenter(static_cast<sf::Vector2f>(window_resolution));

    particleSystem.setStandardRadius(RADIUS);
    // Setup system parameters

    Renderer renderer{window};

    // Set simulation attributes
    constexpr float particle_spawn_delay = 0.025f;
    constexpr float particle_spawn_speed = 500.0f; // too slow will make particles collide off spawn
    constexpr float particle_min_radius = 5.0f;
    constexpr float particle_max_radius = 15.0f;
    const sf::Vector2f particle_spawn_position = {window_width / 2, 200.0f};
    constexpr uint32_t max_particle_count = 1000;
    constexpr float max_angle = 1.0f;
    // Set simulation attributes

    sf::Clock clock;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event)) // goes through event stack, checks if each event is the window closing
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        // Spawn particles
        if (particleSystem.getParticleCount() < max_particle_count && clock.getElapsedTime().asSeconds() >= particle_spawn_delay)
        {
            clock.restart();
            const float t = particleSystem.getTime();
            Particle &particle = particleSystem.addParticle(particle_spawn_position, RADIUS);
            const float angle = max_angle * sin(t) + M_PI * 0.5f;
            particleSystem.setParticleVelocity(particle, particle_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
            particle.color = getRainbow(t);
        }
        // Spawn particles

        // Update particle system
        particleSystem.update();
        window.clear(background_color);
        renderer.render(particleSystem);
        window.display();
    }

    return 0;
}
