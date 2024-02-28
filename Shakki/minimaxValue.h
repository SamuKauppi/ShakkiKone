#pragma once
#include "move.h"

class MinimaxValue
{
public:

	MinimaxValue(int value, Move move, int depth) : Value(value), Best_move(move), Depth(depth)
	{}
	int Depth;
	int Value;
	Move Best_move;
};
