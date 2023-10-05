// use "./build/main" in terminal to run

#include <SFML/Graphics.hpp>
#include <vector>

#define RADIUS 10.0F;

class Particle
{
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;

    Particle(float x, float y, float radius)
        : shape(radius), velocity(0.0f, 0.0f), radius(radius)
    {
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin(radius, radius); // Center the origin for rotation
    }

    void update()
    {
        // Update particle position based on velocity here
        shape.move(velocity);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
    }
};

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    void addParticle(float x, float y, float radius)
    {
        particles.emplace_back(x, y, radius);
    }

    void update()
    {
        for (auto &particle : particles)
        {
            particle.update();
        }
    }

    void draw(sf::RenderWindow &window)
    {
        for (auto &particle : particles)
        {
            particle.draw(window);
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Simulation");
    ParticleSystem particleSystem;

    // Add particles to the system
    particleSystem.addParticle(400.0f, 300.0f, 10.0f); // Example particle

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) // goes through event stack, checks if each event is the window closing
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update particle system
        // Process events and update particle positions here
        particleSystem.update();

        window.clear();

        // Draw particles
        // Loop through particles and draw them at their positions
        // using particleShape and window.draw()
        particleSystem.draw(window);

        window.display();
    }

    return 0;
}
