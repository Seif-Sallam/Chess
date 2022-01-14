#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "Piece.h"
#include <list>

#define LOG(x) std::cout << x

class Board
{
public:
	Board(sf::Vector2f tileSize, sf::Vector2f offset, Piece *&currentPiece);

	inline static sf::Vector2f GetOffset() { return m_Offset; }
	inline static sf::Vector2f GetTileSize() { return m_TileSize; }

	Piece *SelectPiece(sf::Event event, sf::RenderWindow &window);
	void UpdateSelection(sf::Event &event, sf::RenderWindow &window);
	void UpdateTile(const sf::Vector2i &oldPos, const sf::Vector2i &newPos);
	void SetForeColor(const sf::Color &color);
	void SetBackColor(const sf::Color &color);

	void Draw(sf::RenderWindow &window, sf::Shader *shader);

private:
	void CreatePieces(const std::string &places);
	void GetPossibleMoves(Piece *piece);
	bool ValidPosition(const sf::Vector2i &position);
	static sf::Vector2f m_Offset, m_TileSize;
	Piece *&m_CurrentPiece;
	bool generated = false;
	Piece *m_Pieces[64];
	int m_Squares[64];
	std::list<int32_t> m_SelectionSquares;
	sf::Texture m_PiecesSpriteSheet;
	sf::Color m_ForeColor = sf::Color::White;
	sf::Color m_BackColor = sf::Color::Black;
	sf::Color m_SelectionColor = sf::Color(90, 90, 90, 255);
	sf::Text m_Text;
	sf::Font m_Font;
	std::vector<Piece *> m_AlivePieces;
	bool isMoving = false;
	int m_Row, m_Column;
	sf::Vector2i m_OldPosition, m_NewPosition;
	int m_SelectedIndex;
};

std::ostream &operator<<(std::ostream &stream, const sf::Vector2f &v);
std::ostream &operator<<(std::ostream &stream, const sf::Vector2i &v);
