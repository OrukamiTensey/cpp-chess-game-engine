#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "Pieces.h"
#include "vector"

using namespace std;

class Board
{
public:
	vector<shared_ptr<Piece>> pieces; 
	
	void StartPos();
	void drawPieces(sf::RenderWindow& window); 
	King* GetCurrentKing(const string& currentPlayer);
	shared_ptr<Piece> removeOpponentPiece(int row, int col, const string& currentPlayer);
	bool CheckTheMove(int row, int col, King* current_king, Piece* selectedPiece);
	bool isGameOver(bool isInCheck, King* current_king) const;
	void removePiece(int row, int col, string& type);
};



#endif // BOARD_H
