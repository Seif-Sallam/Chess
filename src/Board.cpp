#include "Board.h"
#include <math.h>
#include <iomanip>

sf::Vector2f Board::m_TileSize;

Board::Board(sf::Vector2f tileSize)
	: m_WhiteCastleKingMove(std::make_pair(sf::Vector2i(7, 6), sf::Vector2i(7, 5))),
	  m_WhiteCastleQueenMove(std::make_pair(sf::Vector2i(7, 2), sf::Vector2i(7, 3))),
	  m_BlackCastleKingMove(std::make_pair(sf::Vector2i(0, 6), sf::Vector2i(0, 5))),
	  m_BlackCastleQueenMove(std::make_pair(sf::Vector2i(0, 2), sf::Vector2i(0, 3)))
{
	m_TileSize = tileSize;
	m_PiecesSpriteSheet.loadFromFile("rsc/ChessPieces.png");
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int index = (7 - file) * 8 + rank;
			m_Pieces[index] = nullptr;
		}
	}

	std::string piecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	// std::string piecePlacement = "RRRRRRR/8/8/8/8/8/8/8";
	CreatePieces(piecePlacement);
	m_Font.loadFromFile("rsc/Roboto.ttf");
	m_Text.setCharacterSize(13);
	m_Text.setFont(m_Font);
	m_BlackCasleQueen = true;
	m_BlackCasleKing = true;
	m_WhiteCasleQueen = true;
	m_WhiteCasleKing = true;
}

void Board::UpdateSelection(sf::Event &event, sf::RenderWindow &window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			for (int i = 0; i < m_AlivePieces.size(); i++)
			{
				if (m_AlivePieces[i]->Contains(mousePos))
				{
					bool isWhite = m_AlivePieces[i]->type & PieceType::White;
					if (isWhite == m_WhiteTurn)
					{
						m_SelectedIndex = i;
						m_OldPosition = m_AlivePieces[i]->GetPosition();
						GetPossibleMoves(m_AlivePieces[i]);
						break;
					}
				}
			}
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			m_NewPosition = sf::Vector2i(mousePos.y / m_TileSize.y, mousePos.x / m_TileSize.x);
			m_NewPosition.x = m_NewPosition.x;
			if (SelectableIndex(m_NewPosition))
			{
				Piece *&piece = m_AlivePieces[m_SelectedIndex];
				int type = piece->type;
				bool isWhite = type & PieceType::White;
				bool castlingMove = IsCastlingMove(isWhite);

				if (castlingMove)
				{
					if (isWhite)
						MoveCastlingWhite();
					else
						MoveCastlingBlack();
				}
				else
					MoveNormally();
				m_WhiteTurn = !m_WhiteTurn;
			}
			m_SelectionSquares.clear();
			UpdateKingsCheck();
			if (m_BlackInCheck)
				std::cout << "Black king is in Check\n";
			if (m_WhiteInCheck)
				std::cout << "White king is in Check\n";
		}
	}
}

bool Board::ValidPosition(const sf::Vector2i &position)
{
	return (position.x < 8 && position.x > -1 && position.y < 8 && position.y > -1);
}

void Board::UpdateTile(const sf::Vector2i &oldPos, const sf::Vector2i &newPos)
{

	int index1 = oldPos.x * 8 + oldPos.y;
	int index2 = newPos.x * 8 + newPos.y;
	auto &piece1 = m_Pieces[index1];
	auto &piece2 = m_Pieces[index2];

	if (piece1 != piece2)
	{
		if (piece2 != nullptr)
		{
			for (int i = 0; i < m_AlivePieces.size(); i++)
			{
				if (m_AlivePieces[i] == piece2)
				{
					m_AlivePieces.erase(m_AlivePieces.begin() + i);
					break;
				}
			}
			delete piece2;
		}
		m_Pieces[index2] = m_Pieces[index1];
		m_Pieces[index1] = nullptr;
	}
}

void Board::Draw(sf::RenderWindow &window, sf::Shader *tilesShader)
{
	//Drawing the Tiles
	static sf::RectangleShape shape(m_TileSize);
	static sf::RectangleShape selectShape(m_TileSize);
	selectShape.setFillColor(sf::Color(255, 15, 15, 125));
	sf::RenderStates state;

	state.blendMode = sf::BlendAlpha;
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			shape.setPosition(m_TileSize.x * rank, m_TileSize.y * file);
			int index = file * 8 + rank;
			if (std::find(m_SelectionSquares.begin(), m_SelectionSquares.end(), index) != m_SelectionSquares.end())
				shape.setFillColor(m_SelectionColor);
			else
				shape.setFillColor(((file + rank) % 2 == 0) ? m_ForeColor : m_BackColor);
			window.draw(shape);
		}
	}

	for (auto &item : m_SelectionSquares)
	{
		int index = item;
		int row = index / 8;
		int column = index % 8;
		selectShape.setPosition(column * m_TileSize.x, m_TileSize.y * row);
		window.draw(selectShape, state);
	}

	//Drawing the pieces

	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int index = file * 8 + rank;
			if (m_Pieces[index] != nullptr)
			{
				m_Pieces[index]->Draw(window, nullptr);
			}
		}
	}
	for (int file = 0; file < 8; file++)
	{
		std::string str;
		str.resize(1);
		str[0] = char(file + 'a');
		m_Text.setString(str);
		m_Text.setPosition(m_TileSize.x * file + 5.f, m_TileSize.y * 8 - 15.0f);
		m_Text.setFillColor((file % 2) ? m_BackColor : m_ForeColor);
		window.draw(m_Text);
	}
	for (int rank = 0; rank < 8; rank++)
	{
		std::string str;
		str.resize(1);
		str[0] = char('8' - rank);
		m_Text.setString(str);
		m_Text.setPosition(3.0f, m_TileSize.y * rank + 5.f);
		m_Text.setFillColor((rank % 2) ? m_ForeColor : m_BackColor);
		window.draw(m_Text);
	}
}

void Board::SetForeColor(const sf::Color &color)
{
	m_ForeColor = color;
}

void Board::SetBackColor(const sf::Color &color)
{
	m_BackColor = color;
}

void Board::CreatePieces(const std::string &places)
{
	int rank = 7;
	int file = 0;
	for (int i = 0; i < places.size(); i++)
	{
		file = file % 8;
		if (places[i] == '/')
		{
			rank -= 1;
		}
		else if (isdigit(places[i]))
		{
			int num = places[i] - '0';
			file += num;
			continue;
		}
		else
		{
			int type = 0;
			char pieceChar = tolower(places[i]);
			switch (pieceChar)
			{
			case 'r':
				type = int(PieceType::Rook);
				break;
			case 'n':
				type = int(PieceType::Knight);
				break;
			case 'b':
				type = int(PieceType::Bishop);
				break;
			case 'q':
				type = int(PieceType::Queen);
				break;
			case 'k':
				type = int(PieceType::King);
				break;
			case 'p':
				type = int(PieceType::Pawn);
				break;
			}
			if (pieceChar != places[i])
				type = type | int(PieceType::White);
			int index = (7 - rank) * 8 + file;
			m_Pieces[index] = new Piece(type, m_PiecesSpriteSheet, 7 - rank, file);

			if (type & PieceType::King)
			{
				if (type & PieceType::White)
					m_WhiteKing = m_Pieces[index];
				else
					m_BlackKing = m_Pieces[index];
			}

			m_AlivePieces.push_back(m_Pieces[index]);
			file++;
			continue;
		}
	}
}

void Board::GetPossibleMoves(Piece *piece)
{
	int type = piece->type;
	auto pos = piece->GetPosition();
	int index = pos.x * 8 + pos.y;
	bool isWhite = type & PieceType::White;

	if (type & PieceType::Pawn)
		PossibleMovesPawn(isWhite, index);
	else if (type & PieceType::Rook)
		PossibleMovesRook(isWhite, index);
	else if (type & PieceType::Bishop)
		PossibleMovesBishop(isWhite, index);
	else if (type & PieceType::Queen)
		PossibleMovesQueen(isWhite, index);
	else if (type & PieceType::Knight)
		PossibleMovesKnight(isWhite, index);
	else if (type & PieceType::King)
		PossibleMovesKing(isWhite, index);
}

bool Board::SelectableIndex(sf::Vector2i position)
{
	int index = position.x * 8 + position.y;
	if (std::find(m_SelectionSquares.begin(), m_SelectionSquares.end(), index) != m_SelectionSquares.end())
		return true;

	return false;
}

void Board::PossibleMovesPawn(bool isWhite, int index, bool capturesOnly)
{
	Piece *piece = m_Pieces[index];
	if (isWhite) // it is a whtie piece
	{
		sf::Vector2i pos = piece->GetPosition();
		if (!capturesOnly)
		{
			int index2 = (pos.x - 1) * 8 + pos.y;
			bool added = false;
			if (ValidPosition(sf::Vector2i((pos.x - 1), pos.y)))
				if (m_Pieces[index2] == nullptr)
				{
					m_SelectionSquares.push_back(index2);
					added = true;
				}
			index2 = (pos.x - 2) * 8 + pos.y;
			if (ValidPosition(sf::Vector2i((pos.x - 2), pos.y)))
				if (piece->firstMove && added)
					if (m_Pieces[index2] == nullptr)
						m_SelectionSquares.push_back(index2);
		}
		//Captures
		{
			sf::Vector2i pos2 = piece->GetPosition();
			pos2.x -= 1;
			pos2.y -= 1;
			int index3 = pos2.x * 8 + pos2.y;
			if (ValidPosition(pos2))
				if (m_Pieces[index3])
					if (!SameColor(index3, index))
					{
						m_SelectionSquares.push_back(index3);
					}
		}
		{
			sf::Vector2i pos2 = piece->GetPosition();
			pos2.x -= 1;
			pos2.y += 1;
			int index3 = pos2.x * 8 + pos2.y;
			if (ValidPosition(pos2))
				if (m_Pieces[index3])
					if (!SameColor(index3, index))
					{
						m_SelectionSquares.push_back(index3);
					}
		}
	}
	else
	{
		sf::Vector2i pos = piece->GetPosition();
		if (!capturesOnly)
		{
			int index2 = (pos.x + 1) * 8 + pos.y;
			bool added = false;
			if (ValidPosition(sf::Vector2i((pos.x + 1), pos.y)))
				if (m_Pieces[index2] == nullptr)
				{
					m_SelectionSquares.push_back(index2);
					added = true;
				}
			index2 = (pos.x + 2) * 8 + pos.y;
			if (ValidPosition(sf::Vector2i((pos.x + 2), pos.y)))
				if (piece->firstMove && added)
					if (m_Pieces[index2] == nullptr)
						m_SelectionSquares.push_back(index2);
		}
		//Captures
		{
			sf::Vector2i pos2 = piece->GetPosition();
			pos2.x += 1;
			pos2.y -= 1;
			int index3 = pos2.x * 8 + pos2.y;
			if (ValidPosition(pos2))
				if (m_Pieces[index3])
					if (!SameColor(index3, index))
					{
						m_SelectionSquares.push_back(index3);
					}
		}
		{
			sf::Vector2i pos2 = piece->GetPosition();
			pos2.x += 1;
			pos2.y += 1;
			int index3 = pos2.x * 8 + pos2.y;
			if (ValidPosition(pos2))
				if (m_Pieces[index3])
					if (!SameColor(index3, index))
					{
						m_SelectionSquares.push_back(index3);
					}
		}
	}
}

void Board::PossibleMovesRook(bool isWhite, int index)
{
	sf::Vector2i pos = m_Pieces[index]->GetPosition();

	int counterblockedY = 0;
	for (int i = pos.x - 1; i > -1; i--)
	{
		int index2 = i * 8 + pos.y;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}
	for (int i = pos.x + 1; i < 8; i++)
	{
		int index2 = i * 8 + pos.y;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}
	for (int i = pos.y - 1; i > -1; i--)
	{
		int index2 = pos.x * 8 + i;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}
	for (int i = pos.y + 1; i < 8; i++)
	{
		int index2 = pos.x * 8 + i;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}
}

void Board::PossibleMovesBishop(bool isWhite, int index)
{
	sf::Vector2i pos = m_Pieces[index]->GetPosition();

	for (int i = pos.x, j = pos.y; i < 8 && j < 8; i++, j++)
	{
		int index2 = i * 8 + j;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}

	for (int i = pos.x, j = pos.y; i >= 0 && j >= 0; i--, j--)
	{
		int index2 = i * 8 + j;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}

	for (int i = pos.x, j = pos.y; i < 8 && j >= 0; i++, j--)
	{
		int index2 = i * 8 + j;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}

	for (int i = pos.x, j = pos.y; i >= 0 && j < 8; i--, j++)
	{
		int index2 = i * 8 + j;
		if (index2 != index)
		{
			if (m_Pieces[index2] == nullptr)
				m_SelectionSquares.push_back(index2);
			else
			{
				if (!SameColor(index2, index))
					m_SelectionSquares.push_back(index2);
				break;
			}
		}
	}
}

void Board::PossibleMovesQueen(bool isWhite, int index)
{
	PossibleMovesRook(isWhite, index);
	PossibleMovesBishop(isWhite, index);
}

void Board::PossibleMovesKnight(bool isWhite, int index)
{
	sf::Vector2i pos = m_Pieces[index]->GetPosition();
	int index1 = (pos.x - 2) * 8 + pos.y + 1;
	if (ValidPosition(sf::Vector2i(pos.x - 2, pos.y + 1)))
	{
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);
	}

	index1 = (pos.x - 1) * 8 + pos.y + 2;
	if (ValidPosition(sf::Vector2i(pos.x - 1, pos.y + 2)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x - 2) * 8 + pos.y - 1;
	if (ValidPosition(sf::Vector2i(pos.x - 2, pos.y - 1)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x - 1) * 8 + pos.y - 2;
	if (ValidPosition(sf::Vector2i(pos.x - 1, pos.y - 2)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x + 2) * 8 + pos.y + 1;
	if (ValidPosition(sf::Vector2i(pos.x + 2, pos.y + 1)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x + 1) * 8 + pos.y + 2;
	if (ValidPosition(sf::Vector2i(pos.x + 1, pos.y + 2)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x + 2) * 8 + pos.y - 1;
	if (ValidPosition(sf::Vector2i(pos.x + 2, pos.y - 1)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);

	index1 = (pos.x + 1) * 8 + pos.y - 2;
	if (ValidPosition(sf::Vector2i(pos.x + 1, pos.y - 2)))
		if (m_Pieces[index1] == nullptr || !SameColor(index1, index))
			m_SelectionSquares.push_back(index1);
}

void Board::PossibleMovesKing(bool isWhite, int index2)
{
	auto GetIndex = [](sf::Vector2i v)
	{ return v.x * 8 + v.y; };
	sf::Vector2i pos = m_Pieces[index2]->GetPosition();
	if (ValidPosition(sf::Vector2i(pos.x - 1, pos.y - 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x - 1, pos.y - 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}

	if (ValidPosition(sf::Vector2i(pos.x + 1, pos.y + 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x + 1, pos.y + 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}

	if (ValidPosition(sf::Vector2i(pos.x - 1, pos.y + 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x - 1, pos.y + 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}

	if (ValidPosition(sf::Vector2i(pos.x + 1, pos.y - 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x + 1, pos.y - 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}

	if (ValidPosition(sf::Vector2i(pos.x + 1, pos.y)))
	{
		int index = GetIndex(sf::Vector2i(pos.x + 1, pos.y));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}
	if (ValidPosition(sf::Vector2i(pos.x - 1, pos.y)))
	{
		int index = GetIndex(sf::Vector2i(pos.x - 1, pos.y));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}

	if (ValidPosition(sf::Vector2i(pos.x, pos.y + 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x, pos.y + 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}
	if (ValidPosition(sf::Vector2i(pos.x, pos.y - 1)))
	{
		int index = GetIndex(sf::Vector2i(pos.x, pos.y - 1));
		if (m_Pieces[index] == nullptr || !SameColor(index, index2))
			m_SelectionSquares.push_back(index);
	}
	AddCastlingMoves(isWhite, index2);
}

std::ostream &operator<<(std::ostream &stream, const sf::Vector2f &v)
{
	stream << "(" << v.x << ", " << v.y << ")";
	return stream;
}

std::ostream &operator<<(std::ostream &stream, const sf::Vector2i &v)
{
	stream << "(" << v.x << ", " << v.y << ")";
	return stream;
}

bool Board::SameColor(int index1, int index2)
{
	if (m_Pieces[index1] && m_Pieces[index2])
	{
		return (((m_Pieces[index1]->type ^ m_Pieces[index2]->type) & PieceType::White) == 0);
	}
	else
	{
		return false;
	}
}

void Board::AddCastlingMoves(bool isWhite, int index)
{
	Piece *king = m_Pieces[index];
	sf::Vector2i pos = king->GetPosition();
	if (isWhite)
	{
		if (m_WhiteInCheck)
			return;
		// Standard Position of Rook
		sf::Vector2i leftRookPos = sf::Vector2i(7, 0);
		sf::Vector2i rightRookPos = sf::Vector2i(7, 0);
		Piece *leftRook = m_Pieces[leftRookPos.x * 8 + leftRookPos.y];
		Piece *rightRook = m_Pieces[rightRookPos.x * 8 + rightRookPos.y];
		if (king->firstMove)
		{
			// Queen Side:
			if (leftRook && CanCasle(isWhite, index, true))
			{
				if (leftRook->firstMove)
				{
					int index2 = m_WhiteCastleQueenMove.first.x * 8 + m_WhiteCastleQueenMove.first.y;
					m_SelectionSquares.push_back(index2);
				}
			}
			if (rightRook && CanCasle(isWhite, index, false))
			{
				if (rightRook->firstMove)
				{
					int index2 = m_WhiteCastleKingMove.first.x * 8 + m_WhiteCastleKingMove.first.y;
					m_SelectionSquares.push_back(index2);
				}
			}
		}
	}
	else
	{
		if (m_BlackInCheck)
			return;
		sf::Vector2i leftRookPos = sf::Vector2i(0, 0);
		sf::Vector2i rightRookPos = sf::Vector2i(0, 7);
		Piece *leftRook = m_Pieces[leftRookPos.x * 8 + leftRookPos.y];
		Piece *rightRook = m_Pieces[rightRookPos.x * 8 + rightRookPos.y];
		if (king->firstMove)
		{
			// Queen Side:
			if (leftRook && CanCasle(isWhite, index, true))
			{
				if (leftRook->firstMove)
				{
					int index2 = m_BlackCastleQueenMove.first.x * 8 + m_BlackCastleQueenMove.first.y;
					m_SelectionSquares.push_back(index2);
				}
			}
			if (rightRook && CanCasle(isWhite, index, false))
			{
				if (rightRook->firstMove)
				{
					int index2 = m_BlackCastleKingMove.first.x * 8 + m_WhiteCastleKingMove.first.y;
					m_SelectionSquares.push_back(index2);
				}
			}
		}
	}
}

bool Board::CanCasle(bool isWhite, int index, bool queenSide)
{
	if (isWhite)
	{
		if (queenSide)
		{
			constexpr int arr[] = {
				7 * 8 + 1,
				7 * 8 + 2,
				7 * 8 + 3};
			bool pieceExists = false;
			for (int i = 0; i < 3; i++)
			{
				if (m_Pieces[arr[i]])
				{
					pieceExists = true;
					break;
				}
			}
			return !pieceExists && m_WhiteCasleQueen;
		}
		else
		{
			constexpr int arr[] = {7 * 8 + 6,
								   7 * 8 + 5};
			bool pieceExists = false;
			for (int i = 0; i < 2; i++)
				if (m_Pieces[arr[i]])
				{
					pieceExists = true;
					break;
				}
			return !pieceExists && m_WhiteCasleKing;
		}
	}
	else
	{
		if (queenSide)
		{
			constexpr int arr[] = {
				0 * 8 + 1,
				0 * 8 + 2,
				0 * 8 + 3};
			bool pieceExists = false;
			for (int i = 0; i < 3; i++)
			{
				if (m_Pieces[arr[i]])
				{
					pieceExists = true;
					break;
				}
			}
			return !pieceExists && m_BlackCasleQueen;
		}
		else
		{
			constexpr int arr[] = {0 * 8 + 6,
								   0 * 8 + 5};
			bool pieceExists = false;
			for (int i = 0; i < 2; i++)
				if (m_Pieces[arr[i]])
				{
					pieceExists = true;
					break;
				}
			return !pieceExists && m_BlackCasleKing;
		}
	}
}

void Board::UpdateKingsCheck()
{
	int blackCheckers = 0;
	int whiteCheckers = 0;
	for (int i = 0; i < m_AlivePieces.size(); i++)
	{
		Piece *currentPiece = m_AlivePieces[i];
		if (currentPiece->type & PieceType::White)
		{
			bool f = CanPieceAttack(m_BlackKing, currentPiece);
			if (f)
				blackCheckers++;
		}
		else
		{
			bool f = CanPieceAttack(m_WhiteKing, currentPiece);
			if (f)
				whiteCheckers++;
		}
	}
	if (blackCheckers > 0)
		m_BlackInCheck = true;
	else
		m_BlackInCheck = false;
	if (whiteCheckers > 0)
		m_WhiteInCheck = true;
	else
		m_WhiteInCheck = false;
}

bool Board::CanPieceAttack(Piece *king, Piece *currentPiece)
{
	//We are sure that both the king and the piece are not from the same color
	int type = currentPiece->type;
	int index = currentPiece->GetIndex();
	int kingIndex = king->GetIndex();
	bool isWhite = currentPiece->type & PieceType::White;
	bool flag = false;
	if (type & PieceType::Bishop)
	{
		PossibleMovesBishop(isWhite, index);
		for (auto &square : m_SelectionSquares)
			if (square == kingIndex)
			{
				flag = true;
				break;
			}
		m_SelectionSquares.clear();
	}
	else if (type & PieceType::Rook)
	{
		PossibleMovesRook(isWhite, index);
		for (auto &square : m_SelectionSquares)
			if (square == kingIndex)
			{
				flag = true;
				break;
			}
		m_SelectionSquares.clear();
	}
	else if (type & PieceType::Knight)
	{
		PossibleMovesKnight(isWhite, index);
		for (auto &square : m_SelectionSquares)
			if (square == kingIndex)
			{
				flag = true;
				break;
			}
		m_SelectionSquares.clear();
	}
	else if (type & PieceType::Queen)
	{
		PossibleMovesQueen(isWhite, index);
		for (auto &square : m_SelectionSquares)
			if (square == kingIndex)
			{
				flag = true;
				break;
			}
		m_SelectionSquares.clear();
	}
	else if (type & PieceType::Pawn)
	{
		PossibleMovesPawn(isWhite, index, true);
		for (auto &square : m_SelectionSquares)
			if (square == kingIndex)
			{
				flag = true;
				break;
			}
		m_SelectionSquares.clear();
	}
	return flag;
}

void Board::MoveCastlingWhite()
{
	bool castlingQueen = false;
	bool castlingKing = false;

	//Is the new position a castling position?
	castlingQueen = (m_NewPosition.x == m_WhiteCastleQueenMove.first.x && m_NewPosition.y == m_WhiteCastleQueenMove.first.y);
	castlingKing = (m_NewPosition.x == m_WhiteCastleKingMove.first.x && m_NewPosition.y == m_WhiteCastleKingMove.first.y);

	if (castlingKing)
	{
		m_AlivePieces[m_SelectedIndex]->SetPosition(m_WhiteCastleKingMove.first);
		UpdateTile(m_OldPosition, m_WhiteCastleKingMove.first);

		sf::Vector2i oldRookPos = sf::Vector2i(7, 7);
		Piece *rightRook = m_Pieces[oldRookPos.x * 8 + oldRookPos.y];
		UpdateTile(oldRookPos, m_WhiteCastleKingMove.second);
		rightRook->SetPosition(m_WhiteCastleKingMove.second);
		m_WhiteCasleKing = false;
		m_WhiteCasleQueen = false;
	}
	else if (castlingQueen)
	{

		m_AlivePieces[m_SelectedIndex]->SetPosition(m_WhiteCastleQueenMove.first);
		UpdateTile(m_OldPosition, m_WhiteCastleQueenMove.first);
		sf::Vector2i oldRookPos = sf::Vector2i(7, 0);
		Piece *leftRook = m_Pieces[oldRookPos.x * 8 + oldRookPos.y];
		UpdateTile(oldRookPos, m_WhiteCastleQueenMove.second);
		leftRook->SetPosition(m_WhiteCastleQueenMove.second);
		m_WhiteCasleKing = false;
		m_WhiteCasleQueen = false;
	}
}

void Board::MoveCastlingBlack()
{
	bool castlingQueen = false;
	bool castlingKing = false;
	castlingQueen = (m_NewPosition.x == m_BlackCastleQueenMove.first.x && m_NewPosition.y == m_BlackCastleQueenMove.first.y);
	castlingKing = (m_NewPosition.x == m_BlackCastleKingMove.first.x && m_NewPosition.y == m_BlackCastleKingMove.first.y);
	if (castlingKing)
	{
		m_AlivePieces[m_SelectedIndex]->SetPosition(m_BlackCastleKingMove.first);
		UpdateTile(m_OldPosition, m_BlackCastleKingMove.first);
		sf::Vector2i oldRookPos = sf::Vector2i(0, 7);
		Piece *rightRook = m_Pieces[oldRookPos.x * 8 + oldRookPos.y];
		UpdateTile(oldRookPos, m_BlackCastleKingMove.second);
		rightRook->SetPosition(m_BlackCastleKingMove.second);
		m_BlackCasleKing = false;
		m_BlackCasleQueen = false;
	}
	else if (castlingQueen)
	{
		m_AlivePieces[m_SelectedIndex]->SetPosition(m_BlackCastleQueenMove.first);
		UpdateTile(m_OldPosition, m_BlackCastleQueenMove.first);
		sf::Vector2i oldRookPos = sf::Vector2i(0, 0);
		Piece *leftRook = m_Pieces[oldRookPos.x * 8 + oldRookPos.y];
		UpdateTile(oldRookPos, m_BlackCastleQueenMove.second);
		leftRook->SetPosition(m_BlackCastleQueenMove.second);
		m_BlackCasleKing = false;
		m_BlackCasleQueen = false;
	}
}

void Board::MoveNormally()
{
	m_AlivePieces[m_SelectedIndex]->SetPosition(m_NewPosition);
	UpdateTile(m_OldPosition, m_NewPosition);
}

bool Board::IsCastlingMove(bool isWhite)
{
	bool castlingMove = (!isWhite && (m_BlackCasleQueen && (m_NewPosition.x == m_BlackCastleQueenMove.first.x && m_NewPosition.y == m_BlackCastleQueenMove.first.y)) ||
						 (m_BlackCasleKing && (m_NewPosition.x == m_BlackCastleKingMove.first.x && m_NewPosition.y == m_BlackCastleKingMove.first.y)));
	castlingMove = castlingMove || (isWhite && (m_WhiteCasleQueen && (m_NewPosition.x == m_WhiteCastleQueenMove.first.x && m_NewPosition.y == m_WhiteCastleQueenMove.first.y) ||
												(m_WhiteCasleKing && (m_NewPosition.x == m_WhiteCastleKingMove.first.x && m_NewPosition.y == m_WhiteCastleKingMove.first.y))));
	return castlingMove;
}