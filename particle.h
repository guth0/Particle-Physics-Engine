#pragma once
#include <SFML/Graphics.hpp>

struct Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f position_last;
    sf::Vector2f acceleration;

    float radius = 10.0f;
    sf::Color color = sf::Color::White;

    // Particle() = default;

    Particle(sf::Vector2f position_, float radius_) // constructor
        : position{position_}, position_last{position_}, acceleration{0.0f, 0.0f}, radius{radius_}
    {
    }

    void update(float dt)
    {
        // Compute how much particle moved
        const sf::Vector2f displacement = position - position_last;
        // Update pos

        position_last = position;
        position = position + displacement + acceleration * (dt * dt); // no dampening

        // with dampening
        // constexpr float damp_coef = 0.9f;
        // position = position + displacement + (acceleration * (damp_coef / (radius / 10))) * (dt * dt);

        // Reset acceleration
        acceleration = {};
    }

    void accelerate(sf::Vector2f a)
    {
        acceleration += a;
    }

    void setVelocity(sf::Vector2f v, float dt)
    {
        position_last = position - (v * dt);
    }

    void addVelocity(sf::Vector2f v, float dt)
    {
        position_last -= v * dt;
    }

    // nodiscard gives warning when calling function without using the return for anything
    [[nodiscard]] sf::Vector2f getVelocity(float dt) const
    {
        return (position - position_last) / dt;
    }
};