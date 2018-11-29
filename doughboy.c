#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DOUGHBOY_VERSION "v0.1.0"
#define LOG(x,y) if (x) printf(y)
#define MAX_BUF_SIZE 4096
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))

#if __WORDSIZE == 64
typedef unsigned long int       uint64_t;
#else
__extension__
typedef unsigned long long int  uint64_t;
#endif

struct Option {
    char name[50];
    int check;
    char string[50];
    int spin;
};
struct Option **options;

struct Board {
    //P,N,B,R,Q,K,p,n,b,r,q,k
    uint64_t bitboards[12];
    int white_moves;
    int white_king_castle;
    int white_queen_castle;
    int black_king_castle;
    int black_queen_castle;
    uint64_t en_passent;
    int halfmove_clock;
};
struct Board *curr_board;

#include "utilities.c"
#include "board.c"
#include "main.c"
