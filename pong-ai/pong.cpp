#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

struct LeaderboardEntry {
    std::string name;
    float score;
};

void loadLeaderboard(std::vector<LeaderboardEntry>& leaderboard) {
    std::ifstream infile("leaderboard.txt");
    if (!infile) return;
    leaderboard.clear();
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        float score;
        std::string name;
        if (iss >> score) {
            std::getline(iss, name);
            if (!name.empty() && name[0] == ' ')
                name.erase(0, 1);
            leaderboard.push_back({ name, score });
        }
    }
}

void saveLeaderboard(const std::vector<LeaderboardEntry>& leaderboard) {
    std::ofstream outfile("leaderboard.txt");
    for (const auto& entry : leaderboard) {
        outfile << entry.score << " " << entry.name << "\n";
    }
}

enum class GameState { Menu, Playing, Win, Loss, NameEntry };

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Pong Game", sf::Style::Fullscreen);
    window.setFramerateLimit(240);
    sf::Font font;
    if (!font.loadFromFile(R"(C:\Users\lucas\Downloads\Arial.ttf)")) {
        std::cerr << "Error: Could not load font 'Arial.ttf'" << std::endl;
        return -1;
    }
    std::vector<LeaderboardEntry> leaderboard;
    loadLeaderboard(leaderboard);

    sf::Text titleText("Pong Game", font, 80);
    titleText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = titleText.getLocalBounds();
        titleText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        titleText.setPosition(1920 / 2, 100);
    }

    sf::RectangleShape pvpButton(sf::Vector2f(400, 100));
    pvpButton.setFillColor(sf::Color(0, 150, 0));
    pvpButton.setOutlineColor(sf::Color::White);
    pvpButton.setOutlineThickness(5);
    pvpButton.setPosition(1920 / 2 - 200, 200);

    sf::Text pvpButtonText("Player vs Player", font, 40);
    pvpButtonText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = pvpButtonText.getLocalBounds();
        pvpButtonText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        pvpButtonText.setPosition(pvpButton.getPosition().x + pvpButton.getSize().x / 2,
            pvpButton.getPosition().y + pvpButton.getSize().y / 2);
    }

    sf::RectangleShape aiButton(sf::Vector2f(400, 100));
    aiButton.setFillColor(sf::Color(0, 150, 0));
    aiButton.setOutlineColor(sf::Color::White);
    aiButton.setOutlineThickness(5);
    aiButton.setPosition(1920 / 2 - 200, 350);

    sf::Text aiButtonText("Player vs AI", font, 40);
    aiButtonText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = aiButtonText.getLocalBounds();
        aiButtonText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        aiButtonText.setPosition(aiButton.getPosition().x + aiButton.getSize().x / 2,
            aiButton.getPosition().y + aiButton.getSize().y / 2);
    }

    sf::RectangleShape exitButton(sf::Vector2f(400, 100));
    exitButton.setFillColor(sf::Color(150, 0, 0));
    exitButton.setOutlineColor(sf::Color::White);
    exitButton.setOutlineThickness(5);
    exitButton.setPosition(1920 / 2 - 200, 500);

    sf::Text exitButtonText("Exit", font, 40);
    exitButtonText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = exitButtonText.getLocalBounds();
        exitButtonText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        exitButtonText.setPosition(exitButton.getPosition().x + exitButton.getSize().x / 2,
            exitButton.getPosition().y + exitButton.getSize().y / 2);
    }

    sf::RectangleShape leaderboardBox(sf::Vector2f(500, 400));
    leaderboardBox.setFillColor(sf::Color::Transparent);
    leaderboardBox.setOutlineColor(sf::Color::White);
    leaderboardBox.setOutlineThickness(3);
    leaderboardBox.setPosition(30, 100);

    sf::Text leaderboardDisplay("Leaderboard:\n", font, 30);
    leaderboardDisplay.setFillColor(sf::Color::White);
    leaderboardDisplay.setPosition(50, 120);

    sf::RectangleShape clearButton(sf::Vector2f(200, 50));
    clearButton.setFillColor(sf::Color(150, 0, 0));
    clearButton.setOutlineColor(sf::Color::White);
    clearButton.setOutlineThickness(3);
    clearButton.setPosition(30 + (500 - 200) / 2, 100 + 400 - 60);

    sf::Text clearButtonText("Clear", font, 30);
    clearButtonText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = clearButtonText.getLocalBounds();
        clearButtonText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        clearButtonText.setPosition(clearButton.getPosition().x + clearButton.getSize().x / 2,
            clearButton.getPosition().y + clearButton.getSize().y / 2);
    }

    sf::Text winText("You Win!", font, 80);
    winText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = winText.getLocalBounds();
        winText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        winText.setPosition(1920 / 2, 500);
    }

    sf::Text lossText("You Lose!", font, 80);
    lossText.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = lossText.getLocalBounds();
        lossText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        lossText.setPosition(1920 / 2, 500);
    }

    sf::Text menuPrompt("Press M to return to Menu", font, 40);
    menuPrompt.setFillColor(sf::Color::White);
    {
        sf::FloatRect bounds = menuPrompt.getLocalBounds();
        menuPrompt.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        menuPrompt.setPosition(1920 / 2, 700);
    }

    sf::Text nameEntryPrompt("Enter your name: ", font, 50);
    nameEntryPrompt.setFillColor(sf::Color::White);
    nameEntryPrompt.setPosition(1920 / 2 - 300, 400);

    sf::Text nameEntryText("", font, 50);
    nameEntryText.setFillColor(sf::Color::White);
    nameEntryText.setPosition(1920 / 2 - 300, 500);

    sf::Text scoreText("Score: 0", font, 40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(1920 - 300, 50);

    sf::RectangleShape gameBackground(sf::Vector2f(1920, 1080));
    gameBackground.setFillColor(sf::Color(10, 10, 10));

    sf::RectangleShape paddle1(sf::Vector2f(30, 200));
    paddle1.setFillColor(sf::Color::Green);
    paddle1.setPosition(50, 1080 / 2 - 100);

    sf::RectangleShape paddle2(sf::Vector2f(30, 200));
    paddle2.setFillColor(sf::Color::Green);
    paddle2.setPosition(1920 - 50 - 30, 1080 / 2 - 100);

    sf::CircleShape ball(20);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition(1920 / 2 - 20, 1080 / 2 - 20);

    float ballVelocityX = -3.f;
    float ballVelocityY = 3.f;

    sf::Clock multiplierClock;
    float nextMultiplierTime = 20.f;
    int multiplierCount = 0;
    bool showMultiplierText = false;
    sf::Clock multiplierDisplayClock;
    const float multiplierDisplayDuration = 2.0f;
    sf::Text multiplierText("MULTIPLIER: 1.0X", font, 50);
    multiplierText.setFillColor(sf::Color::Green);
    {
        sf::FloatRect bounds = multiplierText.getLocalBounds();
        multiplierText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        multiplierText.setPosition(1920 / 2, 200);
    }

    GameState state = GameState::Menu;
    bool vsAI = false;

    sf::Clock gameClock;
    float survivalScore = 0.0f;
    std::string playerNameInput = "";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (state == GameState::Menu) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                    if (pvpButton.getGlobalBounds().contains(mousePosF)) {
                        vsAI = false;
                        state = GameState::Playing;
                        gameClock.restart();
                        multiplierClock.restart();
                    }
                    else if (aiButton.getGlobalBounds().contains(mousePosF)) {
                        vsAI = true;
                        state = GameState::Playing;
                        gameClock.restart();
                        multiplierClock.restart();
                    }
                    else if (exitButton.getGlobalBounds().contains(mousePosF)) {
                        window.close();
                    }
                    if (clearButton.getGlobalBounds().contains(mousePosF)) {
                        leaderboard.clear();
                        saveLeaderboard(leaderboard);
                    }
                }
            }
            else if (state == GameState::NameEntry) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b') {
                        if (!playerNameInput.empty())
                            playerNameInput.pop_back();
                    }
                    else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                        if (playerNameInput.empty())
                            playerNameInput = "Anonymous";
                        leaderboard.push_back({ playerNameInput, survivalScore });
                        saveLeaderboard(leaderboard);
                        paddle1.setPosition(50, 1080 / 2 - 100);
                        paddle2.setPosition(1920 - 50 - 30, 1080 / 2 - 100);
                        ball.setPosition(1920 / 2 - 20, 1080 / 2 - 20);
                        ballVelocityX = -3.f;
                        ballVelocityY = 3.f;
                        multiplierClock.restart();
                        nextMultiplierTime = 20.f;
                        multiplierCount = 0;
                        state = GameState::Menu;
                    }
                    else {
                        if (event.text.unicode < 128 && event.text.unicode >= 32) {
                            playerNameInput += static_cast<char>(event.text.unicode);
                        }
                    }
                    nameEntryText.setString(playerNameInput);
                }
            }
            else if (state == GameState::Win || state == GameState::Loss) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
                    state = GameState::Menu;
                    paddle1.setPosition(50, 1080 / 2 - 100);
                    paddle2.setPosition(1920 - 50 - 30, 1080 / 2 - 100);
                    ball.setPosition(1920 / 2 - 20, 1080 / 2 - 20);
                    ballVelocityX = -3.f;
                    ballVelocityY = 3.f;
                    multiplierClock.restart();
                    nextMultiplierTime = 20.f;
                    multiplierCount = 0;
                }
            }
        }

        if (state == GameState::Playing) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (paddle1.getPosition().y > 0)
                    paddle1.move(0, -5);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                if (paddle1.getPosition().y + paddle1.getSize().y < 1080)
                    paddle1.move(0, 5);
            }
            if (!vsAI) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    if (paddle2.getPosition().y > 0)
                        paddle2.move(0, -5);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    if (paddle2.getPosition().y + paddle2.getSize().y < 1080)
                        paddle2.move(0, 5);
                }
            }
            else {
                if (ballVelocityX > 0) {
                    float targetX = paddle2.getPosition().x;
                    float timeToReach = (targetX - ball.getPosition().x) / ballVelocityX;
                    float predictedY = ball.getPosition().y + ballVelocityY * timeToReach;
                    while (predictedY < 0 || predictedY > 1080) {
                        if (predictedY < 0)
                            predictedY = -predictedY;
                        else if (predictedY > 1080)
                            predictedY = 2 * 1080 - predictedY;
                    }
                    float desiredY = predictedY - paddle2.getSize().y / 2;
                    float currentY = paddle2.getPosition().y;
                    float diff = desiredY - currentY;
                    float aiSpeed = 8.f;
                    if (std::abs(diff) < aiSpeed)
                        paddle2.setPosition(paddle2.getPosition().x, desiredY);
                    else
                        paddle2.move(0, (diff > 0 ? aiSpeed : -aiSpeed));
                }
                else {
                    float desiredY = 1080 / 2 - paddle2.getSize().y / 2;
                    float diff = desiredY - paddle2.getPosition().y;
                    float aiSpeed = 8.f;
                    if (std::abs(diff) < aiSpeed)
                        paddle2.setPosition(paddle2.getPosition().x, desiredY);
                    else
                        paddle2.move(0, (diff > 0 ? aiSpeed : -aiSpeed));
                }
            }

            ball.move(ballVelocityX, ballVelocityY);

            if (ball.getPosition().y <= 0 || ball.getPosition().y + ball.getRadius() * 2 >= 1080) {
                ballVelocityY = -ballVelocityY;
            }

            if (ball.getGlobalBounds().intersects(paddle1.getGlobalBounds())) {
                ballVelocityX = std::abs(ballVelocityX);
            }
            if (ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) {
                ballVelocityX = -std::abs(ballVelocityX);
            }

            if (multiplierClock.getElapsedTime().asSeconds() >= nextMultiplierTime) {
                float eventFactor = 1.1f;
                ballVelocityX *= eventFactor;
                ballVelocityY *= eventFactor;
                multiplierCount++;
                float cumulativeMultiplier = std::pow(1.1f, multiplierCount);
                nextMultiplierTime += (multiplierCount < 2 ? 10.f : 15.f);
                showMultiplierText = true;
                multiplierDisplayClock.restart();
                std::ostringstream oss;
                oss << "MULTIPLIER: " << std::fixed << std::setprecision(2) << cumulativeMultiplier << "X";
                multiplierText.setString(oss.str());
                sf::FloatRect bounds = multiplierText.getLocalBounds();
                multiplierText.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
                multiplierText.setPosition(1920 / 2, 200);
            }

            if (!vsAI) {
                if (ball.getPosition().x < 0) {
                    state = GameState::Loss;
                }
                if (ball.getPosition().x > 1920) {
                    state = GameState::Win;
                }
            }
            else {
                if (ball.getPosition().x < 0) {
                    survivalScore = gameClock.getElapsedTime().asSeconds() * 100;
                    state = GameState::NameEntry;
                    playerNameInput = "";
                }
                float currentScore = gameClock.getElapsedTime().asSeconds() * 100;
                std::ostringstream scoreStream;
                scoreStream << "Score: " << static_cast<int>(currentScore);
                scoreText.setString(scoreStream.str());
            }
        }

        window.clear(sf::Color::Black);
        if (state == GameState::Menu) {
            window.draw(titleText);
            window.draw(pvpButton);
            window.draw(pvpButtonText);
            window.draw(aiButton);
            window.draw(aiButtonText);
            window.draw(exitButton);
            window.draw(exitButtonText);
            std::string lbStr = "Leaderboard:\n";
            std::vector<LeaderboardEntry> sortedLB = leaderboard;
            std::sort(sortedLB.begin(), sortedLB.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                return a.score > b.score;
                });
            int count = 1;
            for (auto& entry : sortedLB) {
                lbStr += std::to_string(count) + ". " + entry.name + " - " + std::to_string(static_cast<int>(entry.score)) + "\n";
                if (++count > 10) break;
            }
            leaderboardDisplay.setString(lbStr);
            window.draw(leaderboardBox);
            window.draw(leaderboardDisplay);
            window.draw(clearButton);
            window.draw(clearButtonText);
        }
        else if (state == GameState::Playing) {
            window.draw(gameBackground);
            window.draw(paddle1);
            window.draw(paddle2);
            window.draw(ball);
            window.draw(scoreText);
            if (showMultiplierText) {
                float elapsed = multiplierDisplayClock.getElapsedTime().asSeconds();
                if (elapsed > multiplierDisplayDuration) {
                    showMultiplierText = false;
                }
                else {
                    float scaleFactor = 1.0f + 0.5f * sin(elapsed * 10);
                    multiplierText.setScale(scaleFactor, scaleFactor);
                    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1 - elapsed / multiplierDisplayDuration));
                    sf::Color c = multiplierText.getFillColor();
                    multiplierText.setFillColor(sf::Color(c.r, c.g, c.b, alpha));
                    window.draw(multiplierText);
                }
            }
        }
        else if (state == GameState::Win) {
            window.draw(winText);
            window.draw(menuPrompt);
        }
        else if (state == GameState::Loss) {
            window.draw(lossText);
            window.draw(menuPrompt);
        }
        else if (state == GameState::NameEntry) {
            sf::Text finalScoreText("Your score: " + std::to_string(static_cast<int>(survivalScore)), font, 50);
            finalScoreText.setFillColor(sf::Color::White);
            finalScoreText.setPosition(1920 / 2 - 300, 300);
            window.draw(finalScoreText);
            window.draw(nameEntryPrompt);
            window.draw(nameEntryText);
        }
        window.display();
    }
    return 0;
}
