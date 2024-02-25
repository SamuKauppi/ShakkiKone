#include "gameState.h"
#include "iostream"
#include "scoring_logic.h"

Evaluation eval = Evaluation();

/*
MinimaxValue GameState::iterative_deepening(TranspositionTable& tt) const
{
	return;
}
*/
MinimaxValue GameState::minimax(int depth, int alpha, int beta, TranspositionTable& tt) const
{
	// Generate moves for this state
	/*
	vector<Move> moves(60);
	get_moves(moves);
	*/

	//test
	vector<Move> moves;
	get_raw_moves(TurnPlayer, moves);
	int index = moves.size();
	get_castles(TurnPlayer, moves, index);
	int legal_moves_made = 0;

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
	
	
	// TODO: Calculate zobrist keys without test state, 
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
	
	bool isMax = TurnPlayer == WHITE ? true : false;

	if (TurnPlayer == BLACK)
	{
		sort(moves.begin(), moves.end());
	}
	else
	{
		sort(moves.begin(), moves.end(), greater<>());
	}
	
	// Get the best_value for player
	int best_value = TurnPlayer == WHITE ?
		numeric_limits<int>::lowest() : numeric_limits<int>::max();

	Move best_move(0, 0, 0, 0);
	// Iterate through moves
	for (Move& m : moves)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// Check move legality during search to avoid unnecessarily checking moves
		// that will be pruned away by alpha beta

		/* old expensive method here only for testing
		auto king_pos = (TurnPlayer == WHITE) ? new_state._wK_pos : new_state._bK_pos;
		if (new_state.is_under_threat(king_pos[0], king_pos[1], 1 - TurnPlayer)) continue;
		*/

		// new method to check if king is in check. extremely fast
		if (new_state.is_king_in_check(TurnPlayer)) continue;
		legal_moves_made++;

		// Recursive call
		MinimaxValue value = new_state.minimax(depth - 1, alpha, beta, tt);

		// Get best value for player
		if ((isMax && value.Value > best_value) ||
			(!isMax && value.Value < best_value))
		{
			best_value = value.Value;
			best_move = m;
			
			if (isMax && best_value > alpha)
			{
				alpha = best_value;
			}
			else if (!isMax && best_value < beta)
			{
				beta = best_value;
			}
			
		}
		tt._positionCount++;
		// store position to TT
		tt.hash_new_position(new_state, depth - 1, value.Value, best_move);
		if (beta <= alpha)
		{
			break;
		}
		
	}

	// tt.hash_new_position(*this, depth, best_value, best_move);
	// no legal moves in branch, game is over
	if (legal_moves_made <= 0)
	{
		return	MinimaxValue(score_board(), Move());
	}
	return MinimaxValue(best_value, best_move);
}

int GameState::score_board() const
{
	// Get opponent
	int opponent = 1 - TurnPlayer;

	// Get worst score for turn player
	int worst_score = TurnPlayer == WHITE ? -1000000 : 1000000;

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
	return 0;
}

int GameState::evaluate() const
{
	return eval.eval(_board, TurnPlayer);
}
