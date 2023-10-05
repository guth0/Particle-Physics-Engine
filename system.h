#pragma once
#include <SFML/Graphics.hpp>

struct Particle
{
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    sf::Color particleColor;
    float radius;
    float mass = 1;

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

    void wallBounce(float restitution)
    {

        // Check for collisions with side walls
        if (shape.getPosition().x - radius < 0)
        {
            // Collision with left boundary
            shape.setPosition(radius, shape.getPosition().y);
            velocity.x = -velocity.x * restitution; // Reverse the horizontal velocity
        }
        else if (shape.getPosition().x + radius > windowWidth)
        {
            // Collision with right boundary
            shape.setPosition(windowWidth - radius, shape.getPosition().y);
            velocity.x = -velocity.x * restitution; // Reverse the horizontal velocity
        }

        // colides with top/bottom
        if (shape.getPosition().y - radius <= 0)
        {
            shape.setPosition(shape.getPosition().x, radius);
            velocity.y = fabs(velocity.y) * restitution; // change the y velocity to pos
        }

        if (shape.getPosition().y + radius >= windowHeight)
        {
            velocity.y = -fabs(velocity.y) * restitution; // change the y velocity to neg
            shape.setPosition(shape.getPosition().x, windowHeight - radius);
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

    float restitution; // % energy conserved through collision

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

    void setRestitution(float x)
    {
        restitution = x;
    }

    void update(float Dtime)
    {
        for (int n = 0; n < 10; n++)
        {
            for (int i = 0; i < particles.size(); i++)
            {
                Particle &particle1 = particles[i];

                sf::Vector2f pos1 = particle1.shape.getPosition();
                int XCell1 = floor(pos1.x / winBallWidth);
                int YCell1 = floor(pos1.y / winBallHeight);

                for (int j = i + 1; j < particles.size(); j++)
                {
                    Particle &particle2 = particles[j];

                    sf::Vector2f pos2 = particle2.shape.getPosition();
                    int XCell2 = floor(pos2.x / winBallWidth);
                    int YCell2 = floor(pos2.y / winBallHeight);

                    if (XCell1 == XCell2 && YCell1 == YCell2) // if the particles are within range
                    {
                        handleParticleCollision(particle1, particle2);
                    }
                }
            }
        }

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

    void handleParticleCollision(Particle &particle1, Particle &particle2)
    {
        // Calculate the distance between the centers of the particles
        sf::Vector2f delta = particle2.shape.getPosition() - particle1.shape.getPosition();
        float distance = sqrt(delta.x * delta.x + delta.y * delta.y);

        // Check if particles are colliding and distance is not zero
        if (distance > 0 && distance < particle1.radius + particle2.radius)
        {
            // Calculate penetration depth
            float overlap = (particle1.radius + particle2.radius) - distance;

            // Calculate collision normal
            sf::Vector2f collisionNormal = delta / distance;

            // Move particles away from each other
            sf::Vector2f displacement = 0.5f * overlap * collisionNormal;
            particle1.shape.move(-displacement);
            particle2.shape.move(displacement);
        }
    }

    // void handleParticleCollision(Particle &particle1, Particle &particle2)
    // {
    //     // Calculate the distance between the centers of the particles
    //     sf::Vector2f delta = particle2.shape.getPosition() - particle1.shape.getPosition();
    //     float distance = sqrt(delta.x * delta.x + delta.y * delta.y);

    //     // Check if particles are colliding
    //     if (distance < particle1.radius + particle2.radius)
    //     {
    //         // Calculate collision normal
    //         sf::Vector2f collisionNormal = delta / distance;

    //         // Calculate relative velocity
    //         sf::Vector2f relativeVelocity = particle2.velocity - particle1.velocity;
    //         float dotProduct = relativeVelocity.x * collisionNormal.x + relativeVelocity.y * collisionNormal.y;

    //         // Check if particles are moving towards each other
    //         if (dotProduct < 0)
    //         {
    //             // Calculate impulse
    //             float impulse = (-(1 + restitution) * dotProduct) / (1 / particle1.mass + 1 / particle2.mass);

    //             // Apply impulse
    //             particle1.velocity -= (impulse / particle1.mass) * collisionNormal;
    //             particle2.velocity += (impulse / particle2.mass) * collisionNormal;

    //             // Separate particles to avoid overlap
    //             float overlap = (particle1.radius + particle2.radius - distance) / 2;
    //             particle1.shape.move(-overlap * collisionNormal);
    //             particle2.shape.move(overlap * collisionNormal);
    //         }
    //     }
    // }
};