#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "Piece.h"

class Board
{
public:
	Board(sf::Vector2f tileSize, sf::Vector2f offset);

	inline static sf::Vector2f GetOffset() { return m_Offset; }
	inline static sf::Vector2f GetTileSize() { return m_TileSize; }

	Piece *SelectPiece(sf::Event event, sf::RenderWindow &window);
	void UpdateTile(const sf::Vector2i &oldPos, const sf::Vector2i &newPos);
	void SetForeColor(const sf::Color &color);
	void SetBackColor(const sf::Color &color);

	void Draw(sf::RenderWindow &window, sf::Shader *shader);

private:
	void CreatePieces(const std::string &places);
	static sf::Vector2f m_Offset, m_TileSize;
	Piece *m_Pieces[64];
	int m_Squares[64];
	std::map<PieceType, sf::Shader *> m_Shaders;
	sf::Texture m_PiecesSpriteSheet;
	sf::Color m_ForeColor = sf::Color::White;
	sf::Color m_BackColor = sf::Color::Black;
	sf::Text m_Texts;
	sf::Font m_Font;
};
