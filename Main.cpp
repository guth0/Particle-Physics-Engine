// use "./build/main" in terminal to run

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "head.h"
#include "system.h"
#include "renderer.h"

constexpr uint16_t window_height = 1000;
constexpr uint16_t window_width = 1000;
constexpr float RADIUS = 10;
constexpr float restitution = 0.9f; // % energy conserved through collision
const uint8_t winBallHeight = ceil(window_height / RADIUS);
const uint8_t winBallWidth = ceil(window_width / RADIUS);

int main()
{
    // Set up window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Particle Simulation");
    constexpr uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);
    // Set up winwow

    // Setup system parameters
    ParticleSystem particleSystem;

    particleSystem.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);

    particleSystem.setSubStepsCount(8);
    particleSystem.setSimulationUpdateRate(frame_rate);
    // Setup system parameters

    Renderer renderer{window};

    // Set simulation attributes
    const float particle_spawn_delay = 0.025f;
    const float particle_spawn_speed = 1200.0f;
    const sf::Vector2f particle_spawn_position = {500.0f, 200.0f};
    const float particle_min_radius = 1.0f;
    const float particle_max_radius = 20.0f;
    const uint32_t max_particle_count = 1000;
    const float max_angle = 1.0f;
    // Set simulation Attributes

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

        float deltaTime = clock.restart().asSeconds();

        // Spawn particles
        if (particleSystem.getParticleCount() < max_particle_count && clock.getElapsedTime().asSeconds() >= particle_spawn_delay)
        {
            clock.restart();
            auto &particle = particleSystem.addParticle(particle_spawn_position, 5.0f);
            const float t = particleSystem.getTime();
            const float angle = max_angle * sin(t) + M_PI * 0.5f;
            particleSystem.setParticleVelocity(particle, particle_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
            particle.color = getRainbow(t);
        }
        // Spawn particles

        // Update particle system
        particleSystem.update();
        window.clear(sf::Color::White);
        renderer.render(particleSystem);
        window.display();
    }

    return 0;
}
