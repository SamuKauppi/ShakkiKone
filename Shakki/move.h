#pragma once
#include <string>
#include <vector>
#include "chess.h"
using namespace std;

// Defines move in GameState
class Move
{
public:

	/// <summary>
	/// Default constructor
	/// </summary>
	Move()
	{}

	/// <summary>
	/// Contruct a move from int
	/// </summary>
	/// <param name="s_y_pos"></param>
	/// <param name="s_x_pos"></param>
	/// <param name="e_y_pos"></param>
	/// <param name="e_x_pos"></param>
	Move(int s_y_pos, int s_x_pos, int e_y_pos, int e_x_pos)
	{
		_start_pos[0] = s_y_pos;
		_start_pos[1] = s_x_pos;

		_end_pos[0] = e_y_pos;
		_end_pos[1] = e_x_pos;
	}

	Move(int s_y_pos, int s_x_pos, int e_y_pos, int e_x_pos, int player)
	{
		_start_pos[0] = s_y_pos;
		_start_pos[1] = s_x_pos;

		_end_pos[0] = e_y_pos;
		_end_pos[1] = e_x_pos;

		_evaluation = player == WHITE ?
			numeric_limits<int>::lowest() : numeric_limits<int>::max();
	}

	string get_move_name() const;

	bool operator<(Move& other)
	{
		return _evaluation < other._evaluation;
	}
	bool operator>(Move& other)
	{
		return _evaluation > other._evaluation;
	}

private:
	int _start_pos[2] = {0, 0};
	int _end_pos[2] = {0, 0};

	int _evaluation;

	int _piece_promotion = NA;

	friend class GameState;
};
