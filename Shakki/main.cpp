#include "gameState.h"
#include "move.h"
#include "limits"
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
	srand((unsigned)time(NULL));
	GameState state;

	MinimaxValue ai_value = state.minimax(2);
	cout << ai_value.Best_move.get_move_name() << "\n";
	system("pause");


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
			//cin >> chosen;
			chosen = moves[rand() % moves.size()].get_move_name();
			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}

			cout << "Not a valid move\nMake a move: ";
		}

		state.make_move(moves[move_index]);
		state.print_board();

		system("pause");
	}
}

int main()
{
	game_loop();
	return 0;
}