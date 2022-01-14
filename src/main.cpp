#include <SFML/Graphics.hpp>
#include "SFML/Audio.hpp"
#include "Board.h"
#include "imgui.h"
#include "imgui-SFML.h"
int main()
{
    srand(time(0));
    sf::View view;
    sf::Vector2f tileSize = sf::Vector2f(64.0f, 64.0f);
    sf::RenderWindow window(sf::VideoMode(tileSize.x * 8, tileSize.y * 8), "Chess", sf::Style::Titlebar);
    Board board(tileSize);
    sf::Clock clock;
    board.SetBackColor(sf::Color(116, 77, 54));
    board.SetForeColor(sf::Color(174, 145, 125));
    bool wait = false;
    ImGui::SFML::Init(window);
    view = window.getView();
    int counter = 0;
    view.setCenter(sf::Vector2f(256, 256));
    while (window.isOpen())
    {
        sf::Event event;

        while (window.waitEvent(event))
        {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
            // Bad Shit, Do drag and drop
            board.UpdateSelection(event, window);

            {
                ImGui::SFML::Update(window, clock.restart());
            }
            window.clear(sf::Color(15, 15, 14));
            window.setView(view);

            board.Draw(window, nullptr);
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    return 0;
}