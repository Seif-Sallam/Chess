#include <SFML/Graphics.hpp>
#include "SFML/Audio.hpp"
#include "Board.h"
#include "imgui.h"
#include "imgui-SFML.h"
int main()
{
    srand(time(0));
    sf::View view;
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    Board board(sf::Vector2f(64.0f, 64.0f), sf::Vector2f(150.0f, 60.0f));
    Piece *piece = nullptr;
    sf::Clock clock;
    board.SetBackColor(sf::Color(116, 77, 54));
    board.SetForeColor(sf::Color(174, 145, 125));
    bool wait = false;
    ImGui::SFML::Init(window);
    view = window.getView();
    int counter = 0;
    while (window.isOpen())
    {
        sf::Event event;

        while (window.waitEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized)
            {
                view.setSize(sf::Vector2f(window.getSize()));
                view.setCenter(sf::Vector2f(64 * 4 + 150.0f, 64 * 4 + 64));
            }
            // Bad Shit, Do drag and drop
            if (piece == nullptr && counter > 5)
            {
                piece = board.SelectPiece(event, window);
                if (piece)
                    counter = 0;
            }
            else if (piece != nullptr && counter == 0)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    auto oldPos = piece->GetPosition();
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    int row = 0, column = 0;
                    row = (mousePos.y - Board::GetOffset().y) / Board::GetOffset().y;
                    column = (mousePos.x - Board::GetOffset().x) / Board::GetTileSize().x;
                    row = 7 - row;
                    std::cout << "Old index: " << oldPos.x * 8 + oldPos.y << std::endl;
                    std::cout << "Gient If: " << row << ", " << column << std::endl;
                    int index = row * 8 + column;
                    std::cout << "Index: " << index << std::endl;
                    if (index > -1 && index < 64)
                    {
                        piece->SetPosition(row, column);
                        auto newPos = piece->GetPosition();
                        board.UpdateTile(oldPos, newPos);
                        piece = nullptr;
                        std::cout << "Not selectedf any moreeee\n";
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left ||
                     event.mouseButton.button == sf::Mouse::Button::Right)
            {
                std::cout << "not selected anymore\n";
                piece = nullptr;
            }
            else
            {
                counter++;
            }
            {
                ImGui::SFML::Update(window, clock.restart());
            }

            {
                window.clear(sf::Color(15, 15, 14));

                window.setView(view);
                board.Draw(window, nullptr);
                ImGui::SFML::Render(window);
                window.display();
            }
        }
    }

    return 0;
}