#pragma once
#include "chess.h"
#include "move.h"
#include "minimaxValue.h"
#include "unordered_map"
#include "transpositionTable.h"
#include <chrono>
#include "repeatedPositions.h"
#include <iostream>
using namespace std;
class TranspositionTable;

class GameState
{
public:
	// turn player
	int TurnPlayer = WHITE;
	int DepthReached = 0;
	// time limit allocated for position calculations in milliseconds
	int TimeLimit = 5000;

	int TimeExtension = 0;
	int ExtensionDuration = 0;

	// current positions zobrist value updated after every move in make_move
	uint64_t current_position_zobrist = 0;

	/// <summary>
	/// Moves a piece
	/// </summary>
	/// <param name="m"></param>
	void make_move(const Move& m);

	/// <summary>
	/// Handles logic relating to pawn
	/// </summary>
	/// <param name="start_row"></param>
	/// <param name="start_column"></param>
	/// <param name="end_row"></param>
	/// <param name="end_column"></param>
	void special_pawn_moves(int start_row, int start_column, int end_row, int end_column);

	/// <summary>
	/// Handles logic related to king
	/// </summary>
	/// <param name="piece"></param>
	/// <param name="start_row"></param>
	/// <param name="start_column"></param>
	/// <param name="end_row"></param>
	/// <param name="end_column"></param>
	void special_king_moves(int piece, int start_row, int start_column, int end_row, int end_column);

	/// <summary>
	/// Print board
	/// </summary>
	void print_board() const;

	/// <summary>
	/// Inserts a move to array
	/// </summary>
	/// <param name="moveIndex"></param>
	/// <param name="moves"></param>
	/// <param name="new_move"></param>
	void add_move_with_index(int& moveIndex, int row, int column, int delta_row, int delta_column, int player, Move moves[], bool capture = false, int promo = NA) const;

	/// <summary>
	/// Returns all of the moves for turnplayer
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	/// <param name="moveIndex"></param>
	void get_raw_moves(int player, Move moves[], int& moveIndex) const;

	/// <summary>
	/// Get moves for a piece at this position
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="piece"></param>
	/// <param name="moves"></param>
	void get_piece_moves(int& moveIndex, int row, int column, int player, int piece, Move moves[]) const;
	void get_rook_moves(int& moveIndex, int row, int column, int player, Move moves[]) const;
	void get_bishop_moves(int& moveIndex, int row, int column, int player, Move moves[]) const;
	void get_knight_moves(int& moveIndex, int row, int column, int player, Move moves[]) const;
	void get_king_moves(int& moveIndex, int row, int column, int player, Move moves[]) const;
	void get_pawn_moves(int& moveIndex, int row, int column, int player, Move moves[]) const;


	/// <summary>
	/// Return all possible movements in direction
	/// </summary>
	/// <param name="row">			Starting row				</param>
	/// <param name="column">		Starting column				</param>
	/// <param name="delta_row">	Row change					</param>
	/// <param name="delta_column">	Column change				</param>
	/// <param name="player">		Player enum					</param>
	/// <param name="max_steps">	Max move distance			</param>
	/// <param name="moves">		Filled with moves			</param>
	/// <param name="can_eat">		can eat (true default)		</param>
	/// <param name="has_to_eat">	has to eat (false default	</param>
	void get_raw_moves_in_dir(int& moveIndex,
		int row,
		int column,
		int delta_row,
		int delta_column,
		int player,
		int max_steps,
		Move moves[],
		bool can_eat = true,
		bool has_to_eat = false) const;


	/// <summary>
	/// Creates promotion moves from position
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="delta_row"></param>
	/// <param name="delta_column"></param>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_promotion_moves(
		int& moveIndex,
		int row,
		int column,
		int delta_row,
		int delta_column,
		int player,
		Move moves[],
		bool capture = false) const;

	/// <summary>
	/// Finds the position of given piece on board
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	void find_piece(int piece, int& row, int& column) const;

	/// <summary>
	/// Checks if given position is under threat
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="opponent"></param>
	/// <returns></returns>
	bool is_under_threat(int row, int column, int opponent) const;

	/// <summary>
	/// Sets all legal moves into moves vector
	/// </summary>
	/// <param name="moves"></param>
	void get_moves(vector<Move>& moves, TranspositionTable& tt) const;

	/// <summary>
	/// Checks if square is in check
	/// </summary>
	/// <param name="player"></param>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <returns></returns>
	bool is_square_in_check(int player, int row, int column) const;
	bool is_diagonal_safe(int row_dir, int column_dir, int row, int column, int player) const;
	bool is_safe_from_knight(int row, int column, int player) const;
	bool is_row_or_column_safe(int row_dir, int column_dir, int row, int column, int player) const;

	/// <summary>
	/// Sets all legal castle moves into vector
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_castles(int player, Move moves[], int& moveIndex) const;

	/// <summary>
	/// Keeps up if players can castle
	/// </summary>
	void update_castle_legality();

	/// <summary>
	/// Returns false if neither player can perform any castle
	/// </summary>
	/// <param name="player"></param>
	/// <returns></returns>
	bool can_player_castle(int player) const;

	/// <summary>
	/// Evaluates the board as follow:
	/// 
	/// White made check mate:		1000000
	/// Draw						0
	/// Black made check mate:		-1000000
	/// 
	/// Called when no legal moves are left
	/// </summary>
	/// <returns></returns>
	int score_board(int startingDepth, int depth) const;

	/// <summary>
	/// Evaluates board state
	/// </summary>
	/// <returns></returns>
	int evaluate() const;

	/// <summary>
	/// iteratively deepens search
	/// </summary>
	//MinimaxValue iterative_deepening(TranspositionTable& tt) const;

	MinimaxValue iterative_deepening(int alpha, int beta, TranspositionTable& tt) const;
	/// <summary>
	/// Performs minimax algorythmn
	/// </summary>
	/// <param name="depth"></param>
	/// <returns></returns>
	MinimaxValue minimax(int depth, int startingDepth ,int alpha, int beta, TranspositionTable& tt, chrono::steady_clock::time_point timer_start) const;

	int quiescence(int alpha, int beta) const;

	int get_simple_piece_value(int piece) const;

	bool is_draw_by_repetition(uint64_t key) const;

	bool is_position_repeated(uint64_t key) const;

	/// <summary>
	/// Overwrites this state with chosen states values
	/// </summary>
	/// <param name="other"></param>
	void operator=(GameState& other)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				_board[i][j] = other._board[i][j];
			}
		}

		_w_long_castle = other._w_long_castle;
		_w_short_castle = other._w_short_castle;
		_b_long_castle = other._b_long_castle;
		_b_short_castle = other._b_short_castle;

		_doubleStep = other._doubleStep;

		TurnPlayer = other.TurnPlayer;

		_wK_pos[0] = other._wK_pos[0];
		_wK_pos[1] = other._wK_pos[1];
		_bK_pos[0] = other._bK_pos[0];
		_bK_pos[1] = other._bK_pos[1];
	}

	~GameState()
	{
		if (current_position_zobrist != 0) {
			_repeated_positions->remove_repeat(current_position_zobrist);
		}
	}
	GameState()
	{
		_repeated_positions = new RepeatedPositions();
	}
private:

	// Game board
	// [y][x]

	int _board[8][8] = {
		{bR, bN, bB, bQ, bK, bB, bN, bR},
		{bP, bP, bP, bP, bP, bP, bP, bP},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, wP, wP, wP, wP, wP, wP, wP},
		{wR, wN, wB, wQ, wK, wB, wN, wR}
	};
	/*int _board[8][8] = {
	{bR, NA, NA, NA, bK, NA, NA, bR},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{NA, NA, NA, NA, NA, NA, NA, NA},
	{wR, NA, NA, NA, wK, NA, NA, wR}
	};*/

	//test board
	
	/*int _board[8][8] = {
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, NA, bK, NA, NA, NA, NA, NA},
		{NA, wR, NA, NA, wP, NA, NA, NA},
		{NA, NA, NA, wK, NA, NA, NA, NA},
		{NA, NA, NA, NA, wP, NA, wP, NA},
		{NA, NA, NA, NA, wK, wB, NA, NA}
	};*/
	

	//int _board[8][8] = {
	//{NA, NA, NA, NA, bR, NA, bK, NA},
	//{NA, NA, NA, NA, NA, bP, NA, bP},
	//{bP, NA, NA, NA, NA, NA, bP, NA},
	//{wP, bP, bP, NA, bP, NA, wP, wP},
	//{NA, NA, NA, NA, wB, NA, bB, NA},
	//{NA, NA, wP, bR, NA, NA, NA, NA},
	//{NA, wP, NA, NA, bP, NA, NA, NA},
	//{NA, NA, wR, NA, wQ, wK, NA, NA},
	//};


	int _wK_pos[2] = {7, 4};
	int _bK_pos[2] = {0, 4};

	bool _w_long_castle = true;
	bool _w_short_castle = true;
	bool _b_long_castle = true;
	bool _b_short_castle = true;

	/*bool _w_long_castle = false;
	bool _w_short_castle = false;
	bool _b_long_castle = false;
	bool _b_short_castle = false;*/

	int _doubleStep = -1;

	RepeatedPositions* _repeated_positions;

	friend class TranspositionTable;
};