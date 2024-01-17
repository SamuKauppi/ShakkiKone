#include "move.h"
static int char_to_int_pos(const char& c)
{
	switch (c)
	{
	case 'h':
		return 0;
	case 'g':
		return 1;
	case 'f':
		return 2;
	case 'e':
		return 3;
	case 'd':
		return 4;
	case 'c':
		return 5;
	case 'b':
		return 6;
	case 'a':
		return 7;
	default:
		return (c - '0') - 1;
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
