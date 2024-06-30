#include <iostream>
#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include "utils/number_generator.hpp"
#include "utils/math.hpp"


static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * Math::PI);
    const float b = sin(t + 0.66f * 2.0f * Math::PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}


int32_t main(int32_t, char*[])
{
    // Create window
    constexpr int32_t window_width  = 1000;
    constexpr int32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Verlet", sf::Style::Close, settings);
    window.setFramerateLimit(60);

    Solver   solver;
    Renderer renderer{window};

    // Solver configuration
    solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(60);

    // Set simulation attributes
    const uint32_t     object_spawn_delay    = 2;
    const float        object_spawn_speed    = 1200.0f;
    const sf::Vector2f object_spawn_position = {500.0f, 200.0f};
    const float        object_min_radius     = 10.0f;
    const float        object_max_radius     = 18.0f;
    const uint32_t     max_objects_count     = 900;
    const float        max_angle             = 1.0f;

    unsigned nticks = 0;
    // Main loop
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (solver.getObjectsCount() < max_objects_count && nticks >= object_spawn_delay) {
            nticks = 0;
            const float radius_amp = (object_max_radius - object_min_radius) / 2;
            const float radius_avg = (object_min_radius + object_max_radius) / 2;
            const float t      = solver.getTime();
            auto&       object = solver.addObject(object_spawn_position, radius_amp * sin(1.4f * t + Math::PI * 0.5f) + radius_avg);
            const float angle  = max_angle * sin(t) + Math::PI * 0.5f;
            solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
            object.color = getRainbow(t);
        }
        nticks++;

        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
		window.display();
    }

    return 0;
}
