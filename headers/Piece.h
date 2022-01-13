#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>

enum class PieceType
{
	King = 1,
	Queen = 2,
	Bishop = 4,
	Knight = 8,
	Rook = 16,
	Pawn = 32,
	White = 64
};

class Piece
{
public:
	Piece(int type, sf::Texture &texture, int32_t row, int32_t column);

	void SetPosition(int32_t row, int32_t column);

	sf::Vector2i GetPosition();

	void Draw(sf::RenderWindow &window, sf::Shader *shader);
	int type;

private:
	int32_t m_Row, m_Column;
	sf::Sprite m_Sprite;
};
