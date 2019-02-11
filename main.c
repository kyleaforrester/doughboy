void parse_uci(char *buffer, size_t buf_size) {

    printf("id name Doughboy %s 64\n", DOUGHBOY_VERSION);
    printf("id author Kyle Forrester\n");
    printf("\n");
    printf("option name Threads type spin default 1 min 1 max 128\n");
    printf("option name Ponder type check default false\n");
    printf("uciok\n");
    return;
}

void parse_setoption(char *buffer, size_t buf_size) {
    char **parsed_string = m_tokenize_input(buffer, buf_size);   
    char **p_itr;
    int parsed_size;
    struct Option **o_itr;
    for (p_itr = parsed_string; *p_itr; p_itr++);
    parsed_size = p_itr - parsed_string;

    if (parsed_size != 5) {
        return;
    }
    if (strcmp(parsed_string[0], "setoption") != 0 || strcmp(parsed_string[1], "name") != 0 || strcmp(parsed_string[3], "value") != 0) {
        return;
    }

    for (o_itr = options; *o_itr; o_itr++) {
        if (strcmp((*o_itr)->name, parsed_string[2]) == 0) {
            //Act like a switch/case for the option name
            if (strcmp((*o_itr)->name, "Threads") == 0) {
                (*o_itr)->spin = atoi(parsed_string[4]);
            }
            else if (strcmp((*o_itr)->name, "Ponder") == 0) {
                if (strcmp(parsed_string[4], "true") == 0) {
                    (*o_itr)->check = 1;
                }
                else {
                    (*o_itr)->check = 0;
                }
            }
        }
    }

    free_tokenize_input(parsed_string);    
}

void parse_isready(char *buffer, size_t buf_size) {
    printf("readyok\n");
}

void parse_go(char *buffer, size_t buf_size) {
    int word_count, i;
    char **com_tokens = m_tokenize_input(buffer, buf_size);
    char *com_token_itr;
    int ponder = 0, wtime = 0, btime = 0, winc = 0, binc = 0, movestogo = 0, depth = 0, nodes = 0, movetime = 0, infinite = 0;

    if (!root) {
        root = malloc(sizeof(struct Node));
        root->board = *curr_board;
        root->visits = 0;
        root->height = 0;
        root->depth = 0;
        root->eval = 0;
        root->is_checkmate = 0;
        root->is_stalemate = 0;
        root->parent = NULL;
        root->children = NULL;
        root->child_count = 0;
        pthread_mutex_init(&(root->mutex), NULL);
    }

    if (strcmp(com_tokens[0], "go") != 0) {
        free_tokenize_input(com_tokens);
        return;
    }

    for (com_token_itr = (*com_tokens+1); *com_token_itr; com_token_itr++) {
        if (strcmp(com_token_itr, "ponder") == 0) {
            ponder = 1;
        }
        else if (strcmp(com_token_itr, "wtime") == 0) {
            wtime = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "btime") == 0) {
            btime = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "winc") == 0) {
            winc = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "binc") == 0) {
            binc = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "movestogo") == 0) {
            movestogo = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "depth") == 0) {
            depth = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "nodes") == 0) {
            nodes = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "movetime") == 0) {
            movetime = atoi(com_token_itr + 1);
        }
        else if (strcmp(com_token_itr, "infinite") == 0) {
            infinite = 1;
        }
    }

    spawn_go_workers(ponder, wtime, btime, winc, binc, movestogo, depth, nodes, movetime, infinite);
}

void parse_stop(char *buffer, size_t buf_size) {
    stop_pondering = 1;
}

void parse_position(char *buffer, size_t buf_size) {
    int word_count, i;
    char **com_tokens = m_tokenize_input(buffer, buf_size);

    if (strcmp(com_tokens[0], "position") != 0) {
        free_tokenize_input(com_tokens);
        return;
    }

    //Need to check argument size for security purposes
    //So people cannot so easily crash the program
    for (word_count = 0; com_tokens[word_count]; word_count++);

    if (word_count < 2) {
        free_tokenize_input(com_tokens);
        return;
    }

    //Example commands:
    //position startpos moves e2e4
    //position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4
    if (strcmp(com_tokens[1], "startpos") == 0) {
        set_to_fen(curr_board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-", "0", "1");
        if (word_count > 3 && strcmp(com_tokens[2], "moves") == 0) {
            for (i = 3; i < word_count; i++) {
                do_move(curr_board, com_tokens[i]);
            }
        }
    }
    else if (strcmp(com_tokens[1], "fen") == 0) {
        if (word_count < 8 || char_count(com_tokens[2], strlen(com_tokens[2]), '/') != 7) {
            free_tokenize_input(com_tokens);
            return;
        }
        set_to_fen(curr_board, com_tokens[2], com_tokens[3], com_tokens[4], com_tokens[5], com_tokens[6], com_tokens[7]);
        if (word_count > 9 && strcmp(com_tokens[8], "moves") == 0) {
            for (i = 9; i < word_count; i++) {
                do_move(curr_board, com_tokens[i]);
            }
        }
    }

    if (root) {
        free_node(root);
        root = NULL;
    }
    free_tokenize_input(com_tokens);
}

void parse_printoptions(char *buffer, size_t buf_size) {
    struct Option **o_itr;
    for (o_itr = options; *o_itr; o_itr++) {
        if (strcmp((*o_itr)->name, "Threads") == 0) {
            printf("Threads: %d\n", (*o_itr)->spin);
        }
        else if (strcmp((*o_itr)->name, "Ponder") == 0) {
            if ((*o_itr)->check == 0) {
                printf("Ponder: false\n");
            }
            else {
                printf("Ponder: true\n");
            }
        }
    }
}

void parse_printboard(char *buffer, size_t buf_size) {
    char board[64];
    int x, y;

    //Each square will have = on top edge and | for side edge.
    //Empty squares will be hyphens -
    fill_char_board(board, sizeof(board));

    //Print board
    //Print first row
    //Loop for each row starting from top
    printf("\n=================\n");
    for (y = 7; y >= 0; y--) {
        //Loop for each column starting from left
        printf("|"); 
        for (x = 0; x < 8; x++) {
            printf("%c|", board[8*y+x]);
        }
        printf("\n=================\n");
    }
}

void initialize_options() {
    struct Option **o_itr;
    options = malloc(sizeof(struct Option *) * 3);
    o_itr = options;
    *o_itr = malloc(sizeof(struct Option));
    **o_itr = (struct Option){.name="Threads", .check=0, .string="", .spin=1};
    o_itr++;
    *o_itr = malloc(sizeof(struct Option));
    **o_itr = (struct Option){.name="Ponder", .check=0, .string="", .spin=0};
    o_itr++;
    *o_itr = 0;
}

void initialize_board() {
    curr_board = malloc(sizeof(struct Board));
    //White
    curr_board->bitboards[0] = 0xff00ULL;
    curr_board->bitboards[1] = 0x42ULL;
    curr_board->bitboards[2] = 0x24ULL;
    curr_board->bitboards[3] = 0x81ULL;
    curr_board->bitboards[4] = 0x8ULL;
    curr_board->bitboards[5] = 0x10ULL;
    //Black
    curr_board->bitboards[6] = 0xff000000000000ULL;
    curr_board->bitboards[7] = 0x4200000000000000ULL;
    curr_board->bitboards[8] = 0x2400000000000000ULL;
    curr_board->bitboards[9] = 0x8100000000000000ULL;
    curr_board->bitboards[10] = 0x800000000000000ULL;
    curr_board->bitboards[11] = 0x1000000000000000ULL;

    curr_board->white_moves = 1;
    curr_board->white_king_castle = 1;
    curr_board->white_queen_castle = 1;
    curr_board->black_king_castle = 1;
    curr_board->black_queen_castle = 1;
    curr_board->en_passent = 0;
    curr_board->halfmove_clock = 0;
    curr_board->fullmove_clock = 1;
}

int main(int argc, char **argv) {

    printf("Doughboy %s 64 by Kyle Forrester\n", DOUGHBOY_VERSION);

    size_t buf_size = MAX_BUF_SIZE;
    char *buffer = malloc(sizeof(char) * buf_size);
    char first_word[MAX_BUF_SIZE];
    int debug = 0;

    initialize_options();
    initialize_board();
    stop_pondering = 0;

    while (getline(&buffer, &buf_size, stdin) <= MAX_BUF_SIZE) {
        //Strip the newline char off buffer
        buffer[strlen(buffer)-1] = 0;
        if (!str_first_word(first_word, MAX_BUF_SIZE, buffer)) {
            printf("Failed to parse first word of:\n%s\n",buffer);
            continue;
        }
        if (strcmp(first_word, "uci") == 0) {
            LOG(debug, "UCI detected!");
            parse_uci(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "setoption") == 0) {
            LOG(debug, "SetOption detected!");
            parse_setoption(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "isready") == 0) {
            LOG(debug, "IsReady detected!");
            parse_isready(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "printoptions") == 0) {
            LOG(debug, "Printoptions detected!");
            parse_printoptions(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "printboard") == 0) {
            LOG(debug, "Printboard detected!");
            parse_printboard(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "position") == 0) {
            LOG(debug, "Position detected!");
            parse_position(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "go") == 0) {
            LOG(debug, "Go detected!");
            parse_go(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "stop") == 0) {
            LOG(debug, "Stop detected!");
            parse_stop(buffer, strlen(buffer));
        }
        else {
            LOG(debug, "Received Unknown command.");
        }
    }


    //TODO: Parse the sent command
}

