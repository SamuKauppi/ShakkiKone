#include "gameState.h"

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

	// if move has key update repeated positions list
	current_position_zobrist = m._key;
	if (m._key != 0)
	{
		_repeated_positions->update_position(m._key);
	}
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
	// Update the position of the king
	int& row = piece == wK ? _wK_pos[0] : _bK_pos[0];
	int& column = piece == wK ? _wK_pos[1] : _bK_pos[1];
	row = end_row;
	column = end_column;

	// Move was a castle if king moves more than 1 square
	if (abs(end_column - start_column) > 1)
	{

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