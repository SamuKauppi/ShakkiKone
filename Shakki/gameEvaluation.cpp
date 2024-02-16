#include "gameState.h"

void GameState::generate_hash()
{
	//unsigned long long int test = 1;
	//test ^= 3;
	//int squared = 8 * 8;
	//for (int i = 0; i < 8; i++)
	//{
	//	for (int j = 0; j < 8; j++)
	//	{
	//		_hash += pow(2, squared) * _board[i][j];
	//		squared--;
	//	}
	//}
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
			if (piece == NA)
				continue;

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
	vector<Move> white_m(100);
	vector<Move> black_m(100);

	get_raw_moves(WHITE, white_m);
	get_raw_moves(BLACK, black_m);

	return (float)(white_m.size() - black_m.size());
}

MinimaxValue GameState::minimax(int depth, float alpha, float beta) const
{
	// Generate moves for this state
	vector<Move> moves(60);
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
		MinimaxValue value = new_state.minimax(depth - 1, alpha, beta);

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

		if (beta <= alpha)
		{
			break;
		}
	}

	return MinimaxValue(best_value, best_move);
}
