// The grid is full of particles not pointers to particles

#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "particle.h"
#include "grid.h"

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
        m_grid.clear();
        updateGrid();

        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_num_substep}; i--;)
        {
            // applyGravity();
            applyAttraction();

            applyDrag();

            handleGrid(step_dt);

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
        m_grid.setSize(dimensions.first, dimensions.second);
    }

    void setConstraintBuffer(sf::Vector2i window_resolution, int buffer)
    {
        m_constraint_border = buffer;

        m_world_size = window_resolution;

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

    void setDrag(float drag)
    {
        m_drag = drag;
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
    sf::Vector2f m_gravity = {0.0f, 1000.0f};
    float m_drag = 0.75f;
    sf::Vector2f m_center;
    std::vector<Particle> m_particles;
    uint8_t m_standard_radius;

    float m_time = 0.0f;
    float m_frame_dt = 0.0f;
    float m_attraction_factor = 0.1f;

    sf::Vector2i m_world_size;

    // for rectangle border
    sf::Vector2i m_horozontal_constraints = {0, 800};
    sf::Vector2i m_vertical_constraints = {0, 800};
    int m_constraint_border = 0; // distance from edge of window
    // for rectangle border

    CollisionGrid m_grid;
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
            particle.accelerate(m_gravity);
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

    void applyDrag()
    {
        for (Particle &particle : m_particles)
        {
            particle.slowdown(m_drag);
        }
    }

    void updateGrid()
    {
        for (int i = 0; i < getParticleCount(); i++)
        {
            Particle &particle = m_particles[i];

            if (particle.position.x > m_standard_radius && particle.position.x < m_world_size.x - m_standard_radius &&
                particle.position.y > m_standard_radius && particle.position.y < m_world_size.y - m_standard_radius)
            {
                m_grid.addObject(particle.position.x, particle.position.y, i);
            }
        }
    }

    void checkCollisions(float dt, int index_1, int index_2)
    {

        constexpr float response_coef = 1.0f;
        constexpr float eps = 0.0001f;

        Particle &particle_1 = m_particles[index_1];
        Particle &particle_2 = m_particles[index_2];
        const sf::Vector2f p2_p1 = particle_1.position - particle_2.position;
        const float dist2 = p2_p1.x * p2_p1.x + p2_p1.y * p2_p1.y;
        if (dist2 < m_standard_radius && dist2 > eps)
        {
            std::cout << "here" << std::endl;

            const float dist = sqrt(dist2);

            const float delta = response_coef * 0.5f * (m_standard_radius - dist);
            const sf::Vector2f col_vec = (p2_p1 / dist) * delta;
            particle_1.position += col_vec;
            particle_2.position -= col_vec;
        }
    }

    void checkParticleCellCollisions(float dt, uint32_t index, const CollisionCell &c)
    {
        for (uint32_t i = 0; i < c.objects_count; ++i)
        {
            checkCollisions(dt, index, c.objects[i]);
        }
    }

    void handleGrid(float dt)
    {
        for (int x = 1; x < m_grid.width - 1; x++) // skip far left and right
        {
            for (int y = 1; y < m_grid.height - 1; y++) // skip top and bottom
            {
                for (auto index : m_grid.data[x][y].objects)
                {
                    for (int i = 0; i < 9; i++)
                    {
                        checkParticleCellCollisions(dt, index, m_grid.data[x + x_deltas[i]][y + y_deltas[i]]);
                    }
                }
            }
        }
    }
};

//