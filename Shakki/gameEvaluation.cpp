#include "gameState.h"
#include "iostream"
#include "scoringLogic.h"
#include "future"

static const int MINMAX_DEPTH = 6;
Evaluation eval = Evaluation();

MinimaxValue GameState::iterative_deepening(int alpha, int beta, TranspositionTable& tt) const
{
	MinimaxValue best_value(TurnPlayer == WHITE ?
		numeric_limits<int>::lowest() : numeric_limits<int>::max(), Move(), 0);
	chrono::steady_clock::time_point timer_start = chrono::high_resolution_clock::now();
	// TODO implement quiencense search to fix horizon effect and allow better utilization of TT
	for (int depth = MINMAX_DEPTH; depth < 100; depth++)
	{
		// calculate position at new depth
		MinimaxValue new_value = minimax(depth, depth, alpha, beta, tt, timer_start);

		auto stop = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - timer_start);

		// return best move from a previous finished search if out of time
		// time_limit is set in gamestate header
		if (duration.count() > TimeLimit && depth > MINMAX_DEPTH)
		{
			best_value.Depth = depth-1;
			return best_value;
		}
		// set new best move if we finished searhing at a new depth
		best_value = new_value;
	}
}

MinimaxValue GameState::minimax(int depth, int startingDepth, int alpha, int beta, TranspositionTable& tt, chrono::steady_clock::time_point timer_start) const
{
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - timer_start);

	// Reached max depth or time is out
	if (depth <= 0 || duration.count() > TimeLimit && startingDepth > MINMAX_DEPTH)
	{
		return MinimaxValue(evaluate(), Move(), depth);
	}

	Move tempMoves[200];
	int index = 0;
	get_raw_moves(TurnPlayer, tempMoves, index);
	get_castles(TurnPlayer, tempMoves, index);
	int legal_moves_made = 0;
	Move *moves = new Move[index];

	for (int i = 0; i < index; i++)
	{
		moves[i] = tempMoves[i];
	}

	// If no moves remain, game is over
	if (index <= 0)
	{
		delete moves;
		return MinimaxValue(score_board(), Move(), depth);
	}
	
	
	// TODO: Calculate zobrist keys without test state, 
	for (int i = 0; i < index; i++)
	{
		Move m = moves[i];
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
		sort(moves, moves + index);
	}
	else
	{
		sort(moves, moves + index, greater<>());
	}
	
	// Get the best_value for player
	int best_value = TurnPlayer == WHITE ?
		numeric_limits<int>::lowest() : numeric_limits<int>::max();

	Move best_move(0, 0, 0, 0);
	// Iterate through moves
	for (int i = 0; i < index; i++)
	{
		Move m = moves[i];
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

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
		MinimaxValue value = new_state.minimax(depth - 1, startingDepth, alpha, beta, tt, timer_start);

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
	delete moves;

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

	// Return worst score if �t's under threat
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
