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

            checkCollisions(step_dt);
            applyConstraint();
            updateParticles(step_dt);
        }
    }

    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    // void setConstraint(sf::Vector2f position, float radius)
    // {
    //     m_constraint_center = position;
    //     m_constraint_radius = radius;
    // }

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

    void
    setSubStepsCount(uint32_t sub_steps)
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

    // [[nodiscard]] sf::Vector3f getConstraint() const
    // {
    //     return {m_constraint_center.x, m_constraint_center.y, m_constraint_radius};
    // }

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
    // float m_constraint_radius = 100.0f;
    int m_constraint_border = 0; // distance from edge of window
    std::vector<Particle> m_particles;
    float m_time = 0.0f;
    float m_frame_dt = 0.0f;
    float m_attraction_factor = 0.1f;
    sf::Vector2i m_horozontal_constraints = {0, 800};
    sf::Vector2i m_vertical_constraints = {0, 800};

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

    void checkCollisions(float dt)
    {
        const float response_coef = 0.75f;
        const uint64_t particleCount = m_particles.size();

        for (uint64_t i{0}; i < particleCount; ++i)
        {
            Particle &particle1 = m_particles[i];

            for (uint64_t k{i + 1}; k < particleCount; ++k)
            {
                Particle &particle2 = m_particles[k];
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
        }
    }

    // void applyConstraint()
    // {
    //     for (Particle &particle : m_particles)
    //     {
    //         const sf::Vector2f v = m_constraint_center - particle.position;
    //         const float dist = sqrt(v.x * v.x + v.y * v.y);
    //         if (dist > (m_constraint_radius - particle.radius))
    //         {
    //             const sf::Vector2f n = v / dist;
    //             particle.position = m_constraint_center - n * (m_constraint_radius - particle.radius);
    //         }
    //     }
    // }

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

    void
    updateParticles(float dt)
    {
        for (Particle &particle : m_particles)
        {
            particle.update(dt);
        }
    }
};