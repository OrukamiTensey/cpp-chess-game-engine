#ifndef ENGINE_H
#define ENGINE_H

#include "Board.h"
#include "Pieces.h"
#include <vector>
#include <utility>
#include <string>

using namespace std;

class Engine
{
public:
    void MakeMove(Board& board, string& currentPlayer);
    static int evaluatePosition(const vector<shared_ptr<Piece>>& pieces);
    static int minimax(Board& board, vector<shared_ptr<Piece>> current_position, int depth, int alpha, int beta, bool maximizingPlayer, string currentPlayer);
    pair<Piece*, Position> findBestMove(Board& board, int depth, string& currentPlayer);
    static vector<pair<Piece*, Position>> getPossibleMoves(vector<shared_ptr<Piece>> current_position, const string& currentPlayer, Board& board); 
    static vector<pair<vector<shared_ptr<Piece>>, pair<Piece*, Position>>> getPossiblePositions(Board& board, vector<shared_ptr<Piece>> current_position, const string& currentPlayer);
    
};


#endif // ENGINE_H 