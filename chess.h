#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#define DOUGHBOY_VERSION "v0.1.0"
#define LOG(x,y) if (x) printf(y)
#define MAX_BUF_SIZE 4096
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define LSB(v)    __builtin_ctzll(v)
#define MAX_CHILDREN 200

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
    int height;
    double eval;
    int is_checkmate;
    int is_stalemate;
    struct Node **children;
    int child_count;
    struct Node *parent;
    char last_move[6];
    pthread_mutex_t mutex;
};
struct Node *root;

int stop_pondering;

//This struct is used so I can pass multiple arguments to a thread.
//pthread only allows a single address to pass to a new thread.
struct Go_Args {
    int ponder;
    int ponder_enabled;
    int wtime;
    int btime;
    int winc;
    int binc;
    int movestogo;
    int depth;
    int nodes;
    int movetime;
    int infinite;
    int index;
};

//board.c
void clear_bitboards(struct Board *board, uint64_t bb_clear);
void do_move(struct Board *board, char *move);
void set_piece_placement(struct Board *board, char *piece_placement);
void set_color(struct Board *board, char *color);
void set_castling(struct Board *board, char *castling);
void set_en_passent(struct Board *board, char *en_passent);
void set_halfmove(struct Board *board, char *halfmove);
void set_fullmove(struct Board *board, char *fullmove);
void set_to_fen(struct Board *board, char *piece_placement, char *color, char *castling, char *en_passent, char *halfmove, char *fullmove);
void fill_char_board_bb(char *board, size_t board_size, uint64_t bb, char piece);
void fill_char_board(char *board, size_t board_size);

/*
//magic_values.c
int lsb_index_translation[64];
char col_lookup_table[64];
char row_lookup_table[64];
uint64_t knight_collisions[64];
uint64_t bishop_collisions[64];
uint64_t rook_collisions[64];
uint64_t king_collisions[64];
uint64_t w_pawn_attack_collisions[64];
uint64_t b_pawn_attack_collisions[64];
uint64_t rook_magic_numbers[64];
uint64_t bishop_magic_numbers[64][512];
uint64_t rook_magic_move_sets[64][4096];
uint64_t bishop_magic_move_sets[64];
*/

//main.c
void parse_uci(char *buffer, size_t buf_size);
void parse_setoption(char *buffer, size_t buf_size);
void parse_isready(char *buffer, size_t buf_size);
void parse_go(char *buffer, size_t buf_size);
void parse_stop(char *buffer, size_t buf_size);
void parse_position(char *buffer, size_t buf_size);
void parse_printoptions(char *buffer, size_t buf_size);
void parse_printboard(char *buffer, size_t buf_size);
void initialize_options();
void initialize_board();
int main(int argc, char **argv);

//move_gen.c
int m_bloom_node(struct Node *node, double (*eval_func)(struct Board board));
struct Node *m_spawn_pawn_child(struct Node *node, uint64_t lsb_pawn, uint64_t lsb_moves, char transform, double (*eval_func)(struct Board board));
int m_add_pawn_moves(uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
int m_add_king_moves(uint64_t king, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
int m_add_queen_moves(uint64_t queens, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
int m_add_rook_moves(uint64_t rooks, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
int m_add_bishop_moves(uint64_t bishops, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
int m_add_knight_moves(uint64_t knights, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, double (*eval_func)(struct Board board));
struct Node *m_spawn_child(struct Node *node);
int clear_destination_square(struct Board *board, uint64_t square);
int are_spaces_occupied(struct Board board, uint64_t spaces);
int is_square_in_check(struct Board board, int color, uint64_t bb);
uint64_t solo_pawn_checks(uint64_t solo_king_bb, uint64_t enemy_pawns, int is_white);
uint64_t solo_pawn_moves(uint64_t solo_pawn_bb, uint64_t all_pieces, int is_white);
uint64_t solo_pawn_attacks(uint64_t solo_pawn_bb, uint64_t enemy_pieces, int is_white);
uint64_t solo_knight_moves(uint64_t solo_knight_bb, uint64_t ally_pieces);
uint64_t solo_bishop_moves(uint64_t solo_bishop_bb, uint64_t ally_pieces, uint64_t all_pieces);
uint64_t solo_rook_moves(uint64_t solo_rook_bb, uint64_t ally_pieces, uint64_t all_pieces);
uint64_t solo_king_moves(uint64_t solo_king_bb, uint64_t ally_pieces);

//search.c
int get_pv(struct Node *r_node, char *buffer, size_t buf_size);
void *go_worker(void *argument);
struct Node *select_child_nav(struct Node *parent, uint64_t *prng_state);
void collapse_values(struct Node *my_node);
void set_root_node(int ponder);
void spawn_go_workers(int ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int movetime, int infinite);

//utilities.c
char *knr_fgets(char *s, int n, FILE *iop);
int str_first_word(char *buffer, int buf_size, char *input);
void strip_line_endings(char *input, size_t input_size);
int char_count(char *str, size_t str_len, char match);
char **m_tokenize_input(char *input, size_t input_size);
void free_tokenize_input(char **input);
void free_node(struct Node *my_node);
int bit_count(uint64_t bb);
uint32_t spcg32(uint64_t s[1]);
uint64_t get_nanos();

//basic_eval.c
double evaluate(struct Board board);
double evaluate_board(struct Board board);
double evaluate_pawns(struct Board board, int color);
double evaluate_knights(struct Board board, int color);
double evaluate_bishops(struct Board board, int color);
double evaluate_rooks(struct Board board, int color);
double evaluate_queens(struct Board board, int color);
