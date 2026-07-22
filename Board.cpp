#include "Board.h"
#include "Pieces.h"
#include <cstdlib>
#include <SFML/Graphics.hpp>

using namespace std;

void Board::StartPos()  
{
	for (int i = 0; i < 8; ++i)
	{
		pieces.push_back(make_shared<Pawn>("white", "Pawn", 6, i, -10)); 
		pieces.push_back(make_shared<Pawn>("black", "Pawn", 1, i, 10)); 
	}
	pieces.push_back(make_shared<King>("white", "King", 7, 4, -900));
	pieces.push_back(make_shared<King>("black", "King", 0, 4, 900));

	pieces.push_back(make_shared<Queen>("white", "Queen", 7, 3, -90));
	pieces.push_back(make_shared<Queen>("black", "Queen", 0, 3, 90));

	pieces.push_back(make_shared<Bishop>("white", "Bishop", 7, 2, -30)); pieces.push_back(make_shared<Bishop>("white", "Bishop", 7, 5, -30));
	pieces.push_back(make_shared<Bishop>("black", "Bishop", 0, 2, 30)); pieces.push_back(make_shared<Bishop>("black", "Bishop", 0, 5, 30));

	pieces.push_back(make_shared<Rook>("white", "Rook", 7, 0, -50)); pieces.push_back(make_shared<Rook>("white", "Rook", 7, 7, -50)); 
	pieces.push_back(make_shared<Rook>("black", "Rook", 0, 0, 50)); pieces.push_back(make_shared<Rook>("black", "Rook", 0, 7, 50)); 

	pieces.push_back(make_shared<Knight>("white", "Knight", 7, 1, -30)); pieces.push_back(make_shared<Knight>("white", "Knight", 7, 6, -30));
	pieces.push_back(make_shared<Knight>("black", "Knight", 0, 1, 30)); pieces.push_back(make_shared<Knight>("black", "Knight", 0, 6, 30));

    srand(static_cast<unsigned int>(time(nullptr))); 

    // Перемішування вектора pieces
    for (int i = pieces.size() - 1; i > 0; --i)  
    {
        int j = rand() % (i + 1); 
        std::swap(pieces[i], pieces[j]);  
    }
}
 

void Board::drawPieces(sf::RenderWindow& window) 
{
	for (auto& piece : this->pieces) 
	{
		window.draw(piece->sprite);
	}
}

King* Board::GetCurrentKing(const string& currentPlayer)
{
    for (auto& piece : this->pieces)
    {
        if (piece->type == "King" && piece->side == currentPlayer)
        {
            return static_cast<King*>(piece.get());
        }
    }
    return nullptr;
}

shared_ptr<Piece> Board::removeOpponentPiece(int row, int col, const string& currentPlayer) 
{
    for (auto it = this->pieces.begin(); it != this->pieces.end(); ++it) 
    {
        if (*it && (*it)->pos.row == row && (*it)->pos.col == col && (*it)->side != currentPlayer)
        {
            shared_ptr<Piece> capturedPiece = *it;   
            this->pieces.erase(it);
            return capturedPiece;
        }
    }
    return nullptr;
}

bool Board::CheckTheMove(int row, int col, King* current_king, Piece* selectedPiece)
{
    Position original_pos = selectedPiece->pos;
    selectedPiece->Move(row, col);
    if (!current_king->isChecked(this->pieces)) 
    {
        selectedPiece->Move(original_pos.row, original_pos.col);
        return false;
    }
    else
    {
        selectedPiece->Move(original_pos.row, original_pos.col);
        return true;
    }
}

bool Board::isGameOver(bool isInCheck, King* current_king) const
{
    if (isInCheck && current_king != nullptr) 
    {
        if (current_king->isMate(pieces))
        {
            return true;
        }
    }
}

void Board::removePiece(int row, int col, string& type)   
{
    pieces.erase(  
        remove_if(pieces.begin(), pieces.end(),
            [&](const shared_ptr<Piece>& piece)
            {
                return piece->pos.row == row && piece->pos.col == col && piece->type == type; 
            }),
        pieces.end()
    );
}
