#pragma once
#include "chess.h"
#include "move.h"
#include "minimax_value.h"
#include "unordered_map"

class GameState
{
public:
	// turn player
	int TurnPlayer = WHITE;


	/// <summary>
	/// Empties board
	/// </summary>
	void empty();

	/// <summary>
	/// Moves a piece
	/// </summary>
	/// <param name="m"></param>
	void make_move(const Move& m);

	/// <summary>
	/// Print board
	/// </summary>
	void print_board() const;

	/// <summary>
	/// Returns all of the moves for turn player
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_raw_moves(int player, vector<Move>& moves) const;

	/// <summary>
	/// Get moves for this piece
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="piece"></param>
	/// <param name="moves"></param>
	void get_piece_moves(int row, int column, int player, int piece, vector<Move>& moves) const;

	/// <summary>
	/// Get moves based on piece
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_rook_moves(int row, int column, int player, vector<Move>& moves) const;
	void get_bishop_moves(int row, int column, int player, vector<Move>& moves) const;
	void get_knight_moves(int row, int column, int player, vector<Move>& moves) const;
	void get_king_moves(int row, int column, int player, vector<Move>& moves) const;
	void get_pawn_moves(int row, int column, int player, vector<Move>& moves) const;


	/// <summary>
	/// Return all possible movements in direction
	/// </summary>
	/// <param name="row">			Starting row				</param>
	/// <param name="column">		Starting column				</param>
	/// <param name="delta_row">	Row change					</param>
	/// <param name="delta_column">	Column change				</param>
	/// <param name="player">		Player enum					</param>
	/// <param name="max_steps">	Max move distance			</param>
	/// <param name="moves">		Filled with moves			</param>
	/// <param name="can_eat">		can eat (true default)		</param>
	/// <param name="has_to_eat">	has to eat (false default	</param>
	void get_raw_moves_in_dir(int row,
		int column,
		int delta_row,
		int delta_column,
		int player,
		int max_steps,
		vector<Move>& moves,
		bool can_eat = true,
		bool has_to_eat = false) const;


	/// <summary>
	/// Creates promotion moves from position
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="delta_row"></param>
	/// <param name="delta_column"></param>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void create_promotion_moves(
		int row,
		int column,
		int delta_row,
		int delta_column,
		int player,
		vector<Move>& moves) const;

	/// <summary>
	/// Finds the position of given piece on board
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	void find_piece(int piece, int& row, int& column) const;

	/// <summary>
	/// Checks if given position is under threat
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="opponent"></param>
	/// <returns></returns>
	bool is_under_threat(int row, int column, int opponent) const;

	/// <summary>
	/// Sets all legal moves into moves vector
	/// </summary>
	/// <param name="moves"></param>
	void get_moves(vector<Move>& moves) const;

	/// <summary>
	/// Sets all legal castle moves into vector
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_castles(int player, vector<Move>& moves) const;

	void update_castle_legality();

	void disable_one_castle(bool& short_castle, bool& long_castle, int column);

	bool can_player_castle(int player) const;


	/// <summary>
	/// Evaluates the board as follow:
	/// 
	/// White made check mate:		1000000
	/// Draw						0
	/// Black made check mate:		-1000000
	/// 
	/// Called when no legal moves are left
	/// </summary>
	/// <returns></returns>
	float score_board() const;

	/// <summary>
	/// Evaluates board state
	/// </summary>
	/// <returns></returns>
	float evaluate() const;

	/// <summary>
	/// Gives the difference between white and black pieces
	/// 
	/// pawn 		1
	/// knight		3
	/// bishop		3.25
	/// rook 		5
	/// queen 		9
	/// </summary>
	/// <returns></returns>
	float material_difference() const;


	/// <summary>
	/// Returns the difference between moves
	/// </summary>
	/// <returns></returns>
	float mobility_difference() const;


	/// <summary>
	/// Performs minimax alorythmn
	/// </summary>
	/// <param name="depth"></param>
	/// <returns></returns>
	MinimaxValue minimax(int depth);

private:
	// Game board
	// [y][x]
	int _board[8][8] = {
		{bR, bN, bB, bQ, bK, bB, bN, bR},
		{bP, bP, bP, bP, bP, bP, bP, bP},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, wP, wP, wP, wP, wP, wP, wP},
		{wR, wN, wB, wQ, wK, wB, wN, wR}
	};

	// Test board
	//int _board[8][8] = {
	//	{bR, NA, NA, NA, bK, NA, NA, bR},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{wR, NA, NA, NA, wK, NA, NA, wR}
	//};


	bool _w_long_castle = true;
	bool _w_short_castle = true;
	bool _b_long_castle = true;
	bool _b_short_castle = true;

	int _kaksoisaskel = -1;

	const unordered_map<int, float> piece_values = {
	{wP, 1.0f}, {wN, 3.0f}, {wB, 3.25}, {wR, 5.0f}, {wQ, 9.0f},
	{bP, -1.0f}, {bN, -3.0f}, {bB, -3.25f}, {bR, -5.0f}, {bQ, -9.0f}, 
	{NA, 0.0f} };
};