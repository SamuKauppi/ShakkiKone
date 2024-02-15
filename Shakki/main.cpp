#include "gameState.h"
#include "move.h"
#include "limits"
#include "stack"
#include <vector>
#include <iostream>

static int get_valid_move_index(vector<Move>& moves, string& chosen)
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

static void player_input(GameState& current_state, string& chosen, bool& is_ai)
{

	// Player
	if (!is_ai)
	{
		cin >> chosen;
	}
	else
	{
		MinimaxValue ai_input =
			current_state.minimax(
				3,
				numeric_limits<float>::lowest(),
				numeric_limits<float>::max());

		chosen = ai_input.Best_move.get_move_name();
	}
}

static void game_loop(bool is_w_ai, bool is_b_ai)
{
	// Generate state and print it
	GameState current_state;
	current_state.print_board();

	// test minimax
	//MinimaxValue ai_value = current_state.minimax(2, numeric_limits<float>::lowest(), numeric_limits<float>::max());
	//cout << ai_value.Best_move.get_move_name() << " " << ai_value.Value << "\n";

	// Generate history
	stack<GameState> history;

	while (true)
	{
		// Generate a list of moves
		vector<Move> moves(50);
		current_state.get_moves(moves);

		// If no moves are left, the game is over
		if (moves.size() <= 0)
		{
			break;
		}

		// Print every move
		for (int i = 0; i < moves.size(); i++)
		{
			cout << moves[i].get_move_name();
			if (i < moves.size() - 1)
			{
				cout << ", ";
			}
		}

		// Generate player name string
		string player_name = current_state.TurnPlayer == WHITE ? "White player" : "Black player";

		// Show the prompt to give a move
		cout << "\n" << player_name << " a move: ";
		int move_index = -1;
		bool wasUndo = false;

		while (true)
		{
			// Generate the chosen move
			string chosen = "";

			bool is_ai = current_state.TurnPlayer == WHITE ? is_w_ai : is_b_ai;

			player_input(current_state, chosen, is_ai);

			// If chosen was undo, return to the previous state in history
			if (chosen == "undo" && history.size() > 0)
			{
				current_state = history.top();
				history.pop();
				wasUndo = true;
				break;
			}

			// Validate input
			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}
			cout << "Not a valid move\n" << player_name << " a move: ";
		}

		// if the input was not "undo", add this state to history and make the move
		if (!wasUndo)
		{
			history.push(current_state);
			current_state.make_move(moves[move_index]);
		}
		system("cls");
		cout << player_name << " made the move: " << moves[move_index].get_move_name() << "\n";
		current_state.print_board();

		if (wasUndo)
		{
			cout << "State reversed...\n";
		}
	}
}

static bool is_player_ai(string player_name)
{
	while (true)
	{
		string ans;
		cout << "Should " << player_name << " be controlled by AI?\n";
		cout << "(y/n):";
		cin >> ans;

		if (ans == "y")
		{
			return true;
		}
		else if (ans == "n")
		{
			return false;
		}

		cout << "\nWrong input\n";
	}

}

int main()
{
	bool w = is_player_ai("White");
	bool b = is_player_ai("Black");
	game_loop(w, b);
	return 0;
}