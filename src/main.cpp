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

    void update()
    {
        shape.move(sf::Vector2f(-1.f, 0.f));
    }

    bool is_alive() const
    {
        auto pos = shape.getPosition();
        if ((pos.x < 0) || (pos.x + m_x > _x) || (pos.y < 0) || (pos.y + m_y > _y)) {
            std::cout << "dead x {" << pos.x << ":" << pos.x + m_x << "/" << _x << "} y {" << pos.y << ":" << pos.y + m_y << "/" << _y << "}" << std::endl;
            return false;
        }
        return true;
    }

    auto get_top_left() const
    {
        return shape.getPosition();
    }

    auto get_bottom_right() const
    {
        auto pos = shape.getPosition();
	pos.x += m_x;
	pos.y += m_y;
	return pos;
    }

    auto getGlobalBounds() const
    {
        return shape.getGlobalBounds();
    }

    uint32_t m_x = 0;
    uint32_t m_y = 0;
    sf::RectangleShape shape;
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
        if (m_velocity == -2) {
            return;
        }
        m_velocity -= 0.1;
        shape.move(sf::Vector2f(0.f, -m_velocity));
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, _x);

    auto t = dis(gen);
    auto spawn_point = sf::Vector2f(_x - 24, 0);
    Pipe pipe(20, t);
    pipe.shape.setSize(sf::Vector2f(20, t));
    pipe.shape.setPosition(spawn_point);
    pipe.shape.setFillColor(sf::Color::Green);

    return pipe;
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
    auto pipe = generate_pipe();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                pipe.shape.move(sf::Vector2f(-10.f, 0.f));
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                pipe.shape.move(sf::Vector2f(10.f, 0.f));
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                pipe.shape.move(sf::Vector2f(0.f, -10.f));
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                pipe.shape.move(sf::Vector2f(0.f, 10.f));
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                fluff.jump();
            }
        }

        for (const auto & p : pipes) {
            //if(p.shape.x > fluff.shape.x + fluff.m_size) {
            //    break;
            //}
            if (fluff.colide(p)) {
		std::cout << "fluffy is dead :(" << std::endl;
                break;
            }
        }
        if (!pipe.is_alive()) {
            pipe = generate_pipe();
        }
        pipe.update();
        fluff.update();

        window.clear();
        window.draw(pipe.shape);
        window.draw(fluff.shape);
        window.display();
    }

    return 0;
}

