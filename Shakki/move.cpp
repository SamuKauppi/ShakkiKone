#include <math.h>
#include "move.h"

/// <summary>
/// Converts a chess coordinate char to board index int
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
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

/// <summary>
/// Converts a board index int to chess coordinate char
/// </summary>
/// <param name="pos"></param>
/// <returns></returns>
static char int_pos_to_char(const int& pos)
{
	switch (pos)
	{
	case 7:
		return 'h';
	case 6:
		return 'g';
	case 5:
		return 'f';
	case 4:
		return 'e';
	case 3:
		return 'd';
	case 2:
		return 'c';
	case 1:
		return 'b';
	case 0:
		return 'a';
	default:
		return static_cast<char>(('8' - pos) + '0');
	}
}


// Maybe unessesary
//Move::Move(const string& m)
//{
//	for (int i = 0; i < 4; i++)
//	{
//		int pos_int = char_to_int_pos(m[i]);
//
//		// Start pos
//		if (i < 2)
//		{
//			_start_pos.insert(_start_pos.begin(), pos_int);
//		}
//		// End pos
//		else
//		{
//			_end_pos.insert(_end_pos.begin(), pos_int);
//		}
//	}
//}

string Move::get_move_name()
{
	if (_move_name != "")
	{
		return _move_name;
	}

	for (int i = 0; i < 4; i++)
	{
		int temp;
		if (i > 1)
		{
			temp = _start_pos[i - 2];
		}
		else
		{
			temp = _end_pos[i];
		}

		char m;
		if (i % 2 != 0)
		{
			m = int_pos_to_char(temp);
			_move_name.insert(0, 1, m);
		}
		else
		{
			temp = abs(temp - 8);
			_move_name.insert(0, to_string(temp));
		}
	}

	if (_piece_promotion != NA)
	{
		_move_name += piece_names[_piece_promotion];
	}

	return _move_name;
}
