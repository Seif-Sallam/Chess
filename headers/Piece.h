#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>

namespace PieceType
{
	const int King = 1,
			  Queen = 2,
			  Bishop = 4,
			  Knight = 8,
			  Rook = 16,
			  Pawn = 32,
			  White = 64;
}

class Piece
{
public:
	Piece(int type, sf::Texture &texture, int32_t row, int32_t column);

	void SetPosition(int32_t row, int32_t column);
	void SetPosition(sf::Vector2i pos);
	sf::Vector2i GetPosition();
	int GetIndex();
	bool Contains(const sf::Vector2i &pos);
	void Draw(sf::RenderWindow &window, sf::Shader *shader);
	int type;
	bool firstMove;
	std::string GetName();

private:
	int32_t m_Row, m_Column;
	sf::Sprite m_Sprite;
};
