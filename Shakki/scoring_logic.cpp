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
            mg_table[pc + 1][sq] = mg_value[p] + mg_pesto_table[p][flip_square(sq)];
            eg_table[pc + 1][sq] = eg_value[p] + eg_pesto_table[p][flip_square(sq)];
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
            int pc = get_evaluated_piece(board[row][column]);
            if (pc != EMPTY) 
            {
                int sq = row * 8 + column;
                mg[get_pcolor(pc)] += mg_table[pc][sq];
                eg[get_pcolor(pc)] += eg_table[pc][sq];
                gamePhase += gamephaseInc[pc];
            }
        }
    }

    /* tapered eval */
    int mgScore = mg[side2Move] - mg[1 - side2Move];
    int egScore = eg[side2Move] - eg[1 - side2Move];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / 24;
}

int Evaluation::flip_square(int sq) const
{
    return sq ^ 56;
}

int Evaluation::get_pcolor(int pc) const
{
    return pc & 1;
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