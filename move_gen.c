


int m_bloom_node(struct Node *node) {
    
    struct Board *board = node->board;
    uint64_t bishops, rooks, knights, king, queens;
    uint64_t all_pieces, ally_pieces, enemy_pieces;
    node->children = malloc(sizeof(struct Node *) * MAX_CHILDREN);
    int children_count = 0;

    if (node->board.white_moves) {
        knights = board->bitboards[1];
        bishops = board->bitboards[2];
        rooks = board->bitboards[3];
        queens = board->bitboards[4];
        king = board->bitboards[5];

        ally_pieces = board->bitboards[0] | knights | bishops | rooks | queens | king;
        enemy_pieces = board->bitboards[6] | board->bitboards[7] | board->bitboards[8] | board->bitboards[9] | board->bitboards[10] | board->bitboards[11];
    }
    else {
        knights = board->bitboards[7];
        bishops = board->bitboards[8];
        rooks = board->bitboards[9];
        queens = board->bitboards[10];
        king = board->bitboards[11];

        ally_pieces = board->bitboards[6] | knights | bishops | rooks | queens | king;
        enemy_pieces = board->bitboards[0] | board->bitboards[1] | board->bitboards[2] | board->bitboards[3] | board->bitboards[4] | board->bitboards[5];
    }

    children_count += m_add_knight_moves(knights, ally_pieces, enemy_pieces, node, children_count);
    children_count += m_add_bishop_moves(bishops, ally_pieces, enemy_pieces, node, children_count);
    children_count += m_add_rook_moves(rooks, ally_pieces, enemy_pieces, node, children_count);

}

int m_add_rook_moves(uint64_t rooks, uint64_t allies, uint64_t enemies, struct Node *node, int children_count) {

    struct Node *child;
    uint64_t lsb_rook, lsb_moves, moves;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each rook
    while (rooks) {
        lsb_rook = rooks & (~rooks + 1);
        moves = solo_rook_moves(lsb_rook, allies, allies | enemies);

        //Loop for each move
        while (moves && (created_children + children_count < MAX_CHILDREN - 1)) {
            lsb_moves = moves & (~moves + 1);

            //Create new Node
            child = m_spawn_child(node);

            //If White
            if (node->board.white_moves) {
                //Clear origin square
                child->board.bitboards[3] &= ~lsb_rook;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[3] |= lsb_moves;

                //Change move order
                child->board.white_moves = 0;
            }
            //If Black
            else {
                //Clear origin square
                child->board.bitboards[9] &= ~lsb_rook;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[9] |= lsb_moves;

                //Change move order
                child->board.white_moves = 1;
            }

            //Common color board updates
            //Clear enpassent
            child->board.en_passent = 0;

            //Increment clocks
            if (captures) {
                child->board.halfmove_clock = 0;
            }
            else {
                child->board.halfmove_clock += 1;
            }
            child->board.fullmove_clock += 1;

            //Evaluate position
            child->eval = evaluate(child->board);

            //Set child's last_move
            old_index = get_index(lsb_rook);
            new_index = get_index(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = NULL;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;

            //Remove LSB from moves
            moves &= moves - 1;
        }

        //Remove LSB from rooks
        rooks &= rooks - 1;
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_bishop_moves(uint64_t bishops, uint64_t allies, uint64_t enemies, struct Node *node, int children_count) {

    struct Node *child;
    uint64_t lsb_bishop, lsb_moves, moves;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each bishop
    while (bishops) {
        lsb_bishop = bishops & (~bishops + 1);
        moves = solo_bishop_moves(lsb_bishop, allies, allies | enemies);

        //Loop for each move
        while (moves && (created_children + children_count < MAX_CHILDREN - 1)) {
            lsb_moves = moves & (~moves + 1);

            //Create new Node
            child = m_spawn_child(node);

            //If White
            if (node->board.white_moves) {
                //Clear origin square
                child->board.bitboards[2] &= ~lsb_bishop;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[2] |= lsb_moves;

                //Change move order
                child->board.white_moves = 0;
            }
            //If Black
            else {
                //Clear origin square
                child->board.bitboards[8] &= ~lsb_bishop;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[8] |= lsb_moves;

                //Change move order
                child->board.white_moves = 1;
            }

            //Common color board updates
            //Clear enpassent
            child->board.en_passent = 0;

            //Increment clocks
            if (captures) {
                child->board.halfmove_clock = 0;
            }
            else {
                child->board.halfmove_clock += 1;
            }
            child->board.fullmove_clock += 1;

            //Evaluate position
            child->eval = evaluate(child->board);

            //Set child's last_move
            old_index = get_index(lsb_bishop);
            new_index = get_index(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = NULL;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;

            //Remove LSB from moves
            moves &= moves - 1;
        }

        //Remove LSB from bishops
        bishops &= bishops - 1;
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_knight_moves(uint64_t knights, uint64_t allies, uint64_t enemies, struct Node *node, int children_count) {

    struct Node *child;
    uint64_t lsb_knight, lsb_moves, moves;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each knight
    while (knights) {
        lsb_knight = knights & (~knights + 1);
        moves = solo_knight_moves(lsb_knight, allies);

        //Loop for each move
        while (moves && (created_children + children_count < MAX_CHILDREN - 1)) {
            lsb_moves = moves & (~moves + 1);

            //Create new Node
            child = m_spawn_child(node);

            //If White
            if (node->board.white_moves) {
                //Clear origin square
                child->board.bitboards[1] &= ~lsb_knight;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[1] |= lsb_moves;

                //Change move order
                child->board.white_moves = 0;
            }
            //If Black
            else {
                //Clear origin square
                child->board.bitboards[7] &= ~lsb_knight;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[7] |= lsb_moves;

                //Change move order
                child->board.white_moves = 1;
            }

            //Common color board updates
            //Clear enpassent
            child->board.en_passent = 0;

            //Increment clocks
            if (captures) {
                child->board.halfmove_clock = 0;
            }
            else {
                child->board.halfmove_clock += 1;
            }
            child->board.fullmove_clock += 1;

            //Evaluate position
            child->eval = evaluate(child->board);

            //Set child's last_move
            old_index = get_index(lsb_knight);
            new_index = get_index(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = NULL;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;

            //Remove LSB from moves
            moves &= moves - 1;
        }

        //Remove LSB from knights
        knights &= knights - 1;
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

struct Node *spawn_child(struct Node *node) {
    int i;

    struct Node *new_node = malloc(sizeof(struct Node));
    
    new_node->board = node->board;
    new_node->visits = 1;
    new_node->depth = 0;
    new_node->eval = 0;
    new_node->children = NULL;
    new_node->parent = node;
    pthread_mutex_init(&(new_node->mutex), NULL);

    return new_node;
}

//Returns true if a piece is captured
int clear_destination_square(struct Board *board, uint64_t square) {
    int i, captures = 0;
    for (i = 0; i < 12; i++) {
        if (board->bitboards[i] & square) {
            board->bitboards[i] &= ~square;
            captures += 1
        }
    }
    return captures;
}

int is_king_in_check(struct Board board, int color) {
    uint64_t ally_pieces, enemy_pieces, all_pieces;

    if (color) {
        ally_pieces = board.bitboards[0] | board.bitboards[1] | board.bitboards[2] | board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
        enemy_pieces = board.bitboards[6] | board.bitboards[7] | board.bitboards[8] | board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
        all_pieces = ally_pieces | enemy_pieces;

        //Check for knight attacks
        if (solo_knight_moves(board.bitboards[5], ally_pieces) & board.bitboards[7]) {
            return 1;
        }
        //Check for bishop attacks
        if (solo_bishop_moves(board.bitboards[5], ally_pieces, all_pieces) & (board.bitboards[8] | board.bitboards[10])) {
            return 1;
        }
        //Check for rook attacks
        if (solo_rook_moves(board.bitboards[5], ally_pieces, all_pieces) & (board.bitboards[9] | board.bitboards[10])) {
            return 1;
        }
        //Check for other king attacks
        if (solo_king_moves(board.bitboards[5], ally_pieces) & board.bitboards[11]) {
            return 1;
        }
        //Check for  pawn attacks
        if (solo_pawn_checks(board.bitboards[5], enemy_pieces, color) & board.bitboards[6]) {
            return 1;
        }

    }
    else {
        enemy_pieces = board.bitboards[0] | board.bitboards[1] | board.bitboards[2] | board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
        ally_pieces = board.bitboards[6] | board.bitboards[7] | board.bitboards[8] | board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
        all_pieces = ally_pieces | enemy_pieces;

        //Check for knight attacks
        if (solo_knight_moves(board.bitboards[11], ally_pieces) & board.bitboards[1]) {
            return 1;
        }
        //Check for bishop attacks
        if (solo_bishop_moves(board.bitboards[11], ally_pieces, all_pieces) & (board.bitboards[2] | board.bitboards[4])) {
            return 1;
        }
        //Check for rook attacks
        if (solo_rook_moves(board.bitboards[11], ally_pieces, all_pieces) & (board.bitboards[3] | board.bitboards[4])) {
            return 1;
        }
        //Check for other king attacks
        if (solo_king_moves(board.bitboards[11], ally_pieces) & board.bitboards[5]) {
            return 1;
        }
        //Check for  pawn attacks
        if (solo_pawn_checks(board.bitboards[11], enemy_pieces, color) & board.bitboards[0]) {
            return 1;
        }

    }

    //The king is not under attack
    return 0;
}

uint64_t solo_pawn_checks(uint64_t solo_king_bb, uint64_t enemy_pieces, int is_white) {
    int square = LSB(solo_king_bb);
    if (is_white) {
        return w_pawn_attack_collisions[square] & enemy_pieces;
    }
    else {
        return b_pawn_attack_collisions[square] & enemy_pieces;
    }
}

uint64_t solo_knight_moves(uint64_t solo_knight_bb, uint64_t ally_pieces) {
    int square = LSB(solo_knight_bb);

    return knight_collisions[square] & ~ally_pieces;
}

uint64_t solo_bishop_moves(uint64_t solo_bishop_bb, uint64_t ally_pieces, uint64_t all_pieces) {
    int square = LSB(solo_bishop_bb);

    uint64_t occupied_bb = bishop_collisions[square] & all_pieces;
    int magic_index = (occupied_bb * bishop_magic_numbers[square]) >> 55;
    return bishop_magic_move_sets[square][magic_index] & ~ally_pieces;
}

uint64_t solo_rook_moves(uint64_t solo_rook_bb, uint64_t ally_pieces, uint64_t all_pieces) {
    int square = LSB(solo_rook_bb);

    uint64_t occupied_bb = rook_collisions[square] & all_pieces;
    int magic_index = (occupied_bb * rook_magic_numbers[square]) >> 52;
    return rook_magic_move_sets[square][magic_index] & ~ally_pieces;
}

uint64_t solo_king_moves(uint64_t solo_king_bb, uint64_t ally_pieces) {
    int square = LSB(solo_king_bb);

    return king_collisions[square] & ~ally_pieces;
}

