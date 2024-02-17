#pragma once
#include "chess.h"
#include "move.h"
#include "minimaxValue.h"
#include "unordered_map"

class GameState
{
public:
	// turn player
	int TurnPlayer = WHITE;
	int _hash;

	// TODO: Implement hashing
	GameState()
	{
		generate_hash();
	}
	void generate_hash();

	/// <summary>
	/// Empties board
	/// </summary>
	void empty();

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
	/// Adds the new move to moves list using the index
	/// if index is out of bounds, push_back
	/// </summary>
	/// <param name="moveIndex"></param>
	/// <param name="moves"></param>
	/// <param name="new_move"></param>
	void add_move_with_index(int& moveIndex, vector<Move>& moves, Move& new_move) const;

	/// <summary>
	/// Returns all of the moves for turn player
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_raw_moves(int player, vector<Move>& moves) const;

	/// <summary>
	/// Get moves for this piece
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="piece"></param>
	/// <param name="moves"></param>
	void get_piece_moves(int& moveIndex, int row, int column, int player, int piece, vector<Move>& moves) const;

	/// <summary>
	/// Get moves based on piece
	/// </summary>
	/// <param name="row"></param>
	/// <param name="column"></param>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_rook_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const;
	void get_bishop_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const;
	void get_knight_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const;
	void get_king_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const;
	void get_pawn_moves(int& moveIndex, int row, int column, int player, vector<Move>& moves) const;


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
		vector<Move>& moves,
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
	void create_promotion_moves(
		int& moveIndex,
		int row,
		int column,
		int delta_row,
		int delta_column,
		int player,
		vector<Move>& moves) const;

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
	void get_moves(vector<Move>& moves) const;

	/// <summary>
	/// Sets all legal castle moves into vector
	/// </summary>
	/// <param name="player"></param>
	/// <param name="moves"></param>
	void get_castles(int player, vector<Move>& moves, int& moveIndex) const;

	/// <summary>
	/// Keeps up if players can castle
	/// </summary>
	void update_castle_legality();

	/// <summary>
	/// Disables long and/or short castle from player based on column
	/// </summary>
	/// <param name="short_castle"></param>
	/// <param name="long_castle"></param>
	/// <param name="column"></param>
	void disable_one_castle(bool& short_castle, bool& long_castle, int column);

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
	float score_board() const;

	/// <summary>
	/// Evaluates board state
	/// </summary>
	/// <returns></returns>
	float evaluate() const;


	/// <summary>
	/// Returns the difference between moves
	/// </summary>
	/// <returns></returns>
	float mobility_difference() const;
	/// <summary>
	/// Returns the difference between castles performed and not performed
	/// </summary>
	/// <returns></returns>
	float castle_difference() const;
	float evaluate_player_castle(bool done_castle, bool can_short, bool can_long) const;


	/// <summary>
	/// Gives the difference between white and black pieces
	/// 
	/// pawn 		1
	/// knight		3
	/// bishop		3.25
	/// rook 		5
	/// queen 		9
	/// </summary>	
	/// <returns></returns>
	float material_difference() const;
	float evaluate_piece_at_pos(int piece, float piece_value, int row, int column) const;

	float evaluate_pawn_at_pos(float piece_value, int row, int column) const;
	float evaluate_rook_at_pos(float piece_value, int row, int column) const;
	float evaluate_bishop_at_pos(float piece_value, int row, int column) const;
	float evaluate_knight_at_pos(float piece_value, int row, int column) const;
	float evaluate_queen_at_pos(float piece_value, int row, int column) const;
	float evaluate_king_at_pos(float piece_value, int row, int column) const;

	float check_difference() const;

	/// <summary>
	/// Performs minimax alorythmn
	/// </summary>
	/// <param name="depth"></param>
	/// <returns></returns>
	MinimaxValue minimax(int depth, float alpha, float beta) const;

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

		_wK_pos = other._wK_pos;
		_bK_pos = other._bK_pos;
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

	// Test board
	//int _board[8][8] = {
	//	{bR, NA, NA, NA, bK, NA, NA, bR},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{NA, NA, NA, NA, NA, NA, NA, NA},
	//	{wR, NA, NA, NA, wK, NA, NA, wR}
	//};

	vector<int> _wK_pos = { 7, 4 };
	vector<int> _bK_pos = { 0, 4 };

	bool _w_long_castle = true;
	bool _w_short_castle = true;
	bool _b_long_castle = true;
	bool _b_short_castle = true;

	bool _w_castle = false;
	bool _b_castle = false;

	int _doubleStep = -1;

	const unordered_map<int, float> piece_values = {
	{wP, 1.0f}, {wN, 3.0f}, {wB, 3.25}, {wR, 5.0f}, {wQ, 9.0f},
	{bP, -1.0f}, {bN, -3.0f}, {bB, -3.25f}, {bR, -5.0f}, {bQ, -9.0f},
	{wK, 1.0f}, {bK, -1.0f},
	{NA, 0.0f} };
};