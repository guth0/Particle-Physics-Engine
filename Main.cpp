// use "./build/main" in terminal to run

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Head.h"

const int windowHeight = 600;
const int windowWidth = 800;
const float RADIUS = 1;

class Particle
{
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    sf::Color particleColor;
    float radius;

    Particle(float x, float y, float radius)
        : shape(radius), velocity(0.0f, 0.0f), radius(radius)
    {
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue); // defaunt color
        shape.setOrigin(radius, radius);     // Center the origin for rotation
    }

    void setVelocity(float x, float y) // Set the velocity
    {
        velocity.x = x;
        velocity.y = y;
    }

    void applyImpulse(float x, float y) // Change the velocity
    {
        velocity.x += x;
        velocity.y += y;
    }

    void setColor(sf::Color color) // set color
    {
        shape.setFillColor(color);
    }

    void wallBounce()
    {
        // Check for collisions with the walls
        if (shape.getPosition().x - radius <= 0 || shape.getPosition().x + radius >= windowWidth)
        {
            // collides with side walls
            velocity.x = -velocity.x; // reverse the x velocity

            // velocity.x -= velocity.x * 0.01f; // remove x% of V on impact
        }

        // colides with top/bottom
        if (shape.getPosition().y - radius <= 0)
        {
            velocity.y = fabs(velocity.y); // change the y velocity to pos
        }

        if (shape.getPosition().y + radius >= windowHeight)
        {
            velocity.y = -fabs(velocity.y); // change the y velocity to neg
        }
    }

    void update(float Dtime, sf::Vector2f gravity, float dampCoef)
    {
        // Update particle position based on velocity here
        shape.move(velocity * Dtime);

        wallBounce();

        velocity += gravity * Dtime; // apply gravity

        sf::Vector2f dampingForce = -velocity * dampCoef;
        velocity += dampingForce * Dtime; // use (dampingForce / mass) * Dtime if mass is involved

        // if (fabs(velocity.x) < 0.25f)
        // {
        //     velocity.x = 0;
        // }

        // if (fabs(velocity.y) < 0.25f)
        // {
        //     velocity.y = 0;
        // }

        // Apply any other forces or interactions here
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
    sf::Vector2f gravity;
    float dampingCoefficient;

    void addParticle(float x, float y, float radius)
    {
        particles.emplace_back(x, y, radius);
    }

    void setGravity(float x, float y) // set the gravity
    {
        gravity.x = x;
        gravity.y = y;
    }

    void update(float Dtime)
    {
        for (auto &particle : particles)
        {
            particle.update(Dtime, gravity, dampingCoefficient);
        }
    }

    void draw(sf::RenderWindow &window)
    {
        for (auto &particle : particles)
        {
            particle.draw(window);
        }
    }

    void setDamping(float damping)
    {
        dampingCoefficient = damping;
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Particle Simulation");
    ParticleSystem particleSystem;

    // Add particles to the system
    // int n = 30;
    // for (int i = 0; i < 30; i++)
    // {
    //     particleSystem.addParticle(50.0f, 50.0f, 10.0f);
    //     particleSystem.particles[i].applyImpulse(20.0f * i, 0.0f);
    // }

    particleSystem.gravity.x = 0.0f;
    particleSystem.gravity.y = 9.81f * 60; // 9.81 m/s

    particleSystem.setDamping(0.5f);

    // particleSystem.particles[0].applyImpulse(-400.0f, 70.0f);

    sf::Clock clock;

    float hue = 0.0f;

    int n = 25000;
    int i = 0;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event)) // goes through event stack, checks if each event is the window closing
        {
            if (event.type == sf::Event::Closed)
                window.close();
            break;
        }

        sf::Color color = HSVtoRGB(hue, 1.0f, 1.0f);

        if (i <= n)
        {
            particleSystem.addParticle(50.0f, 50.0f, RADIUS);
            particleSystem.particles[i].applyImpulse(300.0f, 0.0f);

            sf::Color color = HSVtoRGB(hue, 1.0f, 1.0f);
            particleSystem.particles[i].setColor(color);

            i++;

            hue += 0.005f;
            if (hue > 1.0f)
            {
                hue -= 1.0f;
            }
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
