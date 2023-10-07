// use "./build/main" in terminal to run
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "system.h"
#include "renderer.h"

constexpr uint16_t window_height = 800;
constexpr uint16_t window_width = window_height * 1512 / 982;
const sf::Color background_color = {0, 0, 0};
constexpr uint8_t RADIUS = 5;
const std::pair<uint16_t, uint16_t> gridSize = std::make_pair(ceil(window_width / (RADIUS * 2)), ceil(window_height / (RADIUS * 2)));

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

    std::cout << "Grid dimensions (" << gridSize.first << ", " << gridSize.second << ")" << std::endl;

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

    particleSystem.setDrag(0.98f);

    particleSystem.resizeGrid(gridSize, RADIUS * 2);

    particleSystem.setAttractionFactor(400.0f);

    const sf::Vector2i window_resolution = {window_width, window_height};
    particleSystem.setConstraintBuffer(window_resolution, RADIUS * 2);

    particleSystem.setCenter(static_cast<sf::Vector2f>(window_resolution));

    particleSystem.setStandardRadius(RADIUS);
    // Setup system parameters

    Renderer renderer{window};

    // Set simulation attributes
    constexpr float particle_spawn_delay = 0.05f;
    constexpr float particle_spawn_speed = 0.0f; // too slow will make particles collide off spawn
    constexpr float particle_min_radius = 5.0f;
    constexpr float particle_max_radius = 15.0f;
    const sf::Vector2f particle_spawn_position = {window_width / 2, window_height / 2};
    constexpr uint16_t spawn_radius = (window_height / 2) * .9;
    constexpr uint32_t max_particle_count = 1200;
    constexpr float max_angle = 1.2f;
    int64_t attraction_factor = 200;
    // Set simulation attributes

    sf::Clock clock;

    constexpr float wait_time = 1.5f; // wait x seconds before spawning any particles
    bool has_waited = false;

    // Particle &particle1 = particleSystem.addParticle(sf::Vector2f{8 * RADIUS, 6.01 * RADIUS}, RADIUS);

    // Particle &particle2 = particleSystem.addParticle(sf::Vector2f{8 * RADIUS, 8 * RADIUS}, RADIUS);

    // bool colored = false;

    // int frames = 600;

    while (window.isOpen()) //&& frames > 0
    {
        // frames--;
        sf::Event event;
        while (window.pollEvent(event)) // goes through event stack, checks if each event is the window closing
        {

            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            {
                attraction_factor += 50;
                particleSystem.setAttractionFactor(attraction_factor);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                attraction_factor -= 50;
                particleSystem.setAttractionFactor(attraction_factor);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            {
                attraction_factor = 0;
                particleSystem.setAttractionFactor(0);
            }
            // for (int key = sf::Keyboard::Num0; key <= sf::Keyboard::Num9; ++key)
            // {
            //     if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key)))
            //     {
            //         particleSystem.setAttractionFactor(50.0f * (key + 1));
            //     }
            // }
        }

        // Spawn particles
        if (has_waited)
        {
            if (particleSystem.getParticleCount() < max_particle_count)
            {
                if (clock.getElapsedTime().asSeconds() >= particle_spawn_delay)
                {
                    clock.restart();
                    const float t = particleSystem.getTime();

                    // const float angle = max_angle * sin(t / 2) + M_PI;

                    const float angular_speed = 1.0f; // Adjust the angular speed as needed
                    const float angle = angular_speed * t;

                    float spawnX = particle_spawn_position.x + spawn_radius * cos(angle);
                    float spawnY = particle_spawn_position.y + spawn_radius * sin(angle);

                    // Create the particle at the calculated spawn position
                    Particle &particle = particleSystem.addParticle(sf::Vector2f(spawnX, spawnY), RADIUS);

                    particleSystem.setParticleVelocity(particle, particle_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
                    particle.color = getRainbow(t / 2);
                }
            }
        }
        else if (clock.getElapsedTime().asSeconds() >= wait_time)
        {
            has_waited = true;
        }

        // if (!colored)
        // {
        //     particle2.color = sf::Color::Red;
        //     colored = true;
        // }

        // Spawn particles

        // Update particle system
        particleSystem.update();
        window.clear(background_color);
        renderer.render(particleSystem);
        window.display();
    }

    return 0;
}
