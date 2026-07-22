#ifndef PIECES_H_INCLUDED
#define PIECES_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;

struct Position
{
	int row;
	int col;
	bool operator==(const Position& other) const
	{
		return row == other.row &&
			col == other.col;
	}
};

// Базовий клас фігури
class Piece 
{
public:
	/*virtual Piece* clone() const = 0; */
	virtual void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) = 0;  
	virtual void Move(int row, int col) = 0;
	virtual int AddCellPrice(const vector<vector<int>>& cellPrices) = 0; 
	virtual ~Piece() = default;
	int price;
	Position pos;
	string type;
	string side;
	sf::Texture texture;
	sf::Sprite sprite;
	bool hasMoved = false; 
};

// Похідні класи фігур \/
class Pawn : public Piece
{
public:
	Pawn(const string& side, const string& type, int row, int col, int price); 
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override; 
	bool CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos); 
	shared_ptr<Piece> PromoteToQueen();
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;
};

class King : public Piece
{
public:
	King(const string& side, const string& type, int row, int col, int price);
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override;
	bool CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces); 
	bool isChecked(const vector<shared_ptr<Piece>>& pieces) const;
	bool isMate(const vector<shared_ptr<Piece>>& pieces); 
	void HighLightCheck(sf::RenderWindow& window); 
	bool CanCastleQueenSide(const vector<shared_ptr<Piece>>& pieces);
	bool CanCastleKingSide(const vector<shared_ptr<Piece>>& pieces);
	void Castle(string side, const vector<shared_ptr<Piece>>& pieces, string kingside); 
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;  
};

class Rook : public Piece
{
public:
	Rook(const string& side, const string& type, int row, int col, int price);
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override;
	bool CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos);
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;
};

class Queen : public Piece
{
public:
	Queen(const string& side, const string& type, int row, int col, int price);
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override;
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;
};

class Bishop : public Piece
{
public:
	Bishop(const string& side, const string& type, int row, int col, int price);
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override;
	bool CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos); 
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;
};

class Knight : public Piece
{
public:
	Knight(const string& side, const string& type, int row, int col, int price);
	void Selected(vector<Position>& possible_moves, const vector<shared_ptr<Piece>>& pieces, bool attack) override;
	void Move(int row, int col) override; 
	bool CanAttack(int row, int col, const vector<shared_ptr<Piece>>& pieces, Position orig_pos);   
	int AddCellPrice(const vector<vector<int>>& cellPrices) override;  
};

#endif // PIECES_H