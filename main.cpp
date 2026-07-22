#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <array>
#include "Pieces.h"
#include "Board.h"
#include "Engine.h"

using namespace std;

int main()
{

    sf::Font font; 
    if (!font.loadFromFile("arial.ttf")) 
    {
        return -1;
    }
    sf::RenderWindow window(sf::VideoMode(880, 880), "Chess");
    Board board;
    board.StartPos(); // Початкові координати
    Engine ChessEngine;

    bool isPieceSelected = false;
    Piece* selectedPiece = nullptr;
    vector<Position> possible_moves;
    vector<Position> castle_moves;
    sf::Event event;
    bool isCheckDisplayed = false;

    string currentPlayer = "white";
    // Головний цикл гри
    while (window.isOpen())
    {
        bool needsRedraw = false;
        if (currentPlayer == "black")
        {
            ChessEngine.MakeMove(board, currentPlayer); 
            currentPlayer = "white"; 
            needsRedraw = true;
        } 
        if (currentPlayer == "white")
        {

            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    needsRedraw = true;
                    int col = event.mouseButton.x / 110;
                    int row = event.mouseButton.y / 110;

                    if (isPieceSelected)
                    {
                        bool moveMade = false;
                        for (const auto& move : possible_moves)
                        {
                            if (move.row == row && move.col == col)
                            {
                                board.removeOpponentPiece(row, col, currentPlayer);
                                selectedPiece->Move(row, col);
                                selectedPiece->hasMoved = true;
                                moveMade = true;
                                break;
                            }
                        }
                        for (const auto& move : castle_moves)
                        {
                            King* current_king = board.GetCurrentKing(currentPlayer);
                            if (move.row == row && move.col == 2)
                            {
                                selectedPiece->Move(row, col); 
                                current_king->Castle("QueenSide", board.pieces, current_king->side);
                                selectedPiece->hasMoved = true;
                                moveMade = true; 
                                break;
                            }
                            else if (move.row == row && move.col == 6) 
                            { 
                                selectedPiece->Move(row, col); 
                                current_king->Castle("KingSide", board.pieces, current_king->side); 
                                selectedPiece->hasMoved = true;
                                moveMade = true;
                                break;
                            }
                        }
                        isPieceSelected = false;
                        possible_moves.clear();
                        castle_moves.clear();
                        if (moveMade)
                        {
                            currentPlayer = (currentPlayer == "white") ? "black" : "white";
                        }
                    }
                    else
                    {
                        for (auto& piece : board.pieces)
                        {
                            if (piece->pos.row == row && piece->pos.col == col && piece->side == currentPlayer)
                            {
                                selectedPiece = piece.get();
                                selectedPiece->Selected(possible_moves, board.pieces, false);
                                isPieceSelected = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        for (auto& piece : board.pieces)
        {
            if (piece->type == "Pawn" && (piece->pos.row == 0 || piece->pos.row == 7)) 
            {
                auto queen = dynamic_pointer_cast<Pawn>(piece)->PromoteToQueen(); 
                board.pieces.push_back(queen);  
                board.removePiece(piece->pos.row, piece->pos.col, piece->type);    
                needsRedraw = true; 
                break;
            }
        }

        King* current_king = board.GetCurrentKing(currentPlayer);  
        if (isPieceSelected && selectedPiece->type == "King")
        {
            if (current_king->CanCastleQueenSide(board.pieces)) 
            {
                castle_moves.push_back({ current_king->pos.row, 2 });
            }
            if (current_king->CanCastleKingSide(board.pieces))  
            {
                castle_moves.push_back({ current_king->pos.row, 6 }); 
            }
        }

        vector<Position> attack_moves;
        if (isPieceSelected)
        {
            attack_moves.clear();
            selectedPiece->Selected(attack_moves, board.pieces, true);
        }

        bool isInCheck = current_king->isChecked(board.pieces);

        possible_moves.erase(
            remove_if(possible_moves.begin(), possible_moves.end(),
                [&](const auto& move)
                {
                    return board.CheckTheMove(move.row, move.col, current_king, selectedPiece);
                }),
            possible_moves.end());

        attack_moves.erase(
            remove_if(attack_moves.begin(), attack_moves.end(),
                [&](const auto& move)
                {
                    return board.CheckTheMove(move.row, move.col, current_king, selectedPiece); 
                }),
            attack_moves.end());


        if (needsRedraw)
        {
            window.clear();

            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    sf::RectangleShape square(sf::Vector2f(110, 110));
                    square.setPosition(i * 110, j * 110);

                    if ((i + j) % 2 == 0)
                    {
                        square.setFillColor(sf::Color(255, 206, 158));
                    }
                    else
                    {
                        square.setFillColor(sf::Color(209, 139, 71));
                    }

                    window.draw(square);

                    for (const auto& move : possible_moves)
                    {
                        if (move.row == j && move.col == i)
                        {
                            sf::CircleShape circle(18);
                            circle.setFillColor(sf::Color(128, 128, 128, 200));
                            circle.setPosition(i * 110 + 36, j * 110 + 35);
                            window.draw(circle);
                        }
                    }
                    for (const auto& move : castle_moves)  
                    {
                        if (move.row == j && move.col == i) 
                        {
                            sf::CircleShape circle(18); 
                            circle.setFillColor(sf::Color(0, 128, 0, 200));
                            circle.setPosition(i * 110 + 36, j * 110 + 35); 
                            window.draw(circle); 
                        }
                    }
                    for (const auto& move : attack_moves)
                    {
                        if (move.row == j && move.col == i)
                        {
                            sf::RectangleShape whiteSquare(sf::Vector2f(110, 110));
                            whiteSquare.setFillColor(sf::Color(255, 255, 255, 255));
                            whiteSquare.setPosition(i * 110, j * 110);
                            window.draw(whiteSquare);
                            sf::RectangleShape redSquare(sf::Vector2f(110, 110));
                            redSquare.setFillColor(sf::Color(255, 0, 0, 180));
                            redSquare.setPosition(i * 110, j * 110);
                            window.draw(redSquare);
                        }
                    }
                    std::array<int, 8> numbers = { 1, 2, 3, 4, 5, 6, 7, 8 }; 
                    std::array<char, 8> letters = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' }; 

                    // Додавання цифр на крайніх лівих клітинках
                    if (i == 0)
                    {
                        sf::Text numberText; 
                        numberText.setFont(font); 
                        numberText.setString(std::to_string(numbers[7 - j])); // 7 - j, щоб відображати у зворотному порядку 
                        numberText.setCharacterSize(20); 
                        numberText.setFillColor(sf::Color::Black); 
                        numberText.setPosition(i * 110 + 5, j * 110 + 5); 
                        window.draw(numberText); 
                    }
                    // Додавання букв на крайніх нижніх клітинках
                    if (j == 7)
                    {
                        sf::Text letterText; 
                        letterText.setFont(font); 
                        letterText.setString(letters[i]); 
                        letterText.setCharacterSize(20); 
                        letterText.setFillColor(sf::Color::Black);  
                        letterText.setPosition(i * 110 + 95, j * 110 + 85); // Відступи для позиціонування тексту в правому нижньому куті
                        window.draw(letterText); 
                    }
                }
            }

            possible_moves.insert(possible_moves.end(), attack_moves.begin(), attack_moves.end());

            if (isInCheck && current_king != nullptr)
            {
                current_king->HighLightCheck(window);
            }

            if (board.isGameOver(isInCheck, current_king)) 
            {
                cout << currentPlayer << " has a checkMate!";
                while (window.isOpen()) 
                { 
                    while (window.pollEvent(event)) 
                    {
                        if (event.type == sf::Event::Closed) 
                        {
                            window.close();
                        }
                    }
                }
            }

            board.drawPieces(window); 

            window.display();
        }
    }

    return 0;
} 