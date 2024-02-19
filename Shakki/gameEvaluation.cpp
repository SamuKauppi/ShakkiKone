#include "gameState.h"
#include "scoring_logic.h"
#include "iostream"

MinimaxValue GameState::minimax(int depth, float alpha, float beta, TranspositionTable& tt) const
{
	// Generate moves for this state
	vector<Move> moves(60);
	get_moves(moves);

	for (Move& m : moves)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// order
		uint64_t zobristKey = tt.generate_zobrist_key(new_state);
		if (tt.is_state_hashed(zobristKey)) 
		{
			m._evaluation = tt.get_hashed_evaluation(zobristKey);
		}
	}
	
	sort(moves.begin(), moves.end());

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
		numeric_limits<float>::lowest() : numeric_limits<float>::max();

	Move best_move(0, 0, 0, 0);
	bool isMin = TurnPlayer == WHITE ? true : false;

	// Iterate through moves
	for (Move& m : moves)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// Compare hash value
		// Recursive call
		MinimaxValue value = new_state.minimax(depth - 1, alpha, beta, tt);

		// Get best value for player
		if ((isMin && value.Value > best_value) ||
			(!isMin && value.Value < best_value))
		{
			best_value = value.Value;
			best_move = m;

			if (isMin)
			{
				alpha = best_value;
			}
			else
			{
				beta = best_value;
			}
		}

		// store position to TT
		tt.hash_new_position(new_state, depth, best_value, best_move);

		if (beta <= alpha)
		{
			break;
		}
	}

	return MinimaxValue(best_value, best_move);
}

float GameState::score_board() const
{
	// Get opponent
	int opponent = 1 - TurnPlayer;

	// Get worst score for turn player
	float worst_score = TurnPlayer == WHITE ? -1000000.0f : 1000000.0f;

	// Find correct king
	int row, column;

	if (TurnPlayer == WHITE)
	{
		row = _wK_pos[0];
		column = _wK_pos[1];
	}
	else
	{
		row = _bK_pos[0];
		column = _bK_pos[1];
	}

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
	return
		1.0f * material_difference() +
		0.05f * mobility_difference() +
		0.2f * castle_difference() +
		0.75f * check_difference();
}

int GameState::mobility_difference() const
{
	vector<Move> white_m(100);
	vector<Move> black_m(100);

	get_raw_moves(WHITE, white_m);
	get_raw_moves(BLACK, black_m);

	return white_m.size() - black_m.size();
}

float GameState::castle_difference() const
{
	float score = 0.0f;
	score += evaluate_player_castle(_w_castle, _w_short_castle, _w_long_castle);
	score -= evaluate_player_castle(_b_castle, _b_short_castle, _b_long_castle);
	return score;
}
float GameState::evaluate_player_castle(bool done_castle, bool can_short, bool can_long) const
{
	float score = 0.0f;

	// Player has done a castle
	if (done_castle)
	{
		score += 10.0f;
	}
	// Player hasn't done castle
	else
	{
		// If short castle has been disabled
		if (!can_short)
		{
			score -= 25.0f;
		}
		// If long castle has been disabled
		if (!can_long)
		{
			score -= 20.0f;
		}
	}
	return score;
}

float GameState::material_difference() const
{
	float value = 0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int piece = _board[i][j];
			if (piece == NA)
				continue;

			auto it = piece_values.find(piece);
			if (it != piece_values.end())
			{
				value += evaluate_piece_at_pos(piece, it->second, i, j);
			}
		}
	}

	return value;
}
float GameState::evaluate_piece_at_pos(int piece, float piece_value, int row, int column) const
{
	switch (piece)
	{
	case wP:
	case bP:
		return evaluate_pawn_at_pos(piece_value, row, column);

	case wR:
	case bR:
		return evaluate_rook_at_pos(piece_value, row, column);

	case wN:
	case bN:
		return evaluate_knight_at_pos(piece_value, row, column);

	case wB:
	case bB:
		return evaluate_bishop_at_pos(piece_value, row, column);

	case wQ:
	case bQ:
		return evaluate_queen_at_pos(piece_value, row, column);

	case wK:
	case bK:
		return evaluate_king_at_pos(piece_value, row, column);

	default:
		return -1.0f;
	}

}
float GameState::evaluate_pawn_at_pos(float piece_value, int row, int column) const
{

}
float GameState::evaluate_rook_at_pos(float piece_value, int row, int column) const
{

}
float GameState::evaluate_bishop_at_pos(float piece_value, int row, int column) const
{

}
float GameState::evaluate_knight_at_pos(float piece_value, int row, int column) const
{

}
float GameState::evaluate_queen_at_pos(float piece_value, int row, int column) const
{

}
float GameState::evaluate_king_at_pos(float piece_value, int row, int column) const
{

}


float GameState::check_difference() const
{
	if (is_under_threat(_wK_pos[0], _wK_pos[1], BLACK))
	{
		return 15.0f;
	}

	if (is_under_threat(_bK_pos[0], _bK_pos[1], WHITE))
	{
		return -15.0f;
	}

	return 0.0f;
}