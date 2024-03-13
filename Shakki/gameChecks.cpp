#include "gameState.h"

bool GameState::is_under_threat(int row, int column, int opponent) const
{
	// Generate opponents raw moves
	vector<Move> opponentMoves(50);
	int moveIndex = 0;
	Move moves[200];
	get_raw_moves(opponent, moves, moveIndex);
	for (int i = 0; i < moveIndex; i++)
	{
		opponentMoves.push_back(moves[i]);
	}

	// check if any end position matches row and column
	for (int i = 0; i < opponentMoves.size(); i++)
	{
		if (opponentMoves[i]._end_pos[0] == row && opponentMoves[i]._end_pos[1] == column) {
			return true;
		}
	}
	return false;

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
				if (piece == bP && pawn_attack_check_direction == 1) return false;
				if (piece == wP && pawn_attack_check_direction == -1) return false;
				if (piece == bK || piece == wK) return false;
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
	int steps = 0;

	// loop all tiles in direction
	while (current_column <= 7 && current_column >= 0 && current_row <= 7 && current_row >= 0)
	{
		steps++;
		piece = _board[current_row][current_column];
		if (piece != NA)
		{
			// if piece is same colour king is safe as any attack would be blocked by this piece
			if (get_piece_color(piece) == player) return true;

			// if piece is queen or rook king is not safe
			if (piece == wQ || piece == bQ || piece == wR || piece == bR) return false;

			if (steps == 1)
			{
				if (piece == bK || piece == wK) return false;
			}
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

bool GameState::is_square_in_check(int player, int row, int column) const {

	// check all possible tiles where knight can attack king
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

bool GameState::is_draw_by_repetition(uint64_t key) const {
	return _repeated_positions->is_draw_by_repetition(key);
}

bool GameState::is_position_repeated(uint64_t key) const {
	return _repeated_positions->is_position_repeated(key);
}