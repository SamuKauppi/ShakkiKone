#include "gameState.h"
#include <iostream>

void GameState::make_move(const Move& m)
{
	// Create variables
	int start_row = m._start_pos[0];	//	Starting row
	int start_column = m._start_pos[1];	//	Starting column
	int end_row = m._end_pos[0];		//	Ending row
	int end_column = m._end_pos[1];		//	Ending column
	int piece;							//	Piece index

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

	// Reset doubleStep variable after every move
	_doubleStep = -1;

	// Handle special pawn moves
	if (piece == wP || piece == bP)
		special_pawn_moves(start_row, start_column, end_row, end_column);


	// Handle special king moves
	if (piece == wK || piece == bK)
		special_king_moves(piece, start_row, start_column, end_row, end_column);

	// Reset piece at start pos
	_board[start_row][start_column] = NA;

	// Update piece at end pos
	_board[end_row][end_column] = piece;

	// Handle castle legality
	update_castle_legality();

	// switch players
	TurnPlayer = 1 - TurnPlayer;
}

void GameState::special_pawn_moves(int start_row, int start_column, int end_row, int end_column)
{
	// If pawn has moved more than 1 square, a double step happened. Save column
	if (abs(end_row - start_row) > 1)
	{
		_doubleStep = end_column;
	}

	// if moved diagonally to a non occupied square en passant happened
	if (start_column != end_column && _board[end_row][end_column] == NA)
	{
		_board[start_row][end_column] = NA;
	}

}

void GameState::special_king_moves(int piece, int start_row, int start_column, int end_row, int end_column)
{
	bool is_white = piece == wK;

	// Update the position of the king
	int& row = is_white ? _wK_pos[0] : _bK_pos[0];
	int& column = is_white ? _wK_pos[1] : _bK_pos[1];
	row = end_row;
	column = end_column;

	// Move was a castle if king moves more than 1 square
	if (abs(end_column - start_column) > 1)
	{
		// Set the appropriate castle variable
		if (is_white) 
			_w_castle = true;
		else 
			_b_castle = true;

		// Get the direction for the rook
		int rook_dir = end_column - start_column;
		// Get column where rook will go
		int rook_column = (rook_dir > 0) ? 5 : 3;
		// Get the corner where the rook starts
		int corner = (rook_dir > 0) ? 7 : 0;

		// Get the rook index
		int rook = _board[start_row][corner];

		// Empty the corner spot
		_board[start_row][corner] = NA;

		// Add rook to the end position
		_board[start_row][rook_column] = rook;
	}
}

void GameState::get_raw_moves(int player, vector<Move>& moves) const
{
	int moveIndex = 0;
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
				get_piece_moves(moveIndex, i, j, player, piece, moves);
			}
		}
	}

	moves.erase(moves.begin() + moveIndex, moves.end());
}

void GameState::get_castles(int player, vector<Move>& moves, int& moveIndex) const
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
		Move m = Move(player_pos, 4, player_pos, 6, player);
		add_move_with_index(moveIndex, moves, m);
	}

	if (can_long &&
		!is_under_threat(player_pos, 4, opponent) &&
		!is_under_threat(player_pos, 3, opponent) &&
		!is_under_threat(player_pos, 2, opponent))
	{
		Move m = Move(player_pos, 4, player_pos, 2, player);
		add_move_with_index(moveIndex, moves, m);
	}
}

void GameState::add_move_with_index(int& moveIndex, vector<Move>& moves, Move& new_move) const
{
	if (moveIndex < moves.size())
	{
		moves[moveIndex] = new_move;
	}
	else
	{
		moves.push_back(new_move);
	}
	moveIndex++;
}

void GameState::get_moves(vector<Move>& moves) const
{
	// Get the Players
	int player = TurnPlayer;
	int opponent = 1 - TurnPlayer;

	// Generate raw moves
	vector<Move> rawMoves(50);
	get_raw_moves(player, rawMoves);

	// Generate move index (used to fill empty spots in vector<Move>& moves)
	int moveIndex = 0;

	// Add castle moves into moves (validates if it's legal on it's own)
	get_castles(player, moves, moveIndex);

	// Iterate through moves
	for (Move& rm : rawMoves)
	{
		// Generate test state and test a move
		GameState testState = *this;
		testState.make_move(rm);

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
		if (!testState.is_under_threat(row, column, opponent))
		{
			add_move_with_index(moveIndex, moves, rm);
		}
	}

	// Remove possible empty slots
	moves.erase(moves.begin() + moveIndex, moves.end());
}

bool GameState::is_legal(Move& m) const {
	GameState testState = *this;
	testState.make_move(m);

	// Get king position
	int row, column;
	if (TurnPlayer == WHITE)
	{
		row = testState._wK_pos[0];
		column = testState._wK_pos[1];
	}
	else
	{
		row = testState._bK_pos[0];
		column = testState._bK_pos[1];
	}
	return !testState.is_under_threat(row, column, 1 - TurnPlayer);
}

void GameState::create_promotion_moves(
	int& moveIndex,
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
		add_move_with_index(moveIndex, moves, m);
	}
}

void GameState::get_piece_moves(int& moveIndex, int row, int column, int player, int piece, vector<Move>& moves) const
{
	switch (piece)
	{
	case wR:
	case bR:
		get_rook_moves(moveIndex, row, column, player, moves);
		break;

	case wB:
	case bB:
		get_bishop_moves(moveIndex, row, column, player, moves);
		break;

	case wQ:
	case bQ:
		get_rook_moves(moveIndex, row, column, player, moves);
		get_bishop_moves(moveIndex, row, column, player, moves);
		break;

	case wN:
	case bN:
		get_knight_moves(moveIndex, row, column, player, moves);
		break;

	case wK:
	case bK:
		get_king_moves(moveIndex, row, column, player, moves);
		break;

	case wP:
	case bP:
		get_pawn_moves(moveIndex, row, column, player, moves);
		break;

	default:
		break;
	}
}

void GameState::get_pawn_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const
{
	// Direction based on player
	int direction = player == WHITE ? -1 : 1;

	// Eating
	get_raw_moves_in_dir(moveIndex, row, column, direction, 1, player, 1, moves, true, true);
	get_raw_moves_in_dir(moveIndex, row, column, direction, -1, player, 1, moves, true, true);

	// If in starting pos, 2 directly forward
	if (row == 6 || row == 1)
	{
		get_raw_moves_in_dir(moveIndex, row, column, direction, 0, player, 2, moves, false);
	}
	// 1 directly forward
	else
	{
		get_raw_moves_in_dir(moveIndex, row, column, direction, 0, player, 1, moves, false);
	}

	// check for en passant
	int rowToCheck = player == WHITE ? 3 : 4;
	if (_doubleStep != -1 && rowToCheck == row)
	{

		int enemyPawn = player == WHITE ? bP : wP;
		// check if double step happened within 1 square
		if (abs(_doubleStep - column) < 2)
		{
			get_raw_moves_in_dir(moveIndex, row, column, direction, _doubleStep - column, player, 1, moves);
		}

	}

}

void GameState::get_rook_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(moveIndex, row, column, -1, 0, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 0, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, -1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, 1, player, 7, moves);
}

void GameState::get_bishop_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(moveIndex, row, column, -1, 1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, -1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, -1, player, 7, moves);
}

void GameState::get_knight_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(moveIndex, row, column, 2, 1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 2, -1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -2, 1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -2, -1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 2, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, -2, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, 2, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, -2, player, 1, moves);
}

void GameState::get_king_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const
{
	get_raw_moves_in_dir(moveIndex, row, column, 1, 0, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, 0, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, 1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, -1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, -1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, -1, player, 1, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, 1, player, 1, moves);
}

void GameState::get_raw_moves_in_dir(int& moveIndex, int row, int column, int delta_row, int delta_column,
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
					create_promotion_moves(moveIndex, row, column, current_row, current_column, player, moves);
				else
				{
					Move m = Move(row, column, current_row, current_column, player);
					add_move_with_index(moveIndex, moves, m);
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
				create_promotion_moves(moveIndex, row, column, current_row, current_column, player, moves);
			else
			{
				Move m = Move(row, column, current_row, current_column, player);
				add_move_with_index(moveIndex, moves, m);
			}
		}

		break;
	}
}
