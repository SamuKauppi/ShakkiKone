#include <iostream>
#include <string>
#include <math.h>
#include "gameState.h"
#include "move.h"
#include "minimaxValue.h"

void GameState::find_piece(int piece, int& row, int& column) const
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int p = _board[i][j];
			if (p == piece)
			{
				row = i;
				column = j;
				return;
			}
		}
	}
}

void GameState::update_castle_legality()
{
	// Check if castles can be performed
	if (!can_player_castle(WHITE) && !can_player_castle(BLACK))
	{
		return;
	}

	for (int i = 0; i < 6; i++)
	{
		// Get alternating row
		int row = i % 2 == 0 ? 0 : 7;
		// Get player index based on row
		int player = row > 0 ? WHITE : BLACK;

		// Skip if this player can't castle
		if (!can_player_castle(player))
		{
			continue;
		}

		// Check if king has been moved
		if (i < 2)
		{
			// Get piece at king pos
			int king = _board[row][4];

			// disable castles if no king was found
			if (row == 0 && king != bK)
			{
				_b_long_castle = false;
				_b_short_castle = false;
			}
			else if (row == 7 && king != wK)
			{
				_w_long_castle = false;
				_w_short_castle = false;
			}
		}
		// Check if rook has been moved
		else
		{
			// check first column index 0 twice and then 7 twice
			int column = i < 4 ? 0 : 7;

			// get piece at pos
			int piece = _board[row][column];

			// disable right castle if no rook was found
			if (piece != wR && row == 7)
			{
				disable_one_castle(_w_short_castle, _w_long_castle, column);
			}
			else if (piece != bR && row == 0)
			{
				disable_one_castle(_b_short_castle, _b_long_castle, column);
			}
		}

	}
}

void GameState::disable_one_castle(bool& short_castle, bool& long_castle, int column)
{
	if (column == 0 && long_castle)
	{
		long_castle = false;
	}
	else if (column == 7 && short_castle)
	{
		short_castle = false;
	}
}

bool GameState::can_player_castle(int player) const
{
	if (player == WHITE && (_w_short_castle || _w_long_castle))
	{
		return true;
	}
	else if (player == BLACK && (_b_short_castle || _b_long_castle))
	{
		return true;
	}

	return false;
}

void GameState::print_board() const
{
	cout << "     a   b   c   d   e   f   g   h\n"; // Print file (column) labels
	cout << "   ----+---+---+---+---+---+---+----\n";

	for (int i = 0; i < 8; i++)
	{
		cout << 8 - i << " "; // Print rank (row) labels
		for (int j = 0; j < 8; j++)
		{
			cout << " | " << piece_names[_board[i][j]];
		}
		cout << " | " << 8 - i << "\n";
		if (i < 7)
			cout << "   |---+---+---+---+---+---+---+---|\n";
	}

	cout << "   ----+---+---+---+---+---+---+----\n";
	cout << "     a   b   c   d   e   f   g   h\n"; // Print file (column) labels
}

int GameState::get_simple_piece_value(int piece) const
{
	switch (piece)
	{
	case wP:
		return 100;
	case bP:
		return 100;
	case wN:
		return 300;
	case bN:
		return 300;
	case wB:
		return 350;
	case bB:
		return 350;
	case wQ:
		return 800;
	case bQ:
		return 800;
	}
}