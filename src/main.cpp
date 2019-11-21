#include "Constants.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

// let's make it simple, only integers
struct Pipe
{
    Pipe(uint32_t x, uint32_t y)
        : m_x(x)
        , m_y(y)
    {}

    /*
    Pipe(const Pipe & p)
    {
        m_x = p.m_x;
        m_y = p.m_y;
        m_pipe_top = p.m_pipe_top;
        m_pipe_bottom = p.m_pipe_bottom;
    }

    Pipe(Pipe && p)
    {
        m_x = p.m_x;
        m_y = p.m_y;
        m_pipe_top = std::move(p.m_pipe_top);
        m_pipe_bottom = std::move(p.m_pipe_bottom);
    }

    Pipe & operator<<(const Pipe & p)
    {
        m_x = p.m_x;
        m_y = p.m_y;
        m_pipe_top = p.m_pipe_top;
        m_pipe_bottom = p.m_pipe_bottom;
    }*/

    void update()
    {
        m_pipe_top.move(sf::Vector2f(-1.f, 0.f));
        m_pipe_bottom.move(sf::Vector2f(-1.f, 0.f));
    }

    bool is_alive() const
    {
        auto pos = m_pipe_top.getPosition();
        if (pos.x + m_x < 0) {
            std::cout << "dead x {" << pos.x << ":" << pos.x + m_x << "} y {" << pos.y << ":" << pos.y + m_y << "}" << std::endl;
            return false;
        }
        return true;
    }

    void display(sf::RenderWindow & w) const
    {
        w.draw(m_pipe_top);
        w.draw(m_pipe_bottom);
    }

    template<typename T>
    bool collide(const T & object) const
    {
        return (m_pipe_top.getGlobalBounds().intersects(object.getGlobalBounds())
                || m_pipe_bottom.getGlobalBounds().intersects(object.getGlobalBounds()));
    }

    uint32_t m_x = 0;
    uint32_t m_y = 0;
    sf::RectangleShape m_pipe_top;
    sf::RectangleShape m_pipe_bottom;
};

struct Fluffy
{
    Fluffy(double x, double y, uint32_t size)
        : m_x(x), m_y(y), m_size(size)
    {
        shape.setRadius(size);
        shape.setPosition(m_x, m_y);
    }

    bool is_alive() const
    {
        return m_is_alive;
    }

    void jump() {
        m_velocity += 2;
    }

    template<typename T>
    bool colide(T && object)
    {
        return shape.getGlobalBounds().intersects(object.getGlobalBounds());
    }

    void update()
    {
        if (m_velocity != -2) {
            m_velocity -= 0.1;
        }
        shape.move(sf::Vector2f(0.f, -m_velocity));
        auto y = shape.getPosition().y;
        if ((y < 0) ||  (y > _y)) {
            m_is_alive = false;
        }
    }

    double m_x = 0;
    double m_y = 0;
    uint32_t m_size = 0;
    double m_velocity = 0;
    sf::CircleShape shape;
    bool m_is_alive = true;
};

Pipe generate_pipe()
{
    static auto spawn_point = sf::Vector2f(_x - 24, 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, _x - FENCE_SIZE - 10);

    int t = dis(gen);

    Pipe pipe(20, t);

    pipe.m_pipe_top.setSize(sf::Vector2f(20, t));
    pipe.m_pipe_top.setPosition(spawn_point);
    pipe.m_pipe_top.setFillColor(sf::Color::Green);

    pipe.m_pipe_bottom.setSize(sf::Vector2f(20, _y  - t - FENCE_SIZE));
    pipe.m_pipe_bottom.setPosition({spawn_point.x, t + FENCE_SIZE});
    pipe.m_pipe_bottom.setFillColor(sf::Color::Red);

    return std::move(pipe);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(_x, _y), WINDOW_TITLE);
    window.setFramerateLimit(50);

    Fluffy fluff(20, _y / 2, 20);
    fluff.shape.setFillColor(sf::Color(255, 192, 203));

    /*sf::Sprite pipe;
    sf::Texture texture;
    if (!texture.loadFromFile("img/sprite.png", sf::IntRect(10, 10, 32, 32))) {
        std::cout << "err" << std::endl;
    }
    sf::Uint8* pixels = new sf::Uint8[40 * 40 * 4];
    texture.update(pixels);
    texture.setSmooth(true);
    //pipe.setTexture(texture);
    pipe.setColor(sf::Color(0, 255, 0));
    pipe.setPosition(sf::Vector2f(10.f, 50.f));*/


    std::vector<Pipe> pipes;
    pipes.emplace_back(std::move(generate_pipe()));
    auto counter = 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                fluff.jump();
            }
        }

        for (auto & p : pipes) {
            //if(p.shape.x > fluff.shape.x + fluff.m_size) {
            //    break;
            //}
            //if (fluff.colide(p)) {
            p.update();
            if (p.collide(fluff.shape)) {
                std::cout << "fluffy is dead :(" << std::endl;
            }
        }
        pipes.erase(
            std::remove_if(pipes.begin(), pipes.end(), [](auto p) {
                return !p.is_alive();
            })
        , pipes.end());
        /*if (!pipe.is_alive()) {
            pipe = generate_pipe();
        }*/
        if (!fluff.is_alive()) {
        }
        if (counter % 90 == 0) {
            pipes.emplace_back(std::move(generate_pipe()));
            counter = 0;
        }
        counter++;
        fluff.update();

        window.clear();
        std::for_each(pipes.begin(), pipes.end(), [&](const auto & p) {
            p.display(window);
        });
        //pipe.display(window);
        //window.draw(pipe.shape);
        window.draw(fluff.shape);
        window.display();
    }

    return 0;
}

