#include "gameState.h"
#include "move.h"
#include "limits"
#include "stack"
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
	//srand((unsigned)time(NULL));
	GameState current_state;

	MinimaxValue ai_value = current_state.minimax(2, numeric_limits<float>::lowest(), numeric_limits<float>::max());
	cout << ai_value.Best_move.get_move_name() << " " << ai_value.Value << "\n";
	system("pause");

	stack<GameState> history;
	history.push(current_state);

	current_state.print_board();
	while (true)
	{
		vector<Move> moves;
		current_state.get_moves(moves);

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
		bool wasUndo = false;

		while (true)
		{
			string chosen = "";
			cin >> chosen;

			if (chosen == "undo" && history.size() > 1)
			{
				history.pop();
				current_state = history.top();
				wasUndo = true;
				break;
			}

			//chosen = moves[rand() % moves.size()].get_move_name();
			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}

			cout << "Not a valid move\nMake a move: ";
		}

		if (!wasUndo)
		{
			current_state.make_move(moves[move_index]);
			history.push(current_state);
		}

		current_state.print_board();


		//system("pause");
	}
}

int main()
{
	game_loop();
	return 0;
}