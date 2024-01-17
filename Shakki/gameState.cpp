#include <iostream>
#include <string>
#include "gameState.h"
#include "move.h"


// 
const string piece_names[] = { "wR", "wN", "wB", "wQ", "wK", "wP", "bR", "bN", "bB", "bQ", "bK", "bP", "  " };

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
}

/// <summary>
/// Prints the board
/// </summary>
void GameState::print_board() const
{
	for (int i = 0; i < 8; i++)
	{
		cout << "\n";
		for (int j = 0; j < 8; j++)
		{
			cout << "| " << piece_names[_board[i][j]] << " |";
		}
	}
	cout << "\n";
}