#pragma once
#include "move.h"

class MinimaxValue
{
public:

	MinimaxValue(int value, Move move) : Value(value), Best_move(move)
	{}
	int Value;
	Move Best_move;
};
