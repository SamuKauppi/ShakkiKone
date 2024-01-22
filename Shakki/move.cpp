#include <math.h>
#include "move.h"

static int char_to_int_pos(const char& c)
{
	switch (c)
	{
	case 'h':
		return 7;
	case 'g':
		return 6;
	case 'f':
		return 5;
	case 'e':
		return 4;
	case 'd':
		return 3;
	case 'c':
		return 2;
	case 'b':
		return 1;
	case 'a':
		return 0;
	default:
		int reverse_int = abs((c - '0') - 8);
		return reverse_int;
	}
}

Move::Move(const string& m)
{
	for (int i = 0; i < m.length(); i++)
	{
		int pos_int = char_to_int_pos(m[i]);

		// Start pos
		if (i < 2)
		{
			_start_pos.insert(_start_pos.begin(), pos_int);
		}
		// End pos
		else
		{
			_end_pos.insert(_end_pos.begin(), pos_int);
		}
	}
}
