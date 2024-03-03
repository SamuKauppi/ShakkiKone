#include "gameState.h"

void GameState::get_piece_moves(int& moveIndex, int row, int column, int player, int piece, Move moves[]) const
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

void GameState::get_pawn_moves(int& moveIndex, int row, int column, int player, Move moves[]) const
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

void GameState::get_rook_moves(int& moveIndex, int row, int column, int player, Move moves[]) const
{
	get_raw_moves_in_dir(moveIndex, row, column, -1, 0, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 0, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, -1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 0, 1, player, 7, moves);
}

void GameState::get_bishop_moves(int& moveIndex, int row, int column, int player, Move moves[]) const
{
	get_raw_moves_in_dir(moveIndex, row, column, -1, 1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, 1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, 1, -1, player, 7, moves);
	get_raw_moves_in_dir(moveIndex, row, column, -1, -1, player, 7, moves);
}

void GameState::get_knight_moves(int& moveIndex, int row, int column, int player, Move moves[]) const
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

void GameState::get_king_moves(int& moveIndex, int row, int column, int player, Move moves[]) const
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
