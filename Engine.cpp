#include "Engine.h"
#include "Board.h"
#include "Cellprices.h"
#include <limits>
#include <algorithm>
#include <iostream>

using namespace std;

int Engine::evaluatePosition(const vector<shared_ptr<Piece>>& pieces)
{
    int score = 0;
    for (const auto& piece : pieces)
    {
        if (piece->side == "black")
        {
            if (piece->type == "Pawn") 
            { 
                score += piece->AddCellPrice(pawnCellPrices); 
            }
            else if (piece->type == "Knight") 
            {
                score += piece->AddCellPrice(knightCellPrices); 
            }
            else if (piece->type == "Queen")
            {
                score += piece->AddCellPrice(queenCellPrices);
            }
            else if (piece->type == "Rook")
            {
                score += piece->AddCellPrice(rookCellPrices);
            }
            else if (piece->type == "Bishop")
            {
                score += piece->AddCellPrice(bishopCellPrices);
            }
        }
        score += piece->price;
    }
    return score;
}

int Engine::minimax(Board& board, vector<shared_ptr<Piece>> current_position, int depth, int alpha, int beta, bool maximizingPlayer, string currentPlayer)
{
    King* current_king = board.GetCurrentKing(currentPlayer);
    if (depth == 0 || board.isGameOver(current_king->isChecked(board.pieces), current_king))
    {
        int eval = evaluatePosition(current_position);
        return eval;
    }

    if (maximizingPlayer)
    {
        int maxEval = INT_MIN;
        for (auto& position : getPossiblePositions(board, current_position, currentPlayer))
        {
            string nextPlayer = "white";
            int eval = minimax(board, position.first, depth - 1, alpha, beta, false, nextPlayer);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (auto& position : getPossiblePositions(board, current_position, currentPlayer))
        {
            string nextPlayer = "black";
            int eval = minimax(board, position.first, depth - 1, alpha, beta, true, nextPlayer);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

pair<Piece*, Position> Engine::findBestMove(Board& board, int depth, string& currentPlayer)
{
    int bestEval = INT_MIN;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    pair<Piece*, Position> bestMove;
    vector<pair<vector<shared_ptr<Piece>>, pair<Piece*, Position>>> possiblePositions = getPossiblePositions(board, board.pieces, currentPlayer);
    string nextPlayer = "white";
    for (auto& position : possiblePositions)
    {
        int eval = minimax(board, position.first, depth, alpha, beta, false, nextPlayer);
        if (eval > bestEval)
        {
            bestEval = eval;
            bestMove = position.second;
        }
    }
    return bestMove;
}

void Engine::MakeMove(Board& board, string& currentPlayer)
{
    pair<Piece*, Position> bestMove = findBestMove(board, 1, currentPlayer);
    board.removeOpponentPiece(bestMove.second.row, bestMove.second.col, currentPlayer);
    bestMove.first->Move(bestMove.second.row, bestMove.second.col);

    for (auto& piece : board.pieces) 
    {
        if (bestMove.first->type == "Pawn" && (bestMove.second.row == 0 || bestMove.second.row == 7))  
        {
            auto queen = dynamic_pointer_cast<Pawn>(piece)->PromoteToQueen(); 
            board.pieces.push_back(queen); 
            board.removePiece(piece->pos.row, piece->pos.col, bestMove.first->type);   
            break;
        }
    }
    cout << board.GetCurrentKing("white")->hasMoved << endl;
}

vector<pair<Piece*, Position>> Engine::getPossibleMoves(vector<shared_ptr<Piece>> current_position, const string& currentPlayer, Board& board)
{
    vector<pair<Piece*, Position>> possibleMoves;
    for (const auto& piece : current_position)
    {
        if (piece->side == currentPlayer)
        {
            vector<Position> moves;
            vector<Position> attack_moves;
            piece->Selected(moves, current_position, false);
            piece->Selected(attack_moves, current_position, true);
            moves.insert(moves.end(), attack_moves.begin(), attack_moves.end());
            for (const auto& move : moves)
            {
                possibleMoves.push_back(make_pair(piece.get(), move));
            }
        }
    }

    return possibleMoves;
}

vector<pair<vector<shared_ptr<Piece>>, pair<Piece*, Position>>> Engine::getPossiblePositions(Board& board, vector<shared_ptr<Piece>> current_position, const string& currentPlayer)
{
    vector<pair<Piece*, Position>> possible_moves = getPossibleMoves(current_position, currentPlayer, board); 
    vector<pair<vector<shared_ptr<Piece>>, pair<Piece*, Position>>> possiblePositions;

    for (const auto& move : possible_moves)
    {
        Position orig_pos({ move.first->pos.row, move.first->pos.col });
        vector<shared_ptr<Piece>> child_position;

        for (const auto& piece : current_position)
        {
            if (piece->pos.row == move.second.row && piece->pos.col == move.second.col && piece->side != currentPlayer)
            {
                continue; // Пропустити захоплену фігуру
            }

            if (piece.get() == move.first)
            {
                if (piece->type == "Pawn")
                    child_position.push_back(make_shared<Pawn>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
                else if (piece->type == "King")
                    child_position.push_back(make_shared<King>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
                else if (piece->type == "Queen")
                    child_position.push_back(make_shared<Queen>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
                else if (piece->type == "Bishop")
                    child_position.push_back(make_shared<Bishop>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
                else if (piece->type == "Rook")
                    child_position.push_back(make_shared<Rook>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
                else if (piece->type == "Knight")
                    child_position.push_back(make_shared<Knight>(currentPlayer, piece->type, move.second.row, move.second.col, piece->price));
            }
            else
            {
                child_position.push_back(piece);
            }
        }
        possiblePositions.push_back(make_pair(child_position, make_pair(move.first, move.second)));
    }

    return possiblePositions;
}
