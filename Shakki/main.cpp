#include "gameState.h"
#include "move.h"
#include "transpositionTable.h"
#include "limits"
#include "stack"
#include <vector>
#include <iostream>
#include "chrono"

static const int MINMAX_DEPTH = 6;

/// <summary>
/// Undo a move if there are moves in history
/// </summary>
/// <param name="current_state"></param>
/// <param name="history"></param>
/// <returns></returns>
static bool try_undo_move(GameState& current_state, stack<GameState>& history)
{
	if (history.size() <= 0)
		return false;

	current_state = history.top();
	history.pop();
	return true;
}

/// <summary>
/// Handle undo logic
/// </summary>
/// <param name="current_state"></param>
/// <param name="history"></param>
static void undo_loop(GameState& current_state, stack<GameState>& history)
{
	string input = "";
	while (input != "start")
	{
		system("cls");
		current_state.print_board();
		cout << "\n'start' = continue game\n";
		cout << "'undo' = move to the previous state\n";
		cout << "Input: ";
		cin >> input;

		if (input == "undo")
		{
			try_undo_move(current_state, history);
		}

	}
	system("cls");
	cout << "State reversed...\n";
	current_state.print_board();
}

/// <summary>
/// Compare given string to move names and return an index corresponding to it
/// Otherwise return -1
/// </summary>
/// <param name="moves"></param>
/// <param name="chosen"></param>
/// <returns></returns>
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

/// <summary>
/// Handles human and ai player input
/// </summary>
/// <param name="current_state"></param>
/// <param name="chosen"></param>
/// <param name="is_ai"></param>
static string player_input(GameState& current_state, bool& is_ai, TranspositionTable& tt)
{
	string chosen;
	// Player
	if (!is_ai)
	{
		cin >> chosen;
	}
	// AI
	else
	{
		cout << "Calculating...";
		MinimaxValue ai_input =
			current_state.minimax(
				MINMAX_DEPTH,
				numeric_limits<int>::lowest(),
				numeric_limits<int>::max(), tt);

		chosen = ai_input.Best_move.get_move_name();
	}

	return chosen;
}

/// <summary>
/// Handles main game loop
/// </summary>
/// <param name="is_w_ai"></param>
/// <param name="is_b_ai"></param>
/// <returns></returns>
static string game_loop(bool is_w_ai, bool is_b_ai)
{
	TranspositionTable tt;
	// Generate state and print it
	GameState current_state;
	current_state.print_board();

	// Generate history
	stack<GameState> history;

	// Undo stop flag (used when user types undo)
	bool wasUndo = false;

	// Main game loop
	while (true)
	{
		auto s = chrono::high_resolution_clock::now();
		// Prevent the game from continuing until user says if undo was last input
		if (wasUndo)
		{
			undo_loop(current_state, history);
			wasUndo = false;
			continue;
		}

		// Generate a list of moves
		vector<Move> moves(40);
		current_state.get_moves(moves);

		// If no moves are left, the game is over
		if (moves.size() <= 0)
		{
			break;
		}

		// Print every move
		cout << "list of every move: \n";
		for (int i = 0; i < moves.size(); i++)
		{
			cout << moves[i].get_move_name();
			if (i < moves.size() - 1)
			{
				cout << ", ";
			}
		}
		cout << "\n";

		// Generate player name string
		string player_name = current_state.TurnPlayer == WHITE ? "White player" : "Black player";

		// Show the prompt to give a move
		cout << "\n" << player_name << ", choose a move: ";
		int move_index = -1;

		while (true)
		{
			// Check if the turn player is ai
			bool is_ai = current_state.TurnPlayer == WHITE ? is_w_ai : is_b_ai;

			// Get the chosen move name
			string chosen = player_input(current_state, is_ai, tt);
			
			// if user types undo, skip and go to undo logic
			if (chosen == "undo")
			{
				wasUndo = true;
				break;
			}

			// Validate input
			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}
			cout << chosen <<"is not a valid move\n" << player_name << " a move: ";
		}

		// Clear the screen
		auto stop = chrono::high_resolution_clock::now();
		system("cls");
		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - s);
		cout << "Time spent: " << duration.count() << "ms\n";
		cout << "positions calculated: " << tt._positionCount << "\n" << "repeat positions: " << tt._positionRepeats << "\n";
		tt._positionCount = 0;
		tt._positionRepeats = 0;

		// If the input was not "undo", add this state to history and make the chosen move
		// Also print what move was made
		if (!wasUndo)
		{
			history.push(current_state);
			current_state.make_move(moves[move_index]);
			cout << player_name << " made the move: " << moves[move_index].get_move_name() << "\n";
		}

		// Print the new board
		current_state.print_board();
	}

	// Check if it is a draw
	// Get losing king
	int losing_king = current_state.TurnPlayer == WHITE ? wK : bK;

	// Get king coordinates
	int row, column;
	current_state.find_piece(losing_king, row, column);

	// If the king is not under threat, it's a draw
	if (!current_state.is_under_threat(row, column, 1 - current_state.TurnPlayer))
	{
		return "it's a draw!";
	}

	// Otherwise there is a winner
	return current_state.TurnPlayer == WHITE ? "Black" : "White";
}

/// <summary>
/// Ask user player is ai and/or human
/// </summary>
/// <param name="player_name"></param>
/// <returns></returns>
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
	string winner = game_loop(w, b);
	cout << "\nAnd the winner is: " << winner;
	return 0;
}