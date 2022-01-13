#include "Board.h"
sf::Vector2f Board::m_Offset;
sf::Vector2f Board::m_TileSize;

Board::Board(sf::Vector2f tileSize, sf::Vector2f offset)
{
	m_Offset = offset;
	m_TileSize = tileSize;
	m_PiecesSpriteSheet.loadFromFile("rsc/ChessPieces.png");
	for (int file = 0; file < 8; file++)
		for (int rank = 0; rank < 8; rank++)
		{
			int index = file * 8 + rank;
			m_Pieces[index] = nullptr;
		}
	std::string piecePlacement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	CreatePieces(piecePlacement);
	std::cout << "\n";
	m_Font.loadFromFile("rsc/Roboto.ttf");
	m_Texts.setCharacterSize(13);
	m_Texts.setFont(m_Font);
}

Piece *Board::SelectPiece(sf::Event event, sf::RenderWindow &window)
{
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	sf::FloatRect boardRect = {m_Offset.x, m_Offset.y, m_TileSize.x * 8.0f, m_TileSize.y * 8.0f};
	sf::FloatRect mouseRect = {mousePos.x, mousePos.y, 2.0f, 2.0f};
	if (boardRect.intersects(mouseRect) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		std::cout << "Selected\n";
		int row = 0, column = 0;
		row = (mousePos.y - m_Offset.y) / m_TileSize.y;
		column = (mousePos.x - m_Offset.x) / m_TileSize.x;
		std::cout << "row: " << row << std::endl;
		int index = ((7 - row)) * 8 + column;
		std::cout << "index: " << index << std::endl;
		if (row >= 0 && column >= 0 && row < 8 && column < 8)
		{
			int index = row * 8 + column;
			return m_Pieces[index];
		}
		else
			return nullptr;
	}
	else

		return nullptr;
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
			delete piece2;
		piece2 = piece1;
		piece1 = nullptr;
	}
}

void Board::Draw(sf::RenderWindow &window, sf::Shader *tilesShader)
{
	//Drawing the Tiles
	static sf::RectangleShape shape;
	shape.setSize(m_TileSize);
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			shape.setPosition(m_TileSize.x * rank + m_Offset.x, m_TileSize.y * file + m_Offset.y);
			shape.setFillColor(((file + rank) % 2 == 0) ? m_ForeColor : m_BackColor);
			if (tilesShader != nullptr)
				window.draw(shape, tilesShader);
			else
				window.draw(shape);
		}
	}

	//Drawing the pieces

	// for (int file = 0; file < 8; file++)
	// {
	// 	for (int rank = 0; rank < 8; rank++)
	// 	{
	// 		int index = file * 8 + rank;
	// 		if (m_Pieces[index] != nullptr)
	// 		{
	// 			m_Pieces[index]->Draw(window, nullptr);
	// 		}
	// 	}
	// }
	for (int file = 0; file < 8; file++)
	{
		// std::cout << "Here\n";
		std::string str;
		str.resize(1);
		str[0] = char(file + 'a');
		m_Texts.setString(str);
		m_Texts.setPosition(m_TileSize.x * file + m_Offset.x + 5.f, m_TileSize.y * 8 + m_TileSize.y - 25.f);
		m_Texts.setFillColor((file % 2) ? m_BackColor : m_ForeColor);
		window.draw(m_Texts);
	}
	for (int rank = 0; rank < 8; rank++)
	{
		// std::cout << "Here\n";
		std::string str;
		str.resize(1);
		str[0] = char('8' - rank);
		m_Texts.setString(str);
		m_Texts.setPosition(m_Offset.x + 3.0f, m_TileSize.y * rank + m_Offset.y + 5.f);
		m_Texts.setFillColor((rank % 2) ? m_ForeColor : m_BackColor);
		window.draw(m_Texts);
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
			int index = ((rank + 8) % 8) * 8 + file;
			std::cout << index << " ";
			m_Pieces[index] = new Piece(type, m_PiecesSpriteSheet, rank, file);
			file++;
			continue;
		}
		std::cout << std::endl;
	}
}