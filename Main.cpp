// use "./build/main" in terminal to run
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "system.h"
#include "renderer.h"

constexpr uint16_t window_height = 850;
constexpr uint16_t window_width = window_height * 1512 / 982;
const sf::Color background_color = {50, 50, 50};
constexpr float RADIUS = 10;

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
    settings.antialiasingLevel = 2;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Particle Simulation");
    constexpr uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);
    // Set up winwow

    // Setup system parameters
    ParticleSystem particleSystem;

    const float constraint_radius = std::min(window_width, window_height) * (.9f / 2);
    particleSystem.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, constraint_radius);

    particleSystem.setSubStepsCount(8);
    particleSystem.setSimulationUpdateRate(frame_rate);
    // Setup system parameters

    Renderer renderer{window};

    // Set simulation attributes
    const float particle_spawn_delay = 0.025f;
    const float particle_spawn_speed = 1200.0f;
    const sf::Vector2f particle_spawn_position = {window_width / 2, 200.0f};
    const float particle_min_radius = 5.0f;
    const float particle_max_radius = 15.0f;
    const uint32_t max_particle_count = 1000;
    const float max_angle = 1.0f;
    // Set simulation Attributes

    sf::Clock clock;
    float last_time = 0;
    float elapsed;
    int fps;

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
            const float radius = fabs(sin(t) * (particle_max_radius - particle_min_radius)) + particle_min_radius;
            Particle &particle = particleSystem.addParticle(particle_spawn_position, radius);
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
