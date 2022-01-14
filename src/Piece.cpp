#include "Piece.h"
#include "Board.h"
#include <math.h>

Piece::Piece(int type, sf::Texture &texture, int32_t row, int32_t column)
    : type(type), m_Row(row), m_Column(column)
{
    m_Sprite.setTexture(texture);
    sf::IntRect rect = {0, 0, 214, 214};
    bool isWhite = type & int(PieceType::White);
    if (isWhite)
        type = type - int(PieceType::White);
    else
        rect.top = 214;

    int32_t power = std::log2(type);
    rect.left = power * 214;
    m_Sprite.setTextureRect(rect);
    m_Sprite.setScale(0.31, 0.31);
    m_Sprite.setPosition(column * Board::GetTileSize().x + Board::GetOffset().x,
                         row * Board::GetTileSize().y + Board::GetOffset().y);
}

//Update the pieces in the board
void Piece::SetPosition(int32_t row, int32_t column)
{
    if (row >= 0 && column >= 0 && row < 8 && column < 8)
    {
        m_Row = row;
        m_Column = column;
        m_Sprite.setPosition(column * Board::GetTileSize().x + Board::GetOffset().x,
                             row * Board::GetTileSize().y + Board::GetOffset().y);
        firstMove = false;
    }
}

void Piece::SetPosition(sf::Vector2i pos)
{
    SetPosition(pos.x, pos.y);
}

sf::Vector2i Piece::GetPosition()
{
    return sf::Vector2i(m_Row, m_Column);
}

void Piece::Draw(sf::RenderWindow &window, sf::Shader *shader)
{
    if (shader != nullptr)
        window.draw(m_Sprite, shader);
    else
        window.draw(m_Sprite);
}

bool Piece::Contains(const sf::Vector2i &pos)
{
    return this->m_Sprite.getGlobalBounds().contains(sf::Vector2f(pos));
}

std::string Piece::GetName()
{
    std::string output;
    if (type & PieceType::White)
        output = "White ";
    else
        output = "Black ";

    if (type & PieceType::King)
        output += "King";
    else if (type & PieceType::Queen)
        output += "Queen";
    else if (type & PieceType::Bishop)
        output += "Bishop";
    else if (type & PieceType::Knight)
        output += "Knight";
    else if (type & PieceType::Pawn)
        output += "Pawn";
    return output;
}
