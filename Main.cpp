// use "./build/main" in terminal to run
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "system.h"
#include "renderer.h"

constexpr uint16_t window_height = 850;
constexpr uint16_t window_width = window_height * 1512 / 982;
const sf::Color background_color = {0, 0, 0};
constexpr uint8_t RADIUS = 3;
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

    particleSystem.setSubStepsCount(8);
    particleSystem.setSimulationUpdateRate(frame_rate);

    particleSystem.setDrag(0.99f);
    particleSystem.setRestitution(.90f);

    particleSystem.resizeGrid(gridSize);

    particleSystem.setAttractionFactor(50);

    const sf::Vector2i window_resolution = {window_width, window_height};
    particleSystem.setConstraintBuffer(window_resolution, 20);

    particleSystem.setCenter(static_cast<sf::Vector2f>(window_resolution));

    particleSystem.setStandardRadius(RADIUS);
    // Setup system parameters

    Renderer renderer{window};

    // Set simulation attributes
    constexpr float particle_spawn_delay = 0.025f;
    constexpr float particle_spawn_speed = 500.0f; // too slow will make particles collide off spawn
    const sf::Vector2f spawn_center = {window_width / 2, window_height / 2};
    constexpr uint32_t spawn_radius = 300;
    constexpr uint32_t max_particle_count = 1000;
    constexpr float max_angle = 1.0f;
    uint32_t attraction_factor = 50;
    // Set simulation attributes

    sf::Clock clock;

    constexpr float wait_time = 0.5f; // wait x seconds before spawning any particles
    bool has_waited = false;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                particleSystem.setAttractionPoint(mousePosition);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            {
                attraction_factor += 50;
                particleSystem.setAttractionFactor(attraction_factor);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                if (attraction_factor >= 50) // negative factor deletes all points???
                {
                    attraction_factor -= 50;
                    particleSystem.setAttractionFactor(attraction_factor);
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            {
                attraction_factor = 0;
                particleSystem.setAttractionFactor(0);
            }
        }

        // Spawn particles
        if (has_waited)
        {
            if (particleSystem.getParticleCount() < max_particle_count && clock.getElapsedTime().asSeconds() >= particle_spawn_delay)
            {
                clock.restart();
                const float t = particleSystem.getTime();

                const float angular_speed = 1.0f;
                const float angle = angular_speed * t;

                float spawnX = spawn_center.x + spawn_radius * cos(angle);
                float spawnY = spawn_center.y + spawn_radius * sin(angle);

                Particle &particle = particleSystem.addParticle(sf::Vector2f(spawnX, spawnY), RADIUS);

                particleSystem.setParticleVelocity(particle, particle_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
                particle.color = getRainbow(t / 2);
            }
        }
        else if (clock.getElapsedTime().asSeconds() >= wait_time)
        {
            has_waited = true;
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
