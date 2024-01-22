#include <iostream>
#include <string>
#include "gameState.h"
#include "move.h"


// Piece indecies mapped to array
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
/// Get the raw moves of the rook
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="player"></param>
/// <param name="directions"></param>
/// <param name="moves"></param>
void GameState::get_rook_moves(int row, int column, int player, int directions, vector<Move>& moves)
{
	for (int i = 0; i < directions; i++)
	{
		int current_row = row;
		int current_column = column;
		int direction = i % 2 == 0 ? 1 : -1;

		while (true)
		{
			if (current_row < 0 ||
				current_row > size(_board) - 1 ||
				current_column < 0 ||
				current_column > size(_board) - 1)
				break;

			if (i + 1 % 2 == 0)
			{
				current_row += direction;
			}
			else
			{
				current_column += direction;
			}

			if (_board[current_row][current_column] == NA)
			{
				moves.push_back(Move(row, column, current_row, current_column));
				continue;
			}

			if (get_piece_color(_board[current_row][current_column]) == player)
			{
				break;
			}

			moves.push_back(Move(row, column, current_row, current_column));
			break;
		}
	}
}

/// <summary>
/// Prints the board
/// </summary>
void GameState::print_board() const
{
	cout << "     a    b    c    d    e    f    g    h\n"; // Print file (column) labels

	for (int i = 0; i < 8; i++)
	{

		cout << 8 - i << " "; // Print rank (row) labels
		for (int j = 0; j < 8; j++)
		{
			cout << " | " << piece_names[_board[i][j]];
		}
		cout << " | " << 8 - i << "\n";

	}

	cout << "     a    b    c    d    e    f    g    h\n"; // Print file (column) labels
}

