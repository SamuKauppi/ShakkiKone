#pragma once
#include <string>
#include <vector>
#include "chess.h"
using namespace std;

// Defines move in GameState
class Move
{
public:
	Move(int s_y_pos, int s_x_pos, int e_y_pos, int e_x_pos)
	{
		_start_pos.push_back(s_y_pos);
		_start_pos.push_back(s_x_pos);
		_end_pos.push_back(e_y_pos);
		_end_pos.push_back(e_x_pos);
	}

	// Make a move from string
	Move(const string& m);

private:
	vector<int> _start_pos;
	vector<int> _end_pos;

	int _piece_promotion = NA;

	friend class GameState;
};
