#include "gameState.h"
#include "iostream"
#include "scoring_logic.h"
#include "future";

Evaluation eval = Evaluation();

MinimaxValue GameState::iterative_deepening(int alpha, int beta, TranspositionTable& tt) const
{
	MinimaxValue best_value(TurnPlayer == WHITE ?
		numeric_limits<int>::lowest() : numeric_limits<int>::max(), Move(), 0);
	chrono::steady_clock::time_point timer_start = chrono::high_resolution_clock::now();

	// currently searching at 2s increment only because player turn effects evaluation wildly slowing down calculation (read horizon effect)
	// TODO implement quiencense search to fix this issue and fix horizon effect
	for (int depth = 4; depth < 100; depth += 2)
	{
		cout << depth << ", ";
		// calculate position at new depth
		MinimaxValue new_value = minimax(depth, alpha, beta, tt, timer_start);

		auto stop = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - timer_start);

		// return best move from a previous finished search if out of time
		// time_limit is set in gamestate header
		if (duration.count() > time_limit)
		{
			best_value.Depth = depth-2;
			return best_value;
		}
		// set new best move if we finished searhing at a new depth
		best_value = new_value;
	}
}

MinimaxValue GameState::minimax(int depth, int alpha, int beta, TranspositionTable& tt, chrono::steady_clock::time_point timer_start) const
{

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - timer_start);

	vector<Move> moves(50);
	get_raw_moves(TurnPlayer, moves);
	int index = (int)moves.size();
	get_castles(TurnPlayer, moves, index);
	int legal_moves_made = 0;

	// If no moves remain, game is over
	if ((int)moves.size() <= 0)
	{

		return MinimaxValue(score_board(), Move(), depth);
	}

	// Reached max depth or time is out
	if (depth <= 0 || duration.count() > time_limit)
	{
		return MinimaxValue(evaluate(), Move(), depth);
	}
	
	
	// TODO: Calculate zobrist keys without test state, 
	for (Move& m : moves)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// order
		uint64_t zobristKey = tt.generate_zobrist_key(new_state);
		m._key = zobristKey;
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
		// Get king position
		int row, column;
		row = TurnPlayer == WHITE ? new_state._wK_pos[0] : new_state._bK_pos[0];
		column = TurnPlayer == WHITE ? new_state._wK_pos[1] : new_state._bK_pos[1];
		if (new_state.is_square_in_check(TurnPlayer, row, column)) continue;
		legal_moves_made++;
		// Recursive call
		// MinimaxValue value = tt.is_state_calculated(m._key, depth - 1) == true ?
		//	tt.get_value(m._key) : new_state.minimax(depth - 1, alpha, beta, tt, timer_start);
		MinimaxValue value = new_state.minimax(depth - 1, alpha, beta, tt, timer_start);

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
		tt.hash_new_position(new_state, depth - 1, value.Value, value.Best_move);
		if (beta <= alpha)
		{
			break;
		}
		
	}
	// tt.hash_new_position(*this, depth, best_value, best_move);
	// no legal moves in branch, game is over
	if (legal_moves_made <= 0)
	{
		return MinimaxValue(score_board(), Move(), depth);
	}
	return MinimaxValue(best_value, best_move, depth);
}

int GameState::score_board() const
{
	// Get opponent
	int opponent = 1 - TurnPlayer;

	// Get worst score for turn player
	int worst_score = TurnPlayer == WHITE ? -1000000 : 1000000;

	// Find correct king
	int row, column;
	row = TurnPlayer == WHITE ? _wK_pos[0] : _bK_pos[0];
	column = TurnPlayer == WHITE ? _wK_pos[1] : _bK_pos[1];

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
	return eval.eval(_board);
}
