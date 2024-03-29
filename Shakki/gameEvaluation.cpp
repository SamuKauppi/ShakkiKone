#include "gameState.h"
#include "iostream"
#include "scoringLogic.h"

static const int MINMAX_DEPTH = 2;
Evaluation eval = Evaluation();

MinimaxValue GameState::iterative_deepening(int alpha, int beta, TranspositionTable& tt) const
{
	eval.update_gamephase(_board);
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

		bool is_best = TurnPlayer == WHITE ? 
			new_value.Value > best_value.Value : new_value.Value < best_value.Value;

		// return best move from a previous finished search if out of time
		// time_limit is set in gamestate header
		if (duration.count() > TimeLimit + TimeExtension && depth > MINMAX_DEPTH)
		{
			best_value.Depth = depth - 1;
			return best_value;
		}

		best_value = new_value;
	}


}

MinimaxValue GameState::minimax(int depth, int startingDepth, int alpha, int beta, TranspositionTable& tt, chrono::steady_clock::time_point timer_start) const
{
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - timer_start);

	if (_repeated_positions->is_draw_by_repetition(current_position_zobrist))
	{
		return MinimaxValue(0, Move(), depth);
	}

	// Reached max depth or time is out
	if (depth <= 0 || duration.count() > TimeLimit + TimeExtension && startingDepth > MINMAX_DEPTH)
	{
		return MinimaxValue(quiescence(alpha, beta), Move(), depth);
	}
	Move tempMoves[200];
	int index = 0;
	get_raw_moves(TurnPlayer, tempMoves, index);
	get_castles(TurnPlayer, tempMoves, index);
	int legal_moves_made = 0;
	Move* moves = new Move[index];

	for (int i = 0; i < index; i++)
	{
		moves[i] = tempMoves[i];
	}

	// If no moves remain, game is over
	if (index <= 0)
	{
		delete[] moves;
		return MinimaxValue(score_board(depth, startingDepth), Move(), depth);
	}


	// TODO: Calculate zobrist keys without test state, 
	for (int i = 0; i < index; i++)
	{
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(moves[i]);

		// order
		uint64_t zobristKey = tt.generate_zobrist_key(new_state);
		moves[i]._key = zobristKey;
		if (tt.is_state_hashed(zobristKey))
		{
			moves[i]._evaluation = tt.get_hashed_evaluation(zobristKey);
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

		// Get king position
		int row, column;
		row = TurnPlayer == WHITE ? new_state._wK_pos[0] : new_state._bK_pos[0];
		column = TurnPlayer == WHITE ? new_state._wK_pos[1] : new_state._bK_pos[1];
		if (new_state.is_square_in_check(TurnPlayer, row, column)) continue;
		legal_moves_made++;

		// Recursive call
		// MinimaxValue value = tt.is_state_calculated(m._key, depth - 1) == true ?
		//	tt.get_value(m._key) : new_state.minimax(depth - 1, startingDepth, alpha, beta, tt, timer_start);
		MinimaxValue value = new_state.minimax(depth - 1, startingDepth, alpha, beta, tt, timer_start);

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
		tt.hash_new_position(m._key, depth - 1, value.Value, value.Best_move);

		if (beta <= alpha)
		{
			break;
		}
	}
	// tt.hash_new_position(*this, depth, best_value, best_move);
	// no legal moves in branch, game is over
	delete[] moves;

	if (legal_moves_made <= 0)
	{
		return MinimaxValue(score_board(depth, startingDepth), Move(), depth);
	}

	return MinimaxValue(best_value, best_move, depth);
}

// Quiescence search to evaluate captures at the end of normal search
int GameState::quiescence(int alpha, int beta) const
{
	// Evaluate this state
	int standPat = evaluate();
	bool isMax = TurnPlayer == WHITE ? true : false;
	if (isMax)
	{
		if (standPat >= beta)
		{
			return standPat;
		}
		if (alpha < standPat) alpha = standPat;
	}
	else
	{
		if (standPat <= alpha)
		{
			return standPat;
		}
		if (beta > standPat) beta = standPat;
	}

	// Generate moves that can capture
	Move tempMoves[200];
	int index = 0;
	get_raw_moves(TurnPlayer, tempMoves, index);
	int legal_moves_made = 0;
	if (index <= 0)
	{
		return score_board(0, 0);
	}
	// Erase empty moves
	Move* moves = new Move[index];
	for (int i = 0; i < index; i++)
	{
		moves[i] = tempMoves[i];
	}

	// Sort moves
	if (TurnPlayer == BLACK)
	{
		sort(moves, moves + index);
	}
	else
	{
		sort(moves, moves + index, greater<>());
	}

	int best_value = TurnPlayer == WHITE ?
		numeric_limits<int>::lowest() : numeric_limits<int>::max();

	for (int i = 0; i < index; i++)
	{
		Move m = moves[i];
		if (m.capture == false) continue;
		// Create copies of current state
		GameState new_state = *this;
		new_state.make_move(m);

		// Get king position
		int row, column;
		row = TurnPlayer == WHITE ? new_state._wK_pos[0] : new_state._bK_pos[0];
		column = TurnPlayer == WHITE ? new_state._wK_pos[1] : new_state._bK_pos[1];
		if (new_state.is_square_in_check(TurnPlayer, row, column)) continue;
		legal_moves_made++;
		int value = new_state.quiescence(alpha, beta);

		// Alpha beta
		if (isMax)
		{
			if (value > best_value) best_value = value;
			if (best_value >= beta)
			{
				delete[] moves;
				return best_value;
			}
			if (best_value > alpha)
				alpha = best_value;
		}
		else
		{
			if (value < best_value) best_value = value;
			if (best_value <= alpha)
			{
				delete[] moves;
				return best_value;
			}
			if (best_value < beta)
				beta = best_value;
		}

	}

	// Return best value
	delete[] moves;
	if (legal_moves_made <= 0)
	{
		return standPat;
	}
	return best_value;
}

int GameState::score_board(int depth, int startingDepth) const
{
	// Get opponent
	int opponent = 1 - TurnPlayer;

	// Get worst score for turn player
	int depthDifference = startingDepth - depth;
	int worst_score = TurnPlayer == WHITE ? -1000000 + depthDifference : 1000000 - depthDifference;
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
	return eval.eval(_board, TurnPlayer);
}
