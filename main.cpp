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


int32_t main()
{
    // Create window
    constexpr uint32_t window_width  = 1000;
    constexpr uint32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Verlet", sf::Style::Close, settings);

    Solver   solver;
    Renderer renderer{window};

    // Solver configuration
    constexpr float   tick_dt        = 0.033f; // must be >= object_spawn_delay, or jittering will happen (no good way to solve this)
    constexpr int32_t max_frame_skip = 5;
    solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);
    solver.setSubStepsCount(6);
    solver.setSimulationTickDt(tick_dt);

    // Set simulation attributes
    constexpr float        object_spawn_delay    = 0.033f;
    constexpr float        object_spawn_speed    = 1200.0f;
    const     sf::Vector2f object_spawn_position = {500.0f, 200.0f};
    constexpr float        object_min_radius     = 12.0f;
    constexpr float        object_max_radius     = 18.0f;
    constexpr uint32_t     max_objects_count     = 700;
    constexpr float        max_angle             = 1.0f;

    // Main loop
    sf::Clock clock;
    float     accumulator = 0;
    uint32_t  spawn       = 0;
    clock.restart();
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        int32_t frame_skip = -1;
        accumulator += clock.getElapsedTime().asSeconds();
        clock.restart();
        while (accumulator >= tick_dt && frame_skip < max_frame_skip) {
            if (solver.getObjectsCount() < max_objects_count && spawn * tick_dt >= object_spawn_delay) {
                spawn = 0;
                const float radius_amp = (object_max_radius - object_min_radius) / 2;
                const float radius_avg = (object_min_radius + object_max_radius) / 2;
                const float t          = solver.getTime();
                const float angle      = max_angle * sin(t) + Math::PI * 0.5f;
                auto&       object     = solver.addObject(object_spawn_position, radius_amp * sin(1.4f * t + Math::PI * 0.5f) + radius_avg);
                solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
                object.color = getRainbow(t);
            }
            solver.update();
            accumulator -= tick_dt;
            spawn++;
            frame_skip++;
        }

        sf::String text = "Verlet | Frame Skip: ";
        text += std::to_string(frame_skip);
        if (frame_skip == max_frame_skip) {
            accumulator = 0.0;
            text += " (MAX)";
        }
        window.setTitle(text); // -1 means solver.update not happened yet

        window.clear(sf::Color::White);
        renderer.render(solver, accumulator);
		window.display();
    }

    return 0;
}
