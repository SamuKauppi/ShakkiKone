#include <iostream>
#include <string>
#include "gameState.h"
#include "move.h"

// Piece indecies mapped to array
const string piece_names[] = { "R", "N", "B", "Q", "K", "P", "r", "n", "b", "q", "k", "p", " " };

/// <summary>
/// Empties gameboard
/// </summary>
void GameState::empty()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			_board[i][j] = NA;
		}
	}
}

/// <summary>
/// Moves piece from a to b
/// </summary>
/// <param name="m"></param>
void GameState::make_move(const Move& m)
{
	// Get piece from start pos
	int piece = _board[m._start_pos[0]][m._start_pos[1]];

	// Reset piece at start pos
	_board[m._start_pos[0]][m._start_pos[1]] = NA;

	// Update piece at end pos
	_board[m._end_pos[0]][m._end_pos[1]] = piece;

	_turn_player = 1 - _turn_player;
}

void GameState::get_officer_moves(int row, int column, int player, vector<Move>& moves) const
{
	int piece = _board[row][column];

	if (piece == wR || piece == bR)
	{
		get_rook_moves(row, column, player, moves);
	}
	else if (piece == wB || piece == bB)
	{
		get_bishop_moves(row, column, player, moves);
	}
	else if (piece == wQ || piece == bQ)
	{
		get_rook_moves(row, column, player, moves);
		get_bishop_moves(row, column, player, moves);
	}
	else if (piece == wN || piece == bN)
	{
		get_knight_moves(row, column, player, moves);
	}
	else if (piece == wK || piece == bK)
	{
		get_king_moves(row, column, player, moves);
	}
}

void GameState::get_rook_moves(int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(row, column, -1, 0, player, 7, moves);
	get_raw_moves_in_dir(row, column, 1, 0, player, 7, moves);
	get_raw_moves_in_dir(row, column, 0, -1, player, 7, moves);
	get_raw_moves_in_dir(row, column, 0, 1, player, 7, moves);
}

void GameState::get_bishop_moves(int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(row, column, -1, 1, player, 7, moves);
	get_raw_moves_in_dir(row, column, 1, 1, player, 7, moves);
	get_raw_moves_in_dir(row, column, 1, -1, player, 7, moves);
	get_raw_moves_in_dir(row, column, -1, 1, player, 7, moves);
}

void GameState::get_knight_moves(int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(row, column, 2, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 2, -1, player, 1, moves);
	get_raw_moves_in_dir(row, column, -2, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, -2, -1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, 2, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, -2, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, 2, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, -2, player, 1, moves);
}

void GameState::get_king_moves(int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(row, column, -1, 0, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, 0, player, 1, moves);
	get_raw_moves_in_dir(row, column, 0, -1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 0, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, -1, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, 1, player, 1, moves);
}

void GameState::get_raw_moves_in_dir(int row, int column, int delta_row, int delta_column, 
	int player, int max_steps, vector<Move>& moves) const
{
	// Initilize varibales
	int current_row = row;
	int current_column = column;
	int steps = 0;

	while (steps < max_steps)
	{
		// Make delta move
		current_row += delta_row;
		current_column += delta_column;

		// Is out of board
		if (current_row < 0 ||
			current_row > 7 ||
			current_column < 0 ||
			current_column > 7)
			break;

		// Is empty spot
		if (_board[current_row][current_column] == NA)
		{
			moves.push_back(Move(row, column, current_row, current_column));
			steps++;
			continue;
		}

		// Is same team as player
		if (get_piece_color(_board[current_row][current_column]) == player)
		{
			break;
		}

		// Can eat
		moves.push_back(Move(row, column, current_row, current_column));
		break;
	}
}

/// <summary>
/// Prints the board
/// </summary>
void GameState::print_board() const
{
	cout << "    a   b   c   d   e   f   g   h\n"; // Print file (column) labels

	for (int i = 0; i < 8; i++)
	{
		cout << 8 - i << " "; // Print rank (row) labels
		for (int j = 0; j < 8; j++)
		{
			cout << " | " << piece_names[_board[i][j]];
		}
		cout << " | " << 8 - i << "\n";

	}

	cout << "    a   b   c   d   e   f   g   h\n"; // Print file (column) labels
}

