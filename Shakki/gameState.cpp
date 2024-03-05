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

	// Iterate through both players
	for (int player = 0; player <= BLACK; player++)
	{
		// Check if this player can castle
		if (!can_player_castle(player))
			continue;

		// Get player row and correct king
		int row = player == WHITE ? 7 : 0;
		int player_king = player == WHITE ? wK : bK;
		
		// Set refrenece variables to edit the original values
		bool& short_castle = player == WHITE ? _w_short_castle : _b_short_castle;
		bool& long_castle = player == WHITE ? _w_long_castle: _b_long_castle;

		// Check if king is moved
		if (_board[row][4] != player_king)
		{
			short_castle = false;
			long_castle = false;
			continue;
		}

		// Check if any of rooks have been moved
		int player_rook = player == WHITE ? wR : bR;
		if (_board[row][7] != player_rook)
		{
			short_castle = false;
		}
		if (_board[row][0] != player_rook)
		{
			long_castle = false;
		}

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