#include "chess.h"


int get_piece_color(int piece)
{
	switch (piece)
	{
	case wR:
	case wN:
	case wB:
	case wQ:
	case wK:
	case wP:
		return WHITE;
	default:
		return BLACK;
	}
}