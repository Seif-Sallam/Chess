#include "Board.h"
#include <math.h>
#include <iomanip>

sf::Vector2f Board::m_Offset;
sf::Vector2f Board::m_TileSize;

Board::Board(sf::Vector2f tileSize, sf::Vector2f offset, Piece *&currentPiece)
	: m_CurrentPiece(currentPiece), m_Row(0), m_Column(0), isMoving(false)
{
	m_Offset = offset;
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
	std::cout << "\n";
	m_Font.loadFromFile("rsc/Roboto.ttf");
	m_Text.setCharacterSize(13);
	m_Text.setFont(m_Font);
}

Piece *Board::SelectPiece(sf::Event event, sf::RenderWindow &window)
{
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	sf::FloatRect boardRect = {m_Offset.x, m_Offset.y, m_TileSize.x * 8.0f, m_TileSize.y * 8.0f};
	sf::FloatRect mouseRect = {mousePos.x, mousePos.y, 0.5f, 0.5f};
	if (boardRect.intersects(mouseRect) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		std::cout << "Selected\n";
		int row = 0, column = 0;
		row = (mousePos.y - m_Offset.y) / m_TileSize.y;
		column = (mousePos.x - m_Offset.x) / m_TileSize.x;
		int index = ((7 - row)) * 8 + column;
		if (row >= 0 && column >= 0 && row < 8 && column < 8)
			return m_Pieces[index];
		else
			return nullptr;
	}
	else

		return nullptr;
}

void Board::UpdateSelection(sf::Event &event, sf::RenderWindow &window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			std::cout << "Alive Pieces: " << m_AlivePieces.size() << std::endl;
			for (int i = 0; i < m_AlivePieces.size(); i++)
			{
				if (m_AlivePieces[i]->Contains(mousePos))
				{
					isMoving = true;
					m_SelectedIndex = i;
					m_OldPosition = m_AlivePieces[i]->GetPosition();
					GetPossibleMoves(m_AlivePieces[i]);
					std::cout << "OldPosition: " << m_OldPosition << std::endl;
					break;
				}
			}
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Left && isMoving == true)
		{
			isMoving = false;
			sf::Vector2i position = m_AlivePieces[m_SelectedIndex]->GetPosition();
			//row, column
			m_NewPosition = sf::Vector2i(mousePos.y / m_TileSize.y, mousePos.x / m_TileSize.x);
			m_NewPosition.x = m_NewPosition.x;
			bool flag = false;
			//Fix this

			if (SelectableIndex(m_NewPosition))
			{
				std::cout << "New Position: " << m_NewPosition << std::endl;
				m_AlivePieces[m_SelectedIndex]->SetPosition(m_NewPosition.x, m_NewPosition.y);
				UpdateTile(m_OldPosition, m_NewPosition);
			}
			m_SelectionSquares.clear();
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
	LOG("Inside Update Tile:\n");
	LOG("Index1: ");
	LOG(index1);
	LOG("\nIndex2: ");
	LOG(index2);
	LOG("\n");
	auto &piece1 = m_Pieces[index1];
	auto &piece2 = m_Pieces[index2];
	if (piece1 != nullptr)
		std::cout << "Piece1: " << piece1->GetName() << std::endl;
	if (piece2 != nullptr)
		std::cout << "Piece2: " << piece2->GetName() << std::endl;

	if (piece1 != piece2)
	{
		if (piece2 != nullptr)
		{
			for (int i = 0; i < m_AlivePieces.size(); i++)
			{
				if (m_AlivePieces[i] == piece2)
				{
					m_AlivePieces.erase(m_AlivePieces.begin() + i);
					std::cout << "Erased\n";
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
			shape.setPosition(m_TileSize.x * rank + m_Offset.x, m_TileSize.y * file + m_Offset.y);
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
		m_Text.setPosition(m_TileSize.x * file + m_Offset.x + 5.f, m_TileSize.y * 8 - 15.0f);
		m_Text.setFillColor((file % 2) ? m_BackColor : m_ForeColor);
		window.draw(m_Text);
	}
	for (int rank = 0; rank < 8; rank++)
	{
		std::string str;
		str.resize(1);
		str[0] = char('8' - rank);
		m_Text.setString(str);
		m_Text.setPosition(m_Offset.x + 3.0f, m_TileSize.y * rank + m_Offset.y + 5.f);
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
			// it is actully White
			if (pieceChar != places[i])
			{
				type = type | int(PieceType::White);
			}
			int index = (7 - rank) * 8 + file;
			// std::cout << index << " ";
			m_Pieces[index] = new Piece(type, m_PiecesSpriteSheet, 7 - rank, file);
			m_AlivePieces.push_back(m_Pieces[index]);
			std::cout << m_Pieces[index]->GetPosition() << " ";
			file++;
			continue;
		}
		std::cout << std::endl;
	}
	std::cout << "\nPIECES\n";
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int index = (7 - i) * 8 + j;
			if (m_Pieces[index])
			{
				std::cout << std::setw(3) << index << " ";
			}
		}
		std::cout << "\n";
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int index = i * 8 + j;
			if (m_Pieces[index])
			{
				std::cout << m_Pieces[index]->GetPosition() << " ";
			}
		}
		std::cout << "\n";
	}
}

void Board::GetPossibleMoves(Piece *piece)
{
	int type = piece->type;
	auto pos = piece->GetPosition();
	pos.x = pos.x;
	int index = pos.x * 8 + pos.y;
	bool isWhite = type & PieceType::White;
	if (type & PieceType::Pawn)
		PossibleMovesPawn(isWhite, index);

	if (type & PieceType::Rook)
	{
		PossibleMovesRook(isWhite, index);
	}
	else if (type & PieceType::Bishop)
	{
		PossibleMovesBishop(isWhite, index);
	}
	else if (type & PieceType::Queen)
	{
		PossibleMovesQueen(isWhite, index);
	}
	else if (type & PieceType::Knight)
	{
		PossibleMovesKnight(isWhite, index);
	}
	else if (type & PieceType::King)
	{
		PossibleMovesKing(isWhite, index);
	}
}

bool Board::SelectableIndex(sf::Vector2i position)
{
	int index = position.x * 8 + position.y;
	if (std::find(m_SelectionSquares.begin(), m_SelectionSquares.end(), index) != m_SelectionSquares.end())
		return true;

	return false;
}

void Board::PossibleMovesPawn(bool isWhite, int index)
{
	Piece *piece = m_Pieces[index];
	if (isWhite) // it is a whtie piece
	{
		sf::Vector2i pos = piece->GetPosition();

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