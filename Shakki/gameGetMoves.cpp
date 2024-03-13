#include "gameState.h"
#include <iostream>

void GameState::get_moves(vector<Move>& moves, TranspositionTable& tt) const
{
	// Get the Players
	int player = TurnPlayer;
	int opponent = 1 - TurnPlayer;

	// Generate raw moves
	Move rawMovesTemp[200];
	int moveIndex = 0;
	get_raw_moves(player, rawMovesTemp, moveIndex);

	// Add castle moves into moves (validates if it's legal on it's own)
	get_castles(player, rawMovesTemp, moveIndex);

	// Iterate through moves
	for (int i = 0; i < moveIndex; i++)
	{
		// Generate test state and test a move
		GameState testState = *this;
		testState.make_move(rawMovesTemp[i]);

		// Get king position
		int row, column;
		if (player == WHITE)
		{
			row = testState._wK_pos[0];
			column = testState._wK_pos[1];
		}
		else
		{
			row = testState._bK_pos[0];
			column = testState._bK_pos[1];
		}

		// If king is not under threat, the move is legal
		if (!testState.is_square_in_check(TurnPlayer ,row, column))
		{
			rawMovesTemp[i]._key = tt.generate_zobrist_key(testState);
			moves.push_back(rawMovesTemp[i]);
		}
	}
}

void GameState::get_raw_moves(int player, Move moves[], int& moveIndex) const
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			// Get piece index
			int piece = _board[i][j];
			if (piece == NA)
				continue;

			// Compare piece to player to piece index
			// White gets only white moves and vice versa
			if ((player == WHITE && get_piece_color(piece) == WHITE) ||
				(player == BLACK && get_piece_color(piece) == BLACK))
			{
				get_piece_moves(moveIndex, i, j, player, piece, moves);
			}
		}
	}
}

void GameState::get_castles(int player, Move moves[], int& moveIndex) const
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
		!is_square_in_check(player, player_pos, 4) &&
		!is_square_in_check(player, player_pos, 5) &&
		!is_square_in_check(player, player_pos, 6))
	{
		add_move_with_index(moveIndex, player_pos, 4, player_pos, 6, player, moves);
	}

	if (can_long &&
		!is_square_in_check(player, player_pos, 4) &&
		!is_square_in_check(player, player_pos, 3) &&
		!is_square_in_check(player, player_pos, 2))
	{
		add_move_with_index(moveIndex, player_pos, 4, player_pos, 2, player, moves);
	}
}

void GameState::get_promotion_moves(
	int& moveIndex,
	int row,
	int column,
	int delta_row,
	int delta_column,
	int player,
	Move moves[],
	bool capture) const
{
	int player_offset = player == BLACK ? 6 : 0;
	for (int i = 0; i < 4; i++)
	{
		int piece = i + player_offset;
		add_move_with_index(moveIndex, row, column, delta_row, delta_column, player, moves, capture, piece);
	}
}

void GameState::get_raw_moves_in_dir(int& moveIndex, int row, int column, int delta_row, int delta_column,
	int player, int max_steps, Move moves[], bool can_eat, bool has_to_eat) const
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
					get_promotion_moves(moveIndex, row, column, current_row, current_column, player, moves);
				else
				{
					add_move_with_index(moveIndex, row, column, current_row, current_column, player, moves);
				}
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
				get_promotion_moves(moveIndex, row, column, current_row, current_column, player, moves, true);
			else
			{
				add_move_with_index(moveIndex, row, column, current_row, current_column, player, moves, true);
			}
		}

		break;
	}
}

void GameState::add_move_with_index(int& moveIndex, int row, int column, int delta_row, int delta_column, int player, Move moves[], bool capture, int promo) const
{
	moves[moveIndex] = Move(row, column, delta_row, delta_column, player);
	
	if (promo != NA)
		moves[moveIndex]._piece_promotion = promo;

	if (capture)
	{
		if (_board[delta_row][delta_column] == wK || _board[delta_row][delta_column] == bK)
		{
			moveIndex++;
			return;
		}
		moves[moveIndex].capture = true;
		if (player == WHITE)
		{
			moves[moveIndex].pieceValueDifferential = get_simple_piece_value(_board[delta_row][delta_column]) - get_simple_piece_value(_board[row][column]);
		}
		else
		{
			moves[moveIndex].pieceValueDifferential = -(get_simple_piece_value(_board[delta_row][delta_column]) - get_simple_piece_value(_board[row][column]));
		}
	}
	moveIndex++;
}

