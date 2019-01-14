#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#define DOUGHBOY_VERSION "v0.1.0"
#define LOG(x,y) if (x) printf(y)
#define MAX_BUF_SIZE 4096
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))

#ifndef __uint32_t_defined
typedef unsigned int            uint32_t;
# define __uint32_t_defined
#endif
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
    int fullmove_clock;
};
struct Board *curr_board;

struct Node {
    struct Board board;
    int visits;
    int depth;
    int cp;
    struct Node **children;
    struct Node *parent;
    char last_move[6];
    pthread_mutex_t mutex;
};
struct Node *root;

int stop_pondering;

#include "utilities.c"
#include "board.c"
#include "search.c"
#include "main.c"
