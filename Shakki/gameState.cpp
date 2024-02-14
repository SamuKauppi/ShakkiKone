#include <iostream>
#include <string>
#include <math.h>
#include <unordered_map>
#include "gameState.h"
#include "move.h"
#include "minimax_value.h"

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
	// Create variables
	int start_row = m._start_pos[0];
	int start_column = m._start_pos[1];
	int end_row = m._end_pos[0];
	int end_column = m._end_pos[1];
	int piece;

	// Get piece from start pos
	if (m._piece_promotion == NA)
	{
		piece = _board[start_row][start_column];
	}
	// Get promoted piece
	else
	{
		piece = m._piece_promotion;
	}

	// Reset piece at start pos
	_board[start_row][start_column] = NA;

	// Update piece at end pos
	_board[end_row][end_column] = piece;

	// Move was a castle if king moves more than 1 step
	if ((piece == wK || piece == bK) &&
		abs(end_column - start_column) > 1)
	{
		// get the direction for the rook
		int rook_dir = end_column - start_column;
		// get column where rook will go
		int rook_column = rook_dir > 0 ? 5 : 3;
		// get the corner where the rook starts
		int corner = rook_dir > 0 ? 7 : 0;

		// get the rook index
		int rook = _board[start_row][corner];
		
		// empty the corner spot
		_board[start_row][corner] = NA;

		// add rook to end pos
		_board[start_row][rook_column] = rook;
	}

	update_castle_legality();

	// switch players
	TurnPlayer = 1 - TurnPlayer;
}

void GameState::get_raw_moves(int player, vector<Move>& moves) const
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

void GameState::get_castles(int player, vector<Move>& moves) const 
{
	// Get opponent
	int opponent = 1 - player;

	// Get the player position
	int player_pos = player == WHITE ? 7 : 0;

	// check if short castle is allowed
	bool can_short = _board[player_pos][5] == NA && _board[player_pos][6] == NA &&
		((player == WHITE && _w_short_castle) || (player == BLACK && _b_short_castle));

	// check if long castle is allowed
	bool can_long = _board[player_pos][3] == NA && _board[player_pos][2] == NA && _board[player_pos][1] == NA &&
		((player == WHITE && _w_long_castle) || (player == BLACK && _b_long_castle));

	if (can_short && 
		!is_under_threat(player_pos, 4, opponent) && 
		!is_under_threat(player_pos, 5, opponent) &&
		!is_under_threat(player_pos, 6, opponent))
	{
		moves.push_back(Move(player_pos, 4, player_pos, 6));
	}

	if (can_long && 
		!is_under_threat(player_pos, 4, opponent) &&
		!is_under_threat(player_pos, 3, opponent) && 
		!is_under_threat(player_pos, 2, opponent))
	{
		moves.push_back(Move(player_pos, 4, player_pos, 2));
	}
}

void GameState::get_moves(vector<Move>& moves) const 
{

	int king = TurnPlayer == WHITE ? wK : bK;
	int player = TurnPlayer;
	int opponent = 1 - TurnPlayer;

	vector<Move> rawMoves;

	get_raw_moves(player, rawMoves);
	get_castles(player, moves);

	for (Move& rm : rawMoves) 
	{
		GameState testState = *this;

		testState.make_move(rm);

		int row, column;
		testState.find_piece(king, row, column);

		if (!testState.is_under_threat(row, column, opponent)) 
		{
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
	get_raw_moves_in_dir(row, column, -1, -1, player, 7, moves);
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
	get_raw_moves_in_dir(row, column, 1, 0, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, 0, player, 1, moves);
	get_raw_moves_in_dir(row, column, 0, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 0, -1, player, 1, moves);
	get_raw_moves_in_dir(row, column, 1, 1, player, 1, moves);
	get_raw_moves_in_dir(row, column, -1, -1, player, 1, moves);
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
	get_raw_moves(opponent, opponentMoves);


	// check if any end position matches row and column
	for (int i = 0; i < opponentMoves.size(); i++)
	{
		if (opponentMoves[i]._end_pos[0] == row && opponentMoves[i]._end_pos[1] == column) {
			return true;
		}
	}
	return false;

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
	else if ( player == BLACK && (_b_short_castle || _b_long_castle))
	{
		return true;
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

float GameState::score_board() const
{
	// Get opponent
	int opponent = 1 - TurnPlayer;

	// Get worst score for turn player
	float worst_score = TurnPlayer == WHITE ? -1000000.0f : 1000000.0f;

	// Find correct king
	int row, column;
	int king = TurnPlayer == WHITE ? wK : bK;
	find_piece(king, row, column);

	// Return worst score if ít's under threat
	if (is_under_threat(row, column, opponent))
	{
		return worst_score;
	}

	// Return draw if the king is not under threat
	return 0.0f;
}

float GameState::evaluate() const
{
	return 1.0f * material_difference() + 0.05f * mobility_difference();
}

float GameState::material_difference() const
{
	float value = 0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int piece = _board[i][j];
			auto it = piece_values.find(piece);
			if (it != piece_values.end()) 
			{
				value += it->second;
			}
		}
	}

	return value;
}

float GameState::mobility_difference() const
{
	vector<Move> white_m;
	vector<Move> black_m;

	get_raw_moves(WHITE, white_m);
	get_raw_moves(BLACK, white_m);


	return (float)(white_m.size() - black_m.size());
}

MinimaxValue GameState::minimax(int depth)
{
	// Generate moves for this state
	vector<Move> moves;
	get_moves(moves);

	// If no moves remain, game is over
	if (moves.size() <= 0)
	{
		return	MinimaxValue(score_board(), Move());
	}

	// Reached max depth
	if (depth <= 0)
	{
		return MinimaxValue(evaluate(), Move());
	}

	// Get the best_value for player
	float best_value = TurnPlayer == WHITE ?
		numeric_limits<float>::min() : numeric_limits<float>::max();

	Move best_move(0, 0, 0, 0);

	// Iterate through moves
	for (Move& m : moves)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// Recursive call
		MinimaxValue value = new_state.minimax(depth - 1);

		// Get best value for player
		if (TurnPlayer == WHITE && value.Value > best_value)
		{
			best_value = value.Value;
			best_move = m;
		}
		else if (TurnPlayer == BLACK && value.Value < best_value)
		{
			best_value = value.Value;
			best_move = m;
		}
	}

	return MinimaxValue(best_value, best_move);
}
