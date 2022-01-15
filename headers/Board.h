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
	Board(sf::Vector2f tileSize);

	inline static sf::Vector2f GetTileSize() { return m_TileSize; }

	void UpdateSelection(sf::Event &event, sf::RenderWindow &window);
	void UpdateTile(const sf::Vector2i &oldPos, const sf::Vector2i &newPos);
	void SetForeColor(const sf::Color &color);
	void SetBackColor(const sf::Color &color);

	void Draw(sf::RenderWindow &window, sf::Shader *shader);

private:
	bool MoveCastlingWhite();
	bool MoveCastlingBlack();
	void MoveNormally();

	bool IsCastlingMove(bool isWhite);

	bool SelectableIndex(sf::Vector2i position);
	void CreatePieces(const std::string &places);
	void GetPossibleMoves(Piece *piece);
	bool ValidPosition(const sf::Vector2i &position);

	void PossibleMovesRook(bool isWhite, int index, std::list<int32_t> *list = nullptr);
	void PossibleMovesPawn(bool isWhite, int index, bool capturesOnly = false, std::list<int32_t> *list = nullptr);
	void PossibleMovesBishop(bool isWhite, int index, std::list<int32_t> *list = nullptr);
	void PossibleMovesQueen(bool isWhite, int index, std::list<int32_t> *list = nullptr);
	void PossibleMovesKnight(bool isWhite, int index, std::list<int32_t> *list = nullptr);
	void PossibleMovesKing(bool isWhite, int index, std::list<int32_t> *list = nullptr);
	void AddCastlingMoves(bool isWhite, int index, std::list<int32_t> *list = nullptr);

	void UpdateKingsCheck();
	bool CanPieceAttack(int32_t kingIndex, Piece *currentPiece, std::list<int32_t> *list = nullptr);
	bool CanCasle(bool isWhite, bool queenSide);
	bool SameColor(int index1, int index2);

	bool IsOneOfEight(sf::Vector2i &pos, int index);
	bool GoodMoveForKing(bool isWhite, int32_t newIndex);

	static sf::Vector2f m_TileSize;
	Piece *m_Pieces[64];
	std::vector<Piece *> m_AlivePieces;
	std::list<int32_t> m_SelectionSquares;
	sf::Texture m_PiecesSpriteSheet;
	sf::Color m_ForeColor = sf::Color::White;
	sf::Color m_BackColor = sf::Color::Black;
	sf::Color m_SelectionColor = sf::Color(90, 90, 90, 255);
	sf::Text m_Text;
	sf::Font m_Font;

	sf::Vector2i m_OldPosition, m_NewPosition;
	int m_SelectedIndex;
	bool m_WhiteTurn = true;

	bool m_BlackCasleQueen;
	bool m_BlackCasleKing;
	bool m_WhiteCasleQueen;
	bool m_WhiteCasleKing;

	bool m_WhiteInCheck;
	bool m_BlackInCheck;

	Piece *m_BlackKing;
	Piece *m_WhiteKing;
	const std::pair<sf::Vector2i, sf::Vector2i> m_BlackCastleQueenMove;
	const std::pair<sf::Vector2i, sf::Vector2i> m_BlackCastleKingMove;
	const std::pair<sf::Vector2i, sf::Vector2i> m_WhiteCastleQueenMove;
	const std::pair<sf::Vector2i, sf::Vector2i> m_WhiteCastleKingMove;
};

std::ostream &operator<<(std::ostream &stream, const sf::Vector2f &v);
std::ostream &operator<<(std::ostream &stream, const sf::Vector2i &v);
