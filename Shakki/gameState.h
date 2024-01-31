#pragma once
#include "chess.h"
#include "move.h"

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


	void get_all_moves(int player, vector<Move>& moves) const;

	void get_piece_moves(int row, int column, int player, int piece, vector<Move>& moves) const;
	/// <summary>
	/// Determine raw move for rook
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
	/// <param name="row">			Starting row		</param>
	/// <param name="column">		Starting column		</param>
	/// <param name="delta_row">	Row change			</param>
	/// <param name="delta_column">	Column change		</param>
	/// <param name="player">		Player enum			</param>
	/// <param name="max_steps">	Max move distance	</param>
	/// <param name="moves">		Filled with moves	</param>
	void get_raw_moves_in_dir(int row, 
		int column, 
		int delta_row, 
		int delta_column, 
		int player,
		int max_steps, 
		vector<Move>& moves,
		bool can_eat = true,
		bool has_to_eat = false) const;

	void create_promotion_moves(
		int row, 
		int column, 
		int delta_row, 
		int delta_column, 
		int player, 
		vector<Move>& moves) const;

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
		{wR, wN, wB, wQ, wK, wB, wN, wR},
	};


};