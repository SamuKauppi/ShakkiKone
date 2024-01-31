#pragma once
using namespace std;

// Players
enum { WHITE, BLACK };


// Pieces (NA = empty)
enum { wR, wN, wB, wQ, wK, wP, bR, bN, bB, bQ, bK, bP, NA };


// Returns color of piece
int get_piece_color(int piece);

// Piece indecies mapped to array
const char piece_names[] = { 'R', 'N', 'B', 'Q', 'K', 'P', 'r', 'n', 'b', 'q', 'k', 'p', ' ' };

