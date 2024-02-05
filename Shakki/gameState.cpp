#include <iostream>
#include <string>
#include "gameState.h"
#include "move.h"

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

void GameState::make_move(const Move& m)
{
	int piece;
	// Get piece from start pos
	if (m._piece_promotion == NA)
	{
		piece = _board[m._start_pos[0]][m._start_pos[1]];
	}
	// Get promoted piece
	else
	{
		piece = m._piece_promotion;
	}

	// Reset piece at start pos
	_board[m._start_pos[0]][m._start_pos[1]] = NA;

	// Update piece at end pos
	_board[m._end_pos[0]][m._end_pos[1]] = piece;

	TurnPlayer = 1 - TurnPlayer;
}

void GameState::get_all_moves(int player, vector<Move>& moves) const
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			// Get piece index
			int piece = _board[i][j];

			// Compare piece to player to piece index
			// White gets only white moves and vice versa
			if ((player == WHITE && piece < 6) ||
				(player == BLACK && (piece >= 6 && piece != 12)))
			{
				get_piece_moves(i, j, player, piece, moves);
			}
		}
	}
}

void GameState::give_moves(vector<Move>& moves) {

	int king = TurnPlayer == WHITE ? wK : bK;
	int player = TurnPlayer;
	int opponent = 1 - TurnPlayer;

	vector<Move> rawMoves;
	get_all_moves(player, rawMoves);

	for (Move& rm : rawMoves) {
		GameState testState = *this;

		testState.make_move(rm);

		int row, column;
		testState.find_piece(king, row, column);

		if (!testState.is_under_threat(row, column, opponent)) {
			moves.push_back(rm);
		}

	}
}

void GameState::create_promotion_moves(
	int row,
	int column,
	int delta_row,
	int delta_column,
	int player,
	vector<Move>& moves) const
{
	int player_offset = player == BLACK ? 6 : 0;
	for (int i = 0; i < 4; i++)
	{
		int piece = i + player_offset;
		Move m = Move(row, column, delta_row, delta_column);
		m._piece_promotion = piece;
		moves.push_back(m);
	}
}

void GameState::get_piece_moves(int row, int column, int player, int piece, vector<Move>& moves) const
{
	switch (piece)
	{
	case wR:
	case bR:
		get_rook_moves(row, column, player, moves);
		break;

	case wB:
	case bB:
		get_bishop_moves(row, column, player, moves);
		break;

	case wQ:
	case bQ:
		get_rook_moves(row, column, player, moves);
		get_bishop_moves(row, column, player, moves);
		break;

	case wN:
	case bN:
		get_knight_moves(row, column, player, moves);
		break;

	case wK:
	case bK:
		get_king_moves(row, column, player, moves);
		break;

	case wP:
	case bP:
		get_pawn_moves(row, column, player, moves);
		break;

	default:
		break;
	}
}

void GameState::get_pawn_moves(int row, int column, int player, vector<Move>& moves) const
{
	// Direction based on player
	int direction = player == WHITE ? -1 : 1;

	// Eating
	get_raw_moves_in_dir(row, column, direction, 1, player, 1, moves, true, true);
	get_raw_moves_in_dir(row, column, direction, -1, player, 1, moves, true, true);

	// If in starting pos, 2 directly forward
	if (row == 6 || row == 1)
	{
		get_raw_moves_in_dir(row, column, direction, 0, player, 2, moves, false);
	}
	// 1 directly forward
	else
	{
		get_raw_moves_in_dir(row, column, direction, 0, player, 1, moves, false);
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
	int player, int max_steps, vector<Move>& moves, bool can_eat, bool has_to_eat) const
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

		// Check if pawn piece will promote
		bool will_promote = false;
		if ((current_row == 0 || current_row == 7) &&
			(_board[row][column] == wP || _board[row][column] == bP))
		{
			will_promote = true;
		}

		// Is empty spot
		if (_board[current_row][current_column] == NA)
		{
			if (!has_to_eat)
			{
				if (will_promote)
					create_promotion_moves(row, column, current_row, current_column, player, moves);
				else
					moves.push_back(Move(row, column, current_row, current_column));
			}

			steps++;
			continue;
		}

		// Is same team as player
		if (get_piece_color(_board[current_row][current_column]) == player)
		{
			break;
		}

		// Can eat
		if (can_eat)
		{
			if (will_promote)
				create_promotion_moves(row, column, current_row, current_column, player, moves);
			else
				moves.push_back(Move(row, column, current_row, current_column));
		}

		break;
	}
}

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

bool GameState::is_under_threat(int row, int column, int opponent) const
{
	// Generate opponents raw moves
	vector<Move> opponentMoves;
	get_all_moves(opponent, opponentMoves);


	// check if any end position matches row and column
	for (int i = 0; i < opponentMoves.size(); i++)
	{
		if (opponentMoves[i]._end_pos[0] == row && opponentMoves[i]._end_pos[1] == column) {
			return true;
		}
	}
	return false;

}

void GameState::print_board() const
{
	cout << "     a   b   c   d   e   f   g   h\n"; // Print file (column) labels
	cout << "   |---|---|---|---|---|---|---|---|\n";

	for (int i = 0; i < 8; i++)
	{
		cout << 8 - i << " "; // Print rank (row) labels
		for (int j = 0; j < 8; j++)
		{
			cout << " | " << piece_names[_board[i][j]];
		}
		cout << " | " << 8 - i << "\n";
		cout << "   |---|---|---|---|---|---|---|---|\n";
	}

	cout << "     a   b   c   d   e   f   g   h\n"; // Print file (column) labels
}

