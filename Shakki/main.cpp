#include "gameState.h"
#include "move.h"
#include <vector>
#include <iostream>

static int get_valid_move_index(vector<Move>& moves, string chosen)
{
	for (int i = 0; i < moves.size(); i++)
	{
		if (chosen == moves[i].get_move_name())
		{
			return i;
		}
	}
	return -1;
}

static void game_loop()
{
	GameState state;
	state.print_board();
	while (true)
	{
		vector<Move> moves;
		state.get_moves(moves);

		if (moves.size() <= 0)
		{
			break;
		}

		for (int i = 0; i < moves.size(); i++)
		{
			cout << moves[i].get_move_name();
			if (i < moves.size() - 1)
			{
				cout << ", ";
			}
		}

		cout << "\nMake a move: ";
		int move_index = -1;

		while (true)
		{
			string chosen = "";
			cin >> chosen;

			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}

			cout << "Not a valid move\nMake a move: ";
		}

		state.make_move(moves[move_index]);
		state.update_castle_legality();
		state.print_board();
	}
}

int main()
{
	game_loop();
	return 0;
}