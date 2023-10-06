#pragma once
#include "system.h"

class Renderer
{
public:
    explicit Renderer(sf::RenderTarget &target)
        : m_target{target}
    {
    }

    void render(const ParticleSystem &system) const
    {
        // Render constraintgetConstraint
        // const sf::Vector3f constraint = system.getConstraint(); // returns x, y, radius
        // sf::CircleShape constraint_background{constraint.z};
        // constraint_background.setOrigin(constraint.z, constraint.z);
        // constraint_background.setFillColor(sf::Color::Black);
        // constraint_background.setPosition(constraint.x, constraint.y);
        // constraint_background.setPointCount(128);
        // m_target.draw(constraint_background);

        // Render objects
        sf::CircleShape circle{1.0f};
        circle.setPointCount(32);
        circle.setOrigin(1.0f, 1.0f);
        const std::vector<Particle> &particles = system.getParticles();
        for (const Particle &particle : particles)
        {
            circle.setPosition(particle.position);
            circle.setScale(particle.radius, particle.radius);
            circle.setFillColor(particle.color);
            m_target.draw(circle);
        }
    }

private:
    sf::RenderTarget &m_target;
};
