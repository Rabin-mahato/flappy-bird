#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include "HUD.hpp" // Include your HUD header

struct Pipe
{
    float x;
    float gapY;
    bool scored = false;
};

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;

    HUD hud(windowWidth, windowHeight); // Initialize HUD

    // Setup
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Flappy Bird SFML Clone");
    window.setFramerateLimit(60);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Load font (for Game Over text)
    sf::Font font;
    if (!font.loadFromFile("Roboto-Rectangular.tff"))
    {
        std::cerr << "Failed to load font Roboto-Rectangular.tff\n";
        return -1;
    }

    // Score variable
    int score = 0;

    // Load Textures
    sf::Texture birdTexture, pipeTexture, backgroundTexture;
    if (!birdTexture.loadFromFile("bird.png") ||
        !pipeTexture.loadFromFile("pipe.png") ||
        !backgroundTexture.loadFromFile("background.png"))
    {
        std::cerr << "Failed to load one or more textures.\n";
        return -1;
    }

    sf::Sprite background(backgroundTexture);

    sf::Sprite bird(birdTexture);
    bird.setPosition(100.f, 300.f);

    const float pipeWidth = pipeTexture.getSize().x;
    const float pipeGap = 150.f;
    const float pipeSpeed = 3.f;
    const int pipeCount = 5;
    const float pipeSpacing = 300.f;

    sf::Sprite topPipe(pipeTexture);
    sf::Sprite bottomPipe(pipeTexture);

    // Flip top pipe vertically
    topPipe.setScale(1.f, -1.f);

    // Pipes vector
    std::vector<Pipe> pipes;
    for (int i = 0; i < pipeCount; i++)
    {
        Pipe p;
        p.x = windowWidth + i * pipeSpacing;
        p.gapY = 150 + std::rand() % 300;
        p.scored = false;
        pipes.push_back(p);
    }

    // Bird physics variables
    float birdVelocity = 0.f;
    const float gravity = 900.f;
    const float flapStrength = -350.f;

    bool gameOver = false;
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
            {
                // Restart game
                pipes.clear();
                for (int i = 0; i < pipeCount; i++)
                {
                    Pipe p;
                    p.x = windowWidth + i * pipeSpacing;
                    p.gapY = 150 + std::rand() % 300;
                    p.scored = false;
                    pipes.push_back(p);
                }
                bird.setPosition(100.f, 300.f);
                birdVelocity = 0.f;
                score = 0;
                hud.updateScore(score);
                gameOver = false;
                clock.restart();
            }
        }

        float dt = clock.restart().asSeconds();

        if (!gameOver)
        {
            // Input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                birdVelocity = flapStrength;
            }

            // Gravity
            birdVelocity += gravity * dt;
            bird.move(0.f, birdVelocity * dt);

            // Move pipes
            for (auto &pipe : pipes)
            {
                pipe.x -= pipeSpeed;

                // Score logic
                if (!pipe.scored && pipe.x + pipeWidth < bird.getPosition().x)
                {
                    score++;
                    pipe.scored = true;
                    hud.updateScore(score); // Update score via HUD
                }

                // Recycle pipes
                if (pipe.x < -pipeWidth)
                {
                    pipe.x += pipeCount * pipeSpacing;
                    pipe.gapY = 150 + std::rand() % 300;
                    pipe.scored = false;
                }
            }

            // Collision detection
            sf::FloatRect birdBounds = bird.getGlobalBounds();

            if (bird.getPosition().y < 0 || bird.getPosition().y + birdBounds.height > window.getSize().y)
            {
                gameOver = true;
            }

            for (const auto &pipe : pipes)
            {
                topPipe.setPosition(pipe.x, pipe.gapY - pipeGap / 2.f);
                bottomPipe.setPosition(pipe.x, pipe.gapY + pipeGap / 2.f);

                if (birdBounds.intersects(topPipe.getGlobalBounds()) || birdBounds.intersects(bottomPipe.getGlobalBounds()))
                {
                    gameOver = true;
                    break;
                }
            }
        }

        // Draw everything
        window.clear();
        window.draw(background);

        for (const auto &pipe : pipes)
        {
            topPipe.setPosition(pipe.x, pipe.gapY - pipeGap / 2.f);
            bottomPipe.setPosition(pipe.x, pipe.gapY + pipeGap / 2.f);
            window.draw(topPipe);
            window.draw(bottomPipe);
        }

        window.draw(bird);

        // Draw score HUD
        hud.draw(window);

        if (gameOver)
        {
            sf::Text gameOverText("Game Over!\nPress Space to Restart", font, 40);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(150.f, 250.f);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
