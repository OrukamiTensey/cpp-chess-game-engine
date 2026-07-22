#include "Pieces.h"
#include <cstdlib>
#include <SFML/Graphics.hpp>

using namespace std;


// ============= Пішак (початок) ==============
Pawn::Pawn(const string& side, const string& type, int row, int col, int price) 
{
    this->price = price; 
	this->side = side;
	pos.row = row;  
	pos.col = col; 
    this->type = type;
	texture.loadFromFile("figures\\" + side + "_pawn.png");
	sprite.setTexture(texture);
	sprite.setPosition(col * 110 + 10, row * 110 + 7);

	float scaleFactor = 0.7f;
	sprite.setScale(scaleFactor, scaleFactor); 
}

void Pawn::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) 
{
	int direction = (side == "white") ? -1 : 1;
    Position orig_pos = this->pos;
    // Перевірка на можливість ходу вперед
    if (!attack)
    {
        if (pos.row + direction >= 0 && pos.row + direction < 8) 
        {
            bool blocked = false;
            for (const auto& piece : pieces)
            {
                if (piece->pos.row == pos.row + direction && piece->pos.col == pos.col) 
                {
                    blocked = true;
                    break;
                }
            }
            if (!blocked)
            {
                possible_moves.push_back({ pos.row + direction, pos.col });
                if ((pos.row == 6 && side == "white") || (pos.row == 1 && side == "black"))
                {
                    bool doubleBlocked = false;
                    for (const auto& piece : pieces)
                    {
                        if (piece->pos.row == pos.row + 2 * direction && piece->pos.col == pos.col)
                        {
                            doubleBlocked = true;
                            break;
                        }
                    }
                    if (!doubleBlocked)
                    {
                        possible_moves.push_back({ pos.row + 2 * direction, pos.col });
                    }
                }
            }
        }
        return;
    }

    // Перевірка на можливість атаки
    if (pos.row + direction >= 0 && pos.row + direction < 8) 
    {
        if (pos.col - 1 >= 0 && CanAttack(pos.row + direction, pos.col - 1, pieces, orig_pos)) 
        {
            possible_moves.push_back({ pos.row + direction, pos.col - 1 }); 
        } 
        if (pos.col + 1 < 8 && CanAttack(pos.row + direction, pos.col + 1, pieces, orig_pos)) 
        {
            possible_moves.push_back({ pos.row + direction, pos.col + 1 }); 
        }
    }
}

void Pawn::Move(int row, int col) 
{
	pos.row = row;
	pos.col = col;
	sprite.setPosition(col * 110 + 10, row * 110 + 7);
}

bool Pawn::CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos) 
{
    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == orig_pos.row && piece->pos.col == orig_pos.col && piece->side != side)
        {
            return false;
        }
    }

    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == row && piece->pos.col == col && piece->side != side)
        {
            return true;
        }
    }

    return false;
}

shared_ptr<Piece> Pawn::PromoteToQueen() 
{
    return std::make_shared<Queen>(side, "Queen", pos.row, pos.col, 90); 
}

int Pawn::AddCellPrice(const vector<vector<int>>& cellPrices) 
{
    return cellPrices[pos.row][pos.col];   
}
// ============= Пішак (кінець) ==============


// ============= Король (початок) ==============
King::King(const string& side, const string& type, int row, int col, int price) 
{
    this->price = price;
    this->side = side; 
    pos.row = row;
    pos.col = col;
    this->type = type;
    hasMoved = false;
    texture.loadFromFile("figures\\" + side + "_king.png");
    sprite.setTexture(texture);
    sprite.setPosition(col * 110 + 6, row * 110 + 4);

    float scaleFactor = 0.75f;
    sprite.setScale(scaleFactor, scaleFactor);
}

void King::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack)
{
    vector<Position> moves = { 
        {pos.row + 1, pos.col}, {pos.row - 1, pos.col}, {pos.row, pos.col + 1}, {pos.row, pos.col - 1},
        {pos.row + 1, pos.col + 1}, {pos.row + 1, pos.col - 1}, {pos.row - 1, pos.col + 1}, {pos.row - 1, pos.col - 1}
    };
    
    Position original_pos = this->pos;

    for (const auto& move : moves)  
    {
        if (move.row >= 0 && move.row < 8 && move.col >= 0 && move.col < 8)  
        {
            bool blocked = false;
            for (const auto& piece : pieces)  
            {
                if (piece->pos.row == move.row && piece->pos.col == move.col)  
                {
                    blocked = true; 
                    if (attack && CanAttack(move.row, move.col, pieces))  
                    {
                        this->Move(move.row, move.col); 
                        if (!isChecked(pieces))
                        {
                            possible_moves.push_back(move);  
                        }
                        this->Move(original_pos.row, original_pos.col); 
                    }
                    break;
                }
            }
            if (!blocked && !attack)    
            {
                this->Move(move.row, move.col);  
                if (!isChecked(pieces))
                {
                    possible_moves.push_back(move);  
                }
                this->Move(original_pos.row, original_pos.col);
            }
        }
    }
}

void King::Move(int row, int col)
{
    pos.row = row;
    pos.col = col;
    sprite.setPosition(col * 110 + 6, row * 110 + 4);
}

bool King::CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces)
{
    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == row && piece->pos.col == col && piece->side != side) 
        {
            return true;
        }
    }
    return false;
}

bool King::isChecked(const vector<shared_ptr<Piece>>& pieces) const 
{
    for (const auto& piece : pieces)
    {
        if (piece->side != this->side)
        {
            vector<Position> possible_moves;
            piece->Selected(possible_moves, pieces, true);
            for (const auto& move : possible_moves)
            {
                if (move.row == this->pos.row && move.col == this->pos.col)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool King::isMate(const vector<shared_ptr<Piece>>& pieces)
{
    if (!isChecked(pieces)) {
        return false; // Якщо король не в шаху, це не мат
    }

    // Перевіряємо, чи є безпечні ходи у короля
    vector<Position> king_moves;
    this->Selected(king_moves, pieces, false);

    Position original_pos = this->pos;
    for (const auto& move : king_moves)
    {
        this->Move(move.row, move.col);
        if (!isChecked(pieces))
        {
            this->Move(original_pos.row, original_pos.col);
            return false; // Є безпечний хід для короля
        }
        this->Move(original_pos.row, original_pos.col);
    }

    // Перевіряємо можливі ходи всіх фігур гравця
    for (const auto& piece : pieces)
    {
        if (piece->side == this->side)
        {
            vector<Position> possible_moves;
            piece->Selected(possible_moves, pieces, false);

            for (const auto& move : possible_moves)
            {
                Position original_pos = piece->pos;
                piece->Move(move.row, move.col);
                if (!isChecked(pieces))
                {
                    piece->Move(original_pos.row, original_pos.col);
                    return false; // Є хід, що захищає короля
                }
                piece->Move(original_pos.row, original_pos.col);
            }
        }
    }

    return true; // Якщо немає безпечних ходів, це мат
}

void King::HighLightCheck(sf::RenderWindow& window)   
{
    sf::RectangleShape whiteSquare(sf::Vector2f(110, 110));
    whiteSquare.setFillColor(sf::Color(209, 139, 71));
    whiteSquare.setPosition(this->pos.col * 110, this->pos.row * 110);
    window.draw(whiteSquare);
    sf::RectangleShape redSquare(sf::Vector2f(110, 110));
    redSquare.setFillColor(sf::Color(255, 0, 0, 125));
    redSquare.setPosition(this->pos.col * 110, this->pos.row * 110);
    window.draw(redSquare);
}

bool King::CanCastleQueenSide(const vector<shared_ptr<Piece>>& pieces)
{
    if (hasMoved) return false; // Якщо король рухався, рокірування неможливе 

    Rook* queensideRook = nullptr;

    for (const auto& piece : pieces)
    {
        if (piece->type == "Rook" && piece->side == this->side)
        {
            Rook* rook = static_cast<Rook*>(piece.get());
            if (!rook->hasMoved)
            {
                if (rook->pos.col == 0) queensideRook = rook; // Тура на ферзевому фланзі
            }
        }
    }

    bool canCastleQueenside = false;

    if (queensideRook)
    {
        // Перевіримо клітинки між королем і ферзевою турою
        bool pathClear = true;
        for (int col = 1; col < 4; ++col)
        {
            for (const auto& piece : pieces)
            {
                if (piece->pos.row == this->pos.row && piece->pos.col == col)
                {
                    pathClear = false; // Між королем і турою є фігура
                    break;
                }
            }
        }

        // Перевіримо, чи клітинки між королем і турою не атаковані
        if (pathClear && !isChecked(pieces))
        {
            Position original_pos = this->pos;
            for (int col = 3; col >= 0; --col)
            {
                this->Move(this->pos.row, col);
                if (isChecked(pieces))
                {
                    pathClear = false; // Клітинки під атакою
                    break;
                }
            }
            this->Move(original_pos.row, original_pos.col);

            if (pathClear)
            {
                canCastleQueenside = true;
            }
        }
    }

    return canCastleQueenside;
}

bool King::CanCastleKingSide(const vector<shared_ptr<Piece>>& pieces)
{
    if (hasMoved) return false; // Якщо король рухався, рокірування неможливе 

    Rook* kingsideRook = nullptr;

    for (const auto& piece : pieces)
    {
        if (piece->type == "Rook" && piece->side == this->side)
        {
            Rook* rook = static_cast<Rook*>(piece.get()); 
            if (!rook->hasMoved) 
            {
                if (rook->pos.col == 0) kingsideRook = rook; // Тура на ферзевому фланзі
            }
        }
    }

    bool canCastleQueenside = false; 

    if (kingsideRook) 
    {
        // Перевіримо клітинки між королем і ферзевою турою
        bool pathClear = true; 
        for (int col = 5; col < 7; ++col) 
        {
            for (const auto& piece : pieces) 
            {
                if (piece->pos.row == this->pos.row && piece->pos.col == col) 
                {
                    pathClear = false; // Між королем і турою є фігура 
                    break; 
                }
            }
        }

        // Перевіримо, чи клітинки між королем і турою не атаковані
        if (pathClear && !isChecked(pieces))
        {
            Position original_pos = this->pos;
            for (int col = 7; col >= 5; --col)
            {
                this->Move(this->pos.row, col);
                if (isChecked(pieces))
                {
                    pathClear = false; // Клітинки під атакою
                    break;
                }
            }
            this->Move(original_pos.row, original_pos.col);

            if (pathClear)
            {
                canCastleQueenside = true;
            }
        }
    }

    return canCastleQueenside;
}

void King::Castle(string side, const vector<shared_ptr<Piece>>& pieces, string kingside) 
{
    if (side == "QueenSide")
    {
        for (const auto& piece : pieces)
        {
            if (piece.get()->type == "Rook" && piece.get()->pos.col == 0 && piece.get()->side == kingside)
            {
                piece.get()->Move(piece.get()->pos.row, 3);
            }
        }
    }

    if (side == "KingSide") 
    {
        for (const auto& piece : pieces) 
        {
            if (piece.get()->type == "Rook" && piece.get()->pos.col == 7 && piece.get()->side == kingside) 
            {
                piece.get()->Move(piece.get()->pos.row, 5);  
            }
        }
    }
}

int King::AddCellPrice(const vector<vector<int>>& cellPrices) 
{
    return cellPrices[pos.row][pos.col];
}

// ============= Король (кінець) ==============


// ============= Королева (початок) ==============
Queen::Queen(const string& side, const string& type, int row, int col, int price)
{
    this->price = price;
    this->side = side;
    pos.row = row;
    pos.col = col;
    this->type = type;
    texture.loadFromFile("figures\\" + side + "_queen.png");
    sprite.setTexture(texture);
    sprite.setPosition(col * 110 + 6, row * 110 + 4);

    float scaleFactor = 0.75f;
    sprite.setScale(scaleFactor, scaleFactor);
}

void Queen::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack)
{ 
    vector<Position> bishop_moves; 
    Bishop bishop(side, type, pos.row, pos.col, 90);  
    bishop.Selected(bishop_moves, pieces, attack);  

    // Вектор для збереження можливих ходів ладьї
    vector<Position> rook_moves; 
    Rook rook(side, type, pos.row, pos.col, 90);   
    rook.Selected(rook_moves, pieces, attack);  

    // Об'єднуємо можливі ходи слона та ладьї в один вектор
    possible_moves.insert(possible_moves.end(), bishop_moves.begin(), bishop_moves.end()); 
    possible_moves.insert(possible_moves.end(), rook_moves.begin(), rook_moves.end()); 
}

void Queen::Move(int row, int col)
{
    pos.row = row;
    pos.col = col;
    sprite.setPosition(col * 110 + 6, row * 110 + 4);
}

int Queen::AddCellPrice(const vector<vector<int>>& cellPrices)
{
    return cellPrices[pos.row][pos.col];
}
// ============= Королева (кінець) ==============


// ============= Слон (початок) ==============
Bishop::Bishop(const string& side, const string& type, int row, int col, int price)
{
    this->price = price;
    this->side = side;
    pos.row = row;
    pos.col = col;
    this->type = type;
    texture.loadFromFile("figures\\" + side + "_bishop.png");
    sprite.setTexture(texture);
    sprite.setPosition(col * 110 + 6, row * 110 + 4);

    float scaleFactor = 0.75f;
    sprite.setScale(scaleFactor, scaleFactor);
}

void Bishop::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack)
{
    Position orig_pos = this->pos;
    // Перевіряємо можливі ходи та атаки по діагоналі вверх-вправо
    for (int i = 1; pos.row + i < 8 && pos.col + i < 8; ++i) 
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row + i && piece->pos.col == pos.col + i) 
            {
                blocked = true;
                if (attack && CanAttack(pos.row + i, pos.col + i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row + i, pos.col + i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row + i, pos.col + i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вверх-вправо
        }
    }

    // Перевіряємо можливі ходи та атаки по діагоналі вниз-вправо
    for (int i = 1; pos.row - i >= 0 && pos.col + i < 8; ++i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row - i && piece->pos.col == pos.col + i)
            {
                blocked = true;
                if (attack && CanAttack(pos.row - i, pos.col + i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row - i, pos.col + i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row - i, pos.col + i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вниз-вправо
        }
    }

    // Перевіряємо можливі ходи та атаки по діагоналі вниз-вліво
    for (int i = 1; pos.row - i >= 0 && pos.col - i >= 0; ++i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row - i && piece->pos.col == pos.col - i)
            {
                blocked = true;
                if (attack && CanAttack(pos.row - i, pos.col - i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row - i, pos.col - i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row - i, pos.col - i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вниз-вліво
        }
    }

    // Перевіряємо можливі ходи та атаки по діагоналі вверх-вліво
    for (int i = 1; pos.row + i < 8 && pos.col - i >= 0; ++i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row + i && piece->pos.col == pos.col - i)
            {
                blocked = true;
                if (attack && CanAttack(pos.row + i, pos.col - i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row + i, pos.col - i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row + i, pos.col - i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вверх-вліво
        }
    }
}

void Bishop::Move(int row, int col)
{
    pos.row = row;
    pos.col = col;
    sprite.setPosition(col * 110 + 6, row * 110 + 4);
}

bool Bishop::CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos) 
{
    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == orig_pos.row && piece->pos.col == orig_pos.col && piece->side != side)
        {
            return false;
        }
    }

    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == row && piece->pos.col == col && piece->side != side)
        {
            return true;
        }
    }

    return false;
}

int Bishop::AddCellPrice(const vector<vector<int>>& cellPrices)
{
    return cellPrices[pos.row][pos.col]; 
}
// ============= Слон (кінець) ==============


// ============= Ладья (початок) ==============
Rook::Rook(const string& side, const string& type, int row, int col, int price)
{
    this->price = price;
    this->side = side;
    pos.row = row;
    pos.col = col;
    this->type = type;
    hasMoved = false;
    texture.loadFromFile("figures\\" + side + "_rook.png");
    sprite.setTexture(texture);
    sprite.setPosition(col * 110 + 10, row * 110 + 7);

    float scaleFactor = 0.7f;
    sprite.setScale(scaleFactor, scaleFactor);
}

void Rook::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack)
{
    Position orig_pos = this->pos;
    // Перевіряємо можливі ходи та атаки вгору
    for (int i = pos.row - 1; i >= 0; --i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == i && piece->pos.col == pos.col)
            {
                blocked = true;
                if (attack && CanAttack(i, pos.col, pieces, orig_pos))
                {
                    possible_moves.push_back({ i, pos.col });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ i, pos.col });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вгору
        }
    }

    // Перевіряємо можливі ходи та атаки вниз
    for (int i = pos.row + 1; i < 8; ++i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == i && piece->pos.col == pos.col)
            {
                blocked = true;
                if (attack && CanAttack(i, pos.col, pieces, orig_pos))
                {
                    possible_moves.push_back({ i, pos.col });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ i, pos.col });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вниз
        }
    }

    // Перевіряємо можливі ходи та атаки вправо
    for (int i = pos.col + 1; i < 8; ++i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row && piece->pos.col == i)
            {
                blocked = true;
                if (attack && CanAttack(pos.row, i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row, i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row, i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вправо
        }
    }

    // Перевіряємо можливі ходи та атаки вліво
    for (int i = pos.col - 1; i >= 0; --i)
    {
        bool blocked = false;
        for (const auto& piece : pieces)
        {
            if (piece->pos.row == pos.row && piece->pos.col == i)
            {
                blocked = true;
                if (attack && CanAttack(pos.row, i, pieces, orig_pos))
                {
                    possible_moves.push_back({ pos.row, i });
                }
                break;
            }
        }
        if (!blocked && !attack)
        {
            possible_moves.push_back({ pos.row, i });
        }
        else if (blocked)
        {
            break; // Якщо є блокер, перестаємо додавати ходи вліво
        }
    }

      
}

void Rook::Move(int row, int col)
{
    pos.row = row;
    pos.col = col;
    sprite.setPosition(col * 110 + 10, row * 110 + 7);
}

bool Rook::CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos)
{
    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == orig_pos.row && piece->pos.col == orig_pos.col && piece->side != side)
        {
            return false;
        }
    }

    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == row && piece->pos.col == col && piece->side != side)
        {
            return true;
        }
    }

    return false;
}

int Rook::AddCellPrice(const vector<vector<int>>& cellPrices)
{
    return cellPrices[pos.row][pos.col]; 
}
// ============= Ладья (кінець) ==============


// ============= Кінь (початок) ==============
Knight::Knight(const string& side, const string& type, int row, int col, int price) 
{
    this->price = price;
    this->side = side;
    pos.row = row;
    pos.col = col;
    this->type = type;
    texture.loadFromFile("figures\\" + side + "_knight.png");
    sprite.setTexture(texture);
    sprite.setPosition(col * 110 + 6, row * 110 + 4);

    float scaleFactor = 0.75f;
    sprite.setScale(scaleFactor, scaleFactor);
}

void Knight::Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack)
{
    Position orig_pos = this->pos;
    // Усі можливі відносні переміщення для коня
    vector<Position> moves = { 
        {pos.row + 2, pos.col + 1}, {pos.row + 2, pos.col - 1}, 
        {pos.row - 2, pos.col + 1}, {pos.row - 2, pos.col - 1},
        {pos.row + 1, pos.col + 2}, {pos.row + 1, pos.col - 2},
        {pos.row - 1, pos.col + 2}, {pos.row - 1, pos.col - 2}
    };

    for (const auto& move : moves) 
    {
        // Перевірка, чи знаходиться хід у межах дошки
        if (move.row >= 0 && move.row < 8 && move.col >= 0 && move.col < 8) 
        {
            bool blocked = false;
            for (const auto& piece : pieces) 
            {
                if (piece->pos.row == move.row && piece->pos.col == move.col) 
                {
                    blocked = true;
                    if (attack && CanAttack(move.row, move.col, pieces, orig_pos))  
                    {
                        possible_moves.push_back(move); 
                    } 
                    break;
                }
            }
            if (!blocked && !attack) 
            {
                possible_moves.push_back(move); 
            }
        }
    }
}

void Knight::Move(int row, int col) 
{
    pos.row = row;
    pos.col = col;
    sprite.setPosition(col * 110 + 6, row * 110 + 4);
}

bool Knight::CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos)
{
    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == orig_pos.row && piece->pos.col == orig_pos.col && piece->side != side)
        {
            return false;
        }
    }

    for (const auto& piece : pieces)
    {
        if (piece && piece->pos.row == row && piece->pos.col == col && piece->side != side)
        {
            return true;
        }
    }

    return false;
}

int Knight::AddCellPrice(const vector<vector<int>>& cellPrices)
{
    return cellPrices[pos.row][pos.col]; 
}
// ============= Кінь (кінець) ==============