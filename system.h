#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "particle.h"

class ParticleSystem
{
public:
    ParticleSystem() = default; // Auto create default constructor

    Particle &addParticle(sf::Vector2f position, float radius)
    {
        return m_particles.emplace_back(position, radius);
    }

    void update()
    {
        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_num_substep}; i--;)
        {
            // applyGravity();
            applyAttraction();

            // clearGrid();
            // updateGrid();
            // handleGrid(step_dt);

            applyConstraint();

            updateParticles(step_dt);
        }
    }
    void setStandardRadius(uint8_t radius)
    {
        m_standard_radius = radius;
    }

    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void resizeGrid(std::pair<uint8_t, uint8_t> dimensions)
    {
        m_grid.resize(dimensions.first, std::vector<std::vector<Particle>>(dimensions.second));
        m_grid_dimensions = dimensions;
    }

    void setConstraintBuffer(sf::Vector2i window_resolution, int buffer)
    {
        m_constraint_border = buffer;

        m_horozontal_constraints.x = buffer;
        m_vertical_constraints.x = buffer;

        m_horozontal_constraints.y = window_resolution.x - buffer;
        m_vertical_constraints.y = window_resolution.y - buffer;
    }

    void setCenter(sf::Vector2f window_resolution)
    {
        m_center = window_resolution * .5f;
    }

    void setSubStepsCount(uint32_t sub_steps)
    {
        m_num_substep = sub_steps;
    }

    void setAttractionFactor(float x)
    {
        m_attraction_factor = x;
    }

    void setParticleVelocity(Particle &particle, sf::Vector2f v)
    {
        particle.setVelocity(v, getStepDt());
    }

    [[nodiscard]] const std::vector<Particle> &getParticles() const
    {
        return m_particles;
    }

    [[nodiscard]] uint64_t getParticleCount() const
    {
        return m_particles.size();
    }

    [[nodiscard]] float getTime() const
    {
        return m_time;
    }

    [[nodiscard]] float getStepDt() const
    {
        return m_frame_dt / static_cast<float>(m_num_substep);
    }

private:
    uint32_t m_num_substep = 1;
    sf::Vector2f gravity = {0.0f, 1000.0f};
    sf::Vector2f m_center;
    std::vector<Particle> m_particles;
    uint8_t m_standard_radius;

    float m_time = 0.0f;
    float m_frame_dt = 0.0f;
    float m_attraction_factor = 0.1f;

    sf::Vector2i m_horozontal_constraints = {0, 800};
    sf::Vector2i m_vertical_constraints = {0, 800};
    int m_constraint_border = 0; // distance from edge of window

    std::vector<std::vector<std::vector<Particle>>> m_grid;
    std::pair<uint8_t, uint8_t> m_grid_dimensions;
    const std::vector<int8_t> x_deltas = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    const std::vector<int8_t> y_deltas = {1, 0, -1, 1, 0, -1, 1, 0, -1};

    void applyAttraction()
    {
        for (Particle &particle : m_particles)
        {
            const sf::Vector2f direction = m_center - particle.position;

            const sf::Vector2f attraction_force = direction * m_attraction_factor;

            particle.accelerate(attraction_force);
        }
    }

    void applyGravity()
    {
        for (Particle &particle : m_particles)
        {
            particle.accelerate(gravity);
        }
    }

    void checkCollisions(float dt, Particle &particle1, Particle &particle2)
    {
        const float response_coef = 0.75f;

        const sf::Vector2f vec = particle1.position - particle2.position;
        float dist = vec.x * vec.x + vec.y * vec.y;
        const float minDist = particle1.radius + particle2.radius;
        // Check overlapping
        if (dist < minDist * minDist)
        {
            dist = sqrt(dist);
            const sf::Vector2f n = vec / dist;
            const float massRatio1 = particle1.radius / (particle1.radius + particle2.radius);
            const float massRatio2 = particle2.radius / (particle1.radius + particle2.radius);
            const float delta = 0.5f * response_coef * (dist - minDist);
            // Update positions
            particle1.position -= n * (massRatio2 * delta);
            particle2.position += n * (massRatio1 * delta);
        }
    }

    void applyConstraint()
    {
        for (Particle &particle : m_particles)
        {
            if (particle.position.x < m_horozontal_constraints.x + particle.radius)
            {
                particle.position.x = m_horozontal_constraints.x + particle.radius;
            }
            else if (particle.position.x > m_horozontal_constraints.y - particle.radius)
            {
                particle.position.x = m_horozontal_constraints.y - particle.radius;
            }

            if (particle.position.y > m_vertical_constraints.y - particle.radius)
            {
                particle.position.y = m_vertical_constraints.y - particle.radius;
            }
            else if (particle.position.y < m_vertical_constraints.x + particle.radius)
            {
                particle.position.y = m_vertical_constraints.x + particle.radius;
            }
        }
    }

    void updateParticles(float dt)
    {
        for (Particle &particle : m_particles)
        {
            particle.update(dt);
        }
    }

    void clearGrid()
    {
        for (auto &column : m_grid)
        {
            for (auto &cell : column)
            {
                cell.clear();
            }
        }
    }

    void updateGrid()
    {
        for (Particle &particle : m_particles)
        {
            int x = static_cast<int>(particle.position.x / (m_standard_radius * 2));
            int y = static_cast<int>(particle.position.y / (m_standard_radius * 2));

            m_grid[x][y].push_back(particle);
        }
    }

    void handleGrid(float dt)
    {
        std::cout << "# Particles: " << getParticleCount() << std::endl;
        for (int x = 0; x < m_grid_dimensions.first; x++)
        {
            for (int y = 0; y < m_grid_dimensions.second; y++)
            {
                for (Particle &particle1 : m_grid[x][y])
                {
                    for (int i = 0; i < 9; i++)
                    {

                        for (Particle &particle2 : m_grid[x + x_deltas[i]][y + y_deltas[i]])
                        {
                            if (&particle1 != &particle2)
                            {
                                checkCollisions(dt, particle1, particle2);
                            }
                        }
                    }
                }
            }
        }
    }
};

//