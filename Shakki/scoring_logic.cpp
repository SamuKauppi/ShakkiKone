#include "scoring_logic.h"

Evaluation::Evaluation()
{
    init_tables();
}

void Evaluation::init_tables()
{
    int pc, p, sq;
    for (p = PAWN, pc = WHITE_PAWN; p <= KING; pc += 2, p++) {
        for (sq = 0; sq < 64; sq++) {
            mg_table[pc][sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[pc][sq] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[pc + 1][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
            eg_table[pc + 1][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
        }
    }
}

int Evaluation::eval(const int board[8][8], int side2Move) const
{
    int mg[2]{};
    int eg[2]{};
    int gamePhase = 0;

    mg[WHITE] = 0;
    mg[BLACK] = 0;
    eg[WHITE] = 0;
    eg[BLACK] = 0;

    /* evaluate each piece */
    for (int row = 0; row < 8; ++row) 
    {
        for (int column = 0; column < 8; column++)
        {
            int pc = board[row][column];
            if (pc != NA) {
                int sq = row * 8 + column;
                mg[PCOLOR(pc)] += mg_table[pc][sq];
                eg[PCOLOR(pc)] += eg_table[pc][sq];
                gamePhase += gamephaseInc[pc];
            }
        }
    }

    /* tapered eval */
    int mgScore = mg[side2Move] - mg[OTHER(side2Move)];
    int egScore = eg[side2Move] - eg[OTHER(side2Move)];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / 24;
}