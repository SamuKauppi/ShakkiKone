#pragma once
#include "move.h"

class MinimaxValue
{
public:

	MinimaxValue(float value, Move move) : Value(value), Best_move(move)
	{}
	float Value;
	Move Best_move;
};
