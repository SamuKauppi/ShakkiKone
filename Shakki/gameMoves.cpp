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

bool GameState::is_diagonal_safe(int row_direction, int column_direction, int row, int column, int player) const {
	int current_row = row;
	int current_column = column;
	current_column += column_direction;
	current_row += row_direction;
	int piece;
	int steps = 0;
	int pawn_attack_check_direction = (player == WHITE) ? -1 : 1;

	// loop all tiles in direction
	while (current_column <= 7 && current_column >= 0 && current_row <= 7 && current_row >= 0)
	{
		steps++;
		piece = _board[current_row][current_column];
		if (piece != NA)
		{
			// if piece is same colour king is safe as any attack would be blocked by this piece
			if (get_piece_color(piece) == player) return true;

			// if only one step is taken towards opponents side diagonally and piece is pawn
			// we are in check
			if (steps == 1) {
				if (piece == bP && pawn_attack_check_direction == -1) return false;
				if (piece == wP && pawn_attack_check_direction == 1) return false;
			}
			// if piece is queen or bishop king is not safe
			if (piece == wQ || piece == bQ || piece == wB || piece == bB) return false;

			// if piece is anything else king is safe as they cannot attack diagonally
			return true;
		}
		current_column += column_direction;
		current_row += row_direction;
	}
	// if no pieces were hit during loop king is safe
	return true;
}


bool GameState::is_row_or_column_safe(int row_direction, int column_direction, int row, int column, int player) const {
	int current_column = column;
	int current_row = row;
	current_column += column_direction;
	current_row += row_direction;
	int piece;

	// loop all tiles in direction
	while (current_column <= 7 && current_column >= 0 && current_row <= 7 && current_row >= 0)
	{
		piece = _board[current_row][current_column];
		if (piece != NA)
		{
			// if piece is same colour king is safe as any attack would be blocked by this piece
			if (get_piece_color(piece) == player) return true;

			// if piece is queen or rook king is not safe
			if (piece == wQ || piece == bQ || piece == wR || piece == bR) return false;

			// if piece is anything else king is safe
			return true;
		}
		current_column += column_direction;
		current_row += row_direction;
	}
	// if no pieces were hit during loop king is safe
	return true;
}

bool GameState::is_safe_from_knight(int row, int column, int player) const {
	// check for horse moves.
	int enemy_horse = (player == WHITE) ? bN : wN;
	int current_row;
	int current_column;
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			current_row = row + j;
			current_column = column + i * 2;
			if (current_row < 0 || current_row > 7 || current_column < 0 || current_column > 7) continue;
			if (_board[current_row][current_column] == enemy_horse) return false;
		}
		for (int j = -1; j < 2; j += 2)
		{
			current_row = row + (i * 2);
			current_column = column + j;
			if (current_row < 0 || current_row > 7 || current_column < 0 || current_column > 7) continue;
			if (_board[current_row][current_column] == enemy_horse) return false;
		}
	}
	return true;
}

bool GameState::is_king_in_check(int player) const {
	// Get king position
	int row, column;
	if (player == WHITE)
	{
		row = _wK_pos[0];
		column = _wK_pos[1];
	}
	else
	{
		row = _bK_pos[0];
		column = _bK_pos[1];
	}
	// checkk all possible tiles where knight can attack king
	if (!is_safe_from_knight(row, column, player)) return true;

	// check horizontal and vertical moves for attacks from rooks and quuens
	if (!is_row_or_column_safe(1, 0, row, column, player)) return true;
	if (!is_row_or_column_safe(-1, 0, row, column, player)) return true;
	if (!is_row_or_column_safe(0, 1, row, column, player)) return true;
	if (!is_row_or_column_safe(0, -1, row, column, player)) return true;

	// check diagonal moves for attack from bishops, quuens and pawns
	if (!is_diagonal_safe(1, 1, row, column, player)) return true;
	if (!is_diagonal_safe(-1, 1, row, column, player)) return true;
	if (!is_diagonal_safe(1, -1, row, column, player)) return true;
	if (!is_diagonal_safe(-1, -1, row, column, player)) return true;

	return false;
}

// checks from kings positions all possible locations where king could be attacked
// when piece is moved.
bool GameState::is_king_safe(Move& m, int player) const {
	int row, column;
	bool in_board;
	bool is_king = false;
	int opponent;
	if (player == WHITE)
	{
		row = _wK_pos[0];
		column = _wK_pos[1];
		opponent = BLACK;
	}
	else
	{
		row = _bK_pos[0];
		column = _bK_pos[1];
		opponent = WHITE;
	}

	if (!is_safe_from_knight(row, column, player)) return false;
	
	// check if moved piece is king.
	// if not king we can avoid a lot of checks
	if (m._end_pos[0] == row && m._end_pos[1] == column) is_king = true;

	if (!is_king) 
	{
		// if not king we can reduce checks by calculating direction that needs to be checked as we
		// can assume position prior to the move was legal and only the moved piece could open king 
		// up to danger.
		int column_dir = 0;
		int row_dir = 0;

		// if moved piece was on same row or column check for rooks/queens on that file
		if (m._start_pos[0] == row || m._start_pos[1] == column)
		{
			// column direction that needs to be checked, 0 if moved piece was not on the same row
			if (m._start_pos[0] == row) column_dir = (m._start_pos[1] < column) ? -1 : 1;
			// row direction, 0 if moved piece was not on the same column
			if (m._start_pos[1] == column) row_dir = (m._start_pos[0] < row) ? -1 : 1;
			
			return is_row_or_column_safe(row_dir, column_dir, row, column, player);
		}

		// row and column difference of moved pieces original position and king
		int row_difference = m._start_pos[0] - row;
		int column_difference = m._start_pos[1] - column;

		// bitwise operation to get absolute value of a number
		uint32_t temp = row_difference >> 31;
		row_difference ^= temp;
		row_difference += temp & 1;

		temp = column_difference >> 31;
		column_difference ^= temp;
		column_difference += temp & 1;

		// if column and row difference are same piece was originally diagonally aligned with king
		if (row_difference == column_difference)
		{
			// column direction that needs to be checked
			column_dir = (m._start_pos[1] < column) ? -1 : 1;
			// row direction,
			row_dir = (m._start_pos[0] < row) ? -1 : 1;
			return is_diagonal_safe(row_dir, column_dir, row, column, player);
		}

		// if moved piece was not vertically, horizontally or diagonally aligned with king we know king is safe
		// this is most likely outcome which allows us to skip most of the looping through the board
		return true;
	}

	// for king moves we do expensive function call
	return !is_king_in_check(player);
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
