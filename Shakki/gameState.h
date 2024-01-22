#pragma once
#include "chess.h"
#include "move.h"

class GameState
{
public:
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
	/// Determine raw move for rook
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_rook_moves(int row, int column, int player, int directions,vector<Move>& moves);

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

	// turn player
	int _turn_player = WHITE;

};