#include "scoringLogic.h"
#include "math.h"

Evaluation::Evaluation()
{
    init_tables();
}

void Evaluation::init_tables()
{
    // Iterate through the every piece at every position
    int pc, p, sq;
    for (p = PAWN, pc = WHITE_PAWN; p <= KING; pc += 2, p++) 
    {
        for (sq = 0; sq < 64; sq++) 
        {
            // Get the mg/eg value and add values from pesto table
            mg_table[pc][sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[pc][sq] = eg_value[p] + eg_pesto_table[p][sq];

            // Flip the square and add black values
            int flipped_Sq = flip_square(sq);
            mg_table[pc + 1][sq] = mg_value[p] + mg_pesto_table[p][flipped_Sq];
            eg_table[pc + 1][sq] = eg_value[p] + eg_pesto_table[p][flipped_Sq];
        }
    }
}

int Evaluation::eval(const int board[8][8], int turnplayer) const
{
    // Initialize mg/eg values for white and black 
    int mg[2]{};
    int eg[2]{};
    mg[WHITE] = 0;
    mg[BLACK] = 0;
    eg[WHITE] = 0;
    eg[BLACK] = 0;
    int gamePhase = 0;

    // Iterate through 8x8 board
    for (int row = 0; row < 8; ++row) 
    {
        for (int column = 0; column < 8; column++)
        {
            // Sum the values if given square is not empty 
            int pc = board[row][column];
            if (pc != NA) 
            {
                int eval_pc = get_evaluated_piece(pc);
                int pcColor = get_piece_color(pc);
                int sq = row * 8 + column;
                mg[pcColor] += mg_table[eval_pc][sq];
                eg[pcColor] += eg_table[eval_pc][sq];
                gamePhase += gamephaseInc[eval_pc];
            }
        }
    }

    // Get evaluated values
    int mgScore = mg[WHITE] - mg[BLACK];
    int egScore = eg[WHITE] - eg[BLACK];

    // Get gamephase
    int mgPhase = gamePhase;
    if (mgPhase > gamePhaseTotal) mgPhase = gamePhaseTotal; // If promotion happens
    int egPhase = gamePhaseTotal - mgPhase;

    int eval = (mgScore * mgPhase + egScore * egPhase) / gamePhaseTotal;

    
    bool has_lead = turnplayer == WHITE ? eval > leadSensitivity : eval < -leadSensitivity;
    if (has_lead && gamePhase >= Gamephase)
    {
        eval -= eval / leadSensitivity;
    }
    
    // Return evaluation
    return eval;
}

int Evaluation::flip_square(int sq) const
{
    return sq ^ 56;
}

int Evaluation::get_evaluated_piece(int pc) const
{
    switch (pc)
    {
    case wR:
        return WHITE_ROOK;
    case wN:
        return WHITE_KNIGHT;
    case wB:
        return WHITE_BISHOP;
    case wQ:
        return WHITE_QUEEN;
    case wK:
        return WHITE_KING;
    case wP:
        return WHITE_PAWN;
    case bR:
        return BLACK_ROOK;
    case bN:
        return BLACK_KNIGHT;
    case bB:
        return BLACK_BISHOP;
    case bQ:
        return BLACK_QUEEN;
    case bK:
        return BLACK_KING;
    case bP:
        return BLACK_PAWN;
    default:
        return EMPTY;
    }
}

void Evaluation::update_gamephase(const int board[8][8])
{
    Gamephase = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int pc = board[i][j];
            if (pc != NA)
            {
                Gamephase += gamephaseInc[get_evaluated_piece(pc)];
            }
        }
    }
}
