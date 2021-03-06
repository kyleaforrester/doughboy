


int m_bloom_node(struct Node *node, int recursion) {
    
    struct Board board = node->board;
    uint64_t bishops, rooks, knights, king, queens;
    uint64_t all_pieces, ally_pieces, enemy_pieces;
    node->children = malloc(sizeof(struct Node *) * MAX_CHILDREN);
    int children_count = 0;

    if (node->board.white_moves) {
        knights = board.bitboards[1];
        bishops = board.bitboards[2];
        rooks = board.bitboards[3];
        queens = board.bitboards[4];
        king = board.bitboards[5];

        ally_pieces = board.bitboards[0] | knights | bishops | rooks | queens | king;
        enemy_pieces = board.bitboards[6] | board.bitboards[7] | board.bitboards[8] | board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
    }
    else {
        knights = board.bitboards[7];
        bishops = board.bitboards[8];
        rooks = board.bitboards[9];
        queens = board.bitboards[10];
        king = board.bitboards[11];

        ally_pieces = board.bitboards[6] | knights | bishops | rooks | queens | king;
        enemy_pieces = board.bitboards[0] | board.bitboards[1] | board.bitboards[2] | board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
    }

    children_count += m_add_knight_moves(knights, ally_pieces, enemy_pieces, node, children_count, recursion);
    children_count += m_add_bishop_moves(bishops, ally_pieces, enemy_pieces, node, children_count, recursion);
    children_count += m_add_rook_moves(rooks, ally_pieces, enemy_pieces, node, children_count, recursion);
    children_count += m_add_queen_moves(queens, ally_pieces, enemy_pieces, node, children_count, recursion);
    children_count += m_add_king_moves(king, ally_pieces, enemy_pieces, node, children_count, recursion);
    children_count += m_add_pawn_moves(ally_pieces, enemy_pieces, node, children_count, recursion);

    node->child_count = children_count;

    //Set checkmate or stalemate flags so we know not to bloom this node again
    if (children_count == 0) {
        if (node->board.white_moves) {
            //It is white's turn
            if (is_square_in_check(node->board, 0, king)) {
                //Black could kill the king
                node->is_checkmate = 1;
            }
            else {
                node->is_stalemate = 1;
            }
        }
        else {
            //It is black's turn
            if (is_square_in_check(node->board, 1, king)) {
                //White could kill the king
                node->is_checkmate = 1;
            }
            else {
                node->is_stalemate = 1;
            }

        }
    }

    return children_count;
}

struct Node *m_spawn_pawn_child(struct Node *node, uint64_t lsb_pawn, uint64_t lsb_moves, char transform, int recursion) {
    struct Node *child;
    int captures, old_index, new_index;

    //Create new Node
    child = m_spawn_child(node);

    //If White
    if (node->board.white_moves) {
        //Clear origin square
        child->board.bitboards[0] &= ~lsb_pawn;

        //Clear destination square
        captures = clear_destination_square(&(child->board), lsb_moves);

        //Clear en-passent destination square
        if (node->board.en_passent & lsb_moves) {
            child->board.bitboards[6] &= ~(lsb_moves >> 8);
        }

        //Add destination square
        //Can differ if a pawn promotion
        if (transform == 'p') {
            child->board.bitboards[0] |= lsb_moves;
        }
        else if (transform == 'n') {
            child->board.bitboards[1] |= lsb_moves;
        }
        else if (transform == 'b') {
            child->board.bitboards[2] |= lsb_moves;
        }
        else if (transform == 'r') {
            child->board.bitboards[3] |= lsb_moves;
        }
        //It's a queen
        else {
            child->board.bitboards[4] |= lsb_moves;
        }

        //Change move order
        child->board.white_moves = 0;

        //Set en-passent
        if (lsb_pawn & 0xff00ULL && lsb_moves & 0xff000000ULL) {
            child->board.en_passent = lsb_pawn << 8;
        }
        else {
            child->board.en_passent = 0;
        }
    }
    //If Black
    else {
        //Clear origin square
        child->board.bitboards[6] &= ~lsb_pawn;

        //Clear destination square
        captures = clear_destination_square(&(child->board), lsb_moves);

        //Clear en-passent destination square
        if (node->board.en_passent & lsb_moves) {
            child->board.bitboards[0] &= ~(lsb_moves << 8);
        }

        //Add destination square
        //Can differ if a pawn promotion
        if (transform == 'p') {
            child->board.bitboards[6] |= lsb_moves;
        }
        else if (transform == 'n') {
            child->board.bitboards[7] |= lsb_moves;
        }
        else if (transform == 'b') {
            child->board.bitboards[8] |= lsb_moves;
        }
        else if (transform == 'r') {
            child->board.bitboards[9] |= lsb_moves;
        }
        //It's a queen
        else {
            child->board.bitboards[10] |= lsb_moves;
        }

        //Change move order
        child->board.white_moves = 1;

        //Set en-passent
        if (lsb_pawn & 0xff000000000000ULL && lsb_moves & 0xff00000000ULL) {
            child->board.en_passent = lsb_pawn >> 8;
        }
        else {
            child->board.en_passent = 0;
        }
    }

    //Common color board updates
    //Increment clocks
    child->board.halfmove_clock = 0;
    child->board.fullmove_clock += 1;

    //Evaluate position
    child->eval = evaluate(child->board, recursion);

    //Set child's last_move
    old_index = LSB(lsb_pawn);
    new_index = LSB(lsb_moves);
    child->last_move[0] = col_lookup_table[old_index];
    child->last_move[1] = row_lookup_table[old_index];
    child->last_move[2] = col_lookup_table[new_index];
    child->last_move[3] = row_lookup_table[new_index];
    if (transform != 'p') {
        child->last_move[4] = transform;
        child->last_move[5] = 0;
    }
    else {
        child->last_move[4] = 0;
    }

    return child;
}

int m_add_pawn_moves(uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t pawns, lsb_pawn, lsb_moves, moves, king_location;
    int created_children = 0, captures, old_index, new_index;
    char transformations[5], *iter;

    //I am white
    if (node->board.white_moves) {
        pawns = node->board.bitboards[0];
        //Loop for each pawn
        for (; pawns; pawns &= pawns - 1) {
            lsb_pawn = pawns & (~pawns + 1);
            moves = solo_pawn_moves(lsb_pawn, allies | enemies, 1);
            moves |= solo_pawn_attacks(lsb_pawn, enemies, 1);

            //Loop for each move
            for (; moves; moves &= moves - 1) {
                lsb_moves = moves & (~moves + 1);

                //Initialize pawn promotion transformations loop
                if (lsb_moves & 0xff00000000000000ULL) {
                    strcpy(transformations, "nbrq");
                }
                else {
                    transformations[0] = 'p';
                    transformations[1] = 0;
                }

                for (iter = transformations; *iter && (created_children + children_count < MAX_CHILDREN - 1); iter++) {

                    //Create new Node
                    child = m_spawn_pawn_child(node, lsb_pawn, lsb_moves, *iter, recursion);

                    //Check for checks
                    if (is_square_in_check(child->board, node->board.white_moves, child->board.bitboards[5])) {
                        //Discard this child
                        free(child);
                        continue;
                    }

                    //Add new child to the parent
                    node->children[children_count + created_children] = child;
                    created_children += 1;
                }
            }
        }
    }
    //I am black
    else {
        pawns = node->board.bitboards[6];
        //Loop for each pawn
        for (; pawns; pawns &= pawns - 1) {
            lsb_pawn = pawns & (~pawns + 1);
            moves = solo_pawn_moves(lsb_pawn, allies | enemies, 0);
            moves |= solo_pawn_attacks(lsb_pawn, enemies, 0);

            //Loop for each move
            for (; moves; moves &= moves - 1) {
                lsb_moves = moves & (~moves + 1);

                //Initialize pawn promotion transformations loop
                if (lsb_moves & 0xffULL) {
                    strcpy(transformations, "nbrq");
                }
                else {
                    transformations[0] = 'p';
                    transformations[1] = 0;
                }

                for (iter = transformations; *iter && (created_children + children_count < MAX_CHILDREN - 1); iter++) {

                    //Create new Node
                    child = m_spawn_pawn_child(node, lsb_pawn, lsb_moves, *iter, recursion);

                    //Check for checks
                    if (is_square_in_check(child->board, node->board.white_moves, child->board.bitboards[11])) {
                        //Discard this child
                        free(child);
                        continue;
                    }

                    //Add new child to the parent
                    node->children[children_count + created_children] = child;
                    created_children += 1;
                }
            }
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_king_moves(uint64_t king, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t lsb_moves, moves, king_location, castle_occupations;
    int created_children = 0, captures, old_index, new_index, i;

    moves = solo_king_moves(king, allies);

    //Loop for each move
    for (; moves && (created_children + children_count < MAX_CHILDREN - 1); moves &= moves - 1) {
        lsb_moves = moves & (~moves + 1);

        //Create new Node
        child = m_spawn_child(node);

        //If White
        if (node->board.white_moves) {
            //Clear origin square
            child->board.bitboards[5] &= ~king;

            //Clear destination square
            captures = clear_destination_square(&(child->board), lsb_moves);

            //Add destination square
            child->board.bitboards[5] |= lsb_moves;

            //Change move order
            child->board.white_moves = 0;

            //Remove castle rights
            child->board.white_king_castle = 0;
            child->board.white_queen_castle = 0;

            //Locate the King
            king_location = child->board.bitboards[5];
        }
        //If Black
        else {
            //Clear origin square
            child->board.bitboards[11] &= ~king;

            //Clear destination square
            captures = clear_destination_square(&(child->board), lsb_moves);

            //Add destination square
            child->board.bitboards[11] |= lsb_moves;

            //Change move order
            child->board.white_moves = 1;

            //Remove castle rights
            child->board.black_king_castle = 0;
            child->board.black_queen_castle = 0;

            //Locate the King
            king_location = child->board.bitboards[11];
        }

        //Check for checks
        if (is_square_in_check(child->board, node->board.white_moves, king_location)) {
            //Discard this child
            free(child);
            continue;
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
        child->eval = evaluate(child->board, recursion);

        //Set child's last_move
        old_index = LSB(king);
        new_index = LSB(lsb_moves);
        child->last_move[0] = col_lookup_table[old_index];
        child->last_move[1] = row_lookup_table[old_index];
        child->last_move[2] = col_lookup_table[new_index];
        child->last_move[3] = row_lookup_table[new_index];
        child->last_move[4] = 0;

        //Add new child to the parent
        node->children[children_count + created_children] = child;
        created_children += 1;
    }

    //Look for castling moves
    //For White
    if (node->board.white_moves) {
        if (node->board.white_king_castle) {
            //Check if spaces are occupied
            castle_occupations = 0x60ULL;
            if (!are_spaces_occupied(node->board, castle_occupations) &&
                node->board.bitboards[5] & 0x10ULL &&
                node->board.bitboards[3] & 0x80ULL &&
                !is_square_in_check(node->board, node->board.white_moves, 0x10ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x20ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x40ULL)) {
                //Create a new child for the castle
                child = m_spawn_child(node);
                //Move the King and Rook around
                child->board.bitboards[5] = 0x40ULL;
                child->board.bitboards[3] &= ~0x80ULL;
                child->board.bitboards[3] |= 0x20ULL;
                //Set all the regular new child variables
                child->board.white_moves = 0;
                child->board.en_passent = 0;
                child->board.halfmove_clock += 1;
                child->board.fullmove_clock += 1;
                child->board.white_king_castle = 0;
                child->board.white_queen_castle = 0;
                child->eval = evaluate(child->board, recursion);
                strcpy(child->last_move, "e1g1");
                node->children[children_count + created_children] = child;
                created_children += 1;
            }
        }
        if (node->board.white_queen_castle) {
            //Check if spaces are occupied
            castle_occupations = 0xeULL;
            if (!are_spaces_occupied(node->board, castle_occupations) &&
                node->board.bitboards[5] & 0x10ULL &&
                node->board.bitboards[3] & 0x1ULL &&
                !is_square_in_check(node->board, node->board.white_moves, 0x4ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x8ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x10ULL)) {
                //Create a new child for the castle
                child = m_spawn_child(node);
                //Move the King and Rook around
                child->board.bitboards[5] = 0x4ULL;
                child->board.bitboards[3] &= ~0x1ULL;
                child->board.bitboards[3] |= 0x8ULL;
                //Set all the regular new child variables
                child->board.white_moves = 0;
                child->board.en_passent = 0;
                child->board.halfmove_clock += 1;
                child->board.fullmove_clock += 1;
                child->board.white_king_castle = 0;
                child->board.white_queen_castle = 0;
                child->eval = evaluate(child->board, recursion);
                strcpy(child->last_move, "e1c1");
                node->children[children_count + created_children] = child;
                created_children += 1;
            }
        }
    }
    //For Black
    else {
        if (node->board.black_king_castle) {
            //Check if spaces are occupied
            castle_occupations = 0x6000000000000000ULL;
            if (!are_spaces_occupied(node->board, castle_occupations) &&
                node->board.bitboards[11] & 0x1000000000000000ULL &&
                node->board.bitboards[9] & 0x8000000000000000ULL &&
                !is_square_in_check(node->board, node->board.white_moves, 0x1000000000000000ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x2000000000000000ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x4000000000000000ULL)) {
                //Create a new child for the castle
                child = m_spawn_child(node);
                //Move the King and Rook around
                child->board.bitboards[11] = 0x4000000000000000ULL;
                child->board.bitboards[9] &= ~0x8000000000000000ULL;
                child->board.bitboards[9] |= 0x2000000000000000ULL;
                //Set all the regular new child variables
                child->board.white_moves = 1;
                child->board.en_passent = 0;
                child->board.halfmove_clock += 1;
                child->board.fullmove_clock += 1;
                child->board.black_king_castle = 0;
                child->board.black_queen_castle = 0;
                child->eval = evaluate(child->board, recursion);
                strcpy(child->last_move, "e8g8");
                node->children[children_count + created_children] = child;
                created_children += 1;
            }
        }
        if (node->board.black_queen_castle) {
            //Check if spaces are occupied
            castle_occupations = 0xe00000000000000ULL;
            if (!are_spaces_occupied(node->board, castle_occupations) &&
                node->board.bitboards[11] & 0x1000000000000000ULL &&
                node->board.bitboards[9] & 0x100000000000000ULL &&
                !is_square_in_check(node->board, node->board.white_moves, 0x400000000000000ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x800000000000000ULL) &&
                !is_square_in_check(node->board, node->board.white_moves, 0x1000000000000000ULL)) {
                //Create a new child for the castle
                child = m_spawn_child(node);
                //Move the King and Rook around
                child->board.bitboards[11] = 0x400000000000000ULL;
                child->board.bitboards[9] &= ~0x100000000000000ULL;
                child->board.bitboards[9] |= 0x800000000000000ULL;
                //Set all the regular new child variables
                child->board.white_moves = 1;
                child->board.en_passent = 0;
                child->board.halfmove_clock += 1;
                child->board.fullmove_clock += 1;
                child->board.black_king_castle = 0;
                child->board.black_queen_castle = 0;
                child->eval = evaluate(child->board, recursion);
                strcpy(child->last_move, "e8c8");
                node->children[children_count + created_children] = child;
                created_children += 1;
            }
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_queen_moves(uint64_t queens, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t lsb_queen, lsb_moves, moves, king_location;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each queen
    for (; queens; queens &= queens - 1) {
        lsb_queen = queens & (~queens + 1);
        moves = solo_bishop_moves(lsb_queen, allies, allies | enemies) |
                solo_rook_moves(lsb_queen, allies, allies | enemies);

        //Loop for each move
        for (; moves && (created_children + children_count < MAX_CHILDREN - 1); moves &= moves - 1) {
            lsb_moves = moves & (~moves + 1);

            //Create new Node
            child = m_spawn_child(node);

            //If White
            if (node->board.white_moves) {
                //Clear origin square
                child->board.bitboards[4] &= ~lsb_queen;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[4] |= lsb_moves;

                //Change move order
                child->board.white_moves = 0;

                //Locate the King
                king_location = child->board.bitboards[5];
            }
            //If Black
            else {
                //Clear origin square
                child->board.bitboards[10] &= ~lsb_queen;

                //Clear destination square
                captures = clear_destination_square(&(child->board), lsb_moves);

                //Add destination square
                child->board.bitboards[10] |= lsb_moves;

                //Change move order
                child->board.white_moves = 1;

                //Locate the King
                king_location = child->board.bitboards[11];
            }

            //Check for checks
            if (is_square_in_check(child->board, node->board.white_moves, king_location)) {
                //Discard this child
                free(child);
                continue;
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
            child->eval = evaluate(child->board, recursion);

            //Set child's last_move
            old_index = LSB(lsb_queen);
            new_index = LSB(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = 0;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_rook_moves(uint64_t rooks, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t lsb_rook, lsb_moves, moves, king_location;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each rook
    for (; rooks; rooks &= rooks - 1) {
        lsb_rook = rooks & (~rooks + 1);
        moves = solo_rook_moves(lsb_rook, allies, allies | enemies);

        //Loop for each move
        for (; moves && (created_children + children_count < MAX_CHILDREN - 1); moves &= moves - 1) {
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

                //Remove castle rights
                if (lsb_rook == 0x1ULL) {
                    child->board.white_queen_castle = 0;
                }
                else if (lsb_rook == 0x80ULL) {
                    child->board.white_king_castle = 0;
                }

                //Locate the King
                king_location = child->board.bitboards[5];
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

                //Remove castle rights
                if (lsb_rook == 0x100000000000000ULL) {
                    child->board.black_queen_castle = 0;
                }
                else if (lsb_rook == 0x8000000000000000ULL) {
                    child->board.black_king_castle = 0;
                }

                //Locate the King
                king_location = child->board.bitboards[11];
            }

            //Check for checks
            if (is_square_in_check(child->board, node->board.white_moves, king_location)) {
                //Discard this child
                free(child);
                continue;
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
            child->eval = evaluate(child->board, recursion);

            //Set child's last_move
            old_index = LSB(lsb_rook);
            new_index = LSB(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = 0;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_bishop_moves(uint64_t bishops, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t lsb_bishop, lsb_moves, moves, king_location;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each bishop
    for (; bishops; bishops &= bishops - 1) {
        lsb_bishop = bishops & (~bishops + 1);
        moves = solo_bishop_moves(lsb_bishop, allies, allies | enemies);

        //Loop for each move
        for (; moves && (created_children + children_count < MAX_CHILDREN - 1); moves &= moves - 1) {
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

                //Locate the King
                king_location = child->board.bitboards[5];
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

                //Locate the King
                king_location = child->board.bitboards[11];
            }

            //Check for checks
            if (is_square_in_check(child->board, node->board.white_moves, king_location)) {
                //Discard this child
                free(child);
                continue;
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
            child->eval = evaluate(child->board, recursion);

            //Set child's last_move
            old_index = LSB(lsb_bishop);
            new_index = LSB(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = 0;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

int m_add_knight_moves(uint64_t knights, uint64_t allies, uint64_t enemies, struct Node *node, int children_count, int recursion) {

    struct Node *child;
    uint64_t lsb_knight, lsb_moves, moves, king_location;
    int created_children = 0, captures, old_index, new_index;

    //Loop for each knight
    for (; knights; knights &= knights - 1) {
        lsb_knight = knights & (~knights + 1);
        moves = solo_knight_moves(lsb_knight, allies);

        //Loop for each move
        for (; moves && (created_children + children_count < MAX_CHILDREN - 1); moves &= moves - 1) {
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

                //Locate the King
                king_location = child->board.bitboards[5];
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

                //Locate the King
                king_location = child->board.bitboards[11];
            }

            //Check for checks
            if (is_square_in_check(child->board, node->board.white_moves, king_location)) {
                //Discard this child
                free(child);
                continue;
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
            child->eval = evaluate(child->board, recursion);

            //Set child's last_move
            old_index = LSB(lsb_knight);
            new_index = LSB(lsb_moves);
            child->last_move[0] = col_lookup_table[old_index];
            child->last_move[1] = row_lookup_table[old_index];
            child->last_move[2] = col_lookup_table[new_index];
            child->last_move[3] = row_lookup_table[new_index];
            child->last_move[4] = 0;

            //Add new child to the parent
            node->children[children_count + created_children] = child;
            created_children += 1;
        }
    }

    //Set the last child to NULL
    node->children[children_count + created_children] = NULL;

    return created_children;
}

struct Node *m_spawn_child(struct Node *node) {
    int i;

    struct Node *new_node = malloc(sizeof(struct Node));
    
    new_node->board = node->board;
    new_node->visits = 1;
    new_node->depth = 0;
    new_node->height = node->height + 1;
    new_node->is_checkmate = 0;
    new_node->mate = 0;
    new_node->is_stalemate = 0;
    new_node->eval = 0;
    new_node->children = NULL;
    new_node->child_count = 0;
    new_node->parent = node;
    new_node->proc_threads = 0;
    pthread_mutex_init(&(new_node->mutex), NULL);

    return new_node;
}

//Returns true if a piece is captured
int clear_destination_square(struct Board *board, uint64_t square) {
    int i, captures = 0;
    for (i = 0; i < 12; i++) {
        if (board->bitboards[i] & square) {
            board->bitboards[i] &= ~square;
            captures += 1;
        }
    }
    return captures;
}

int are_spaces_occupied(struct Board board, uint64_t spaces) {
    int i;
    for (i = 0; i < 12; i++) {
        if (board.bitboards[i] & spaces) {
            return 1;
        }
    }
    return 0;
}

int is_square_in_check(struct Board board, int color, uint64_t bb) {
    uint64_t ally_pieces, enemy_pieces, all_pieces;

    if (color) {
        ally_pieces = board.bitboards[0] | board.bitboards[1] | board.bitboards[2] | board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
        enemy_pieces = board.bitboards[6] | board.bitboards[7] | board.bitboards[8] | board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
        all_pieces = ally_pieces | enemy_pieces;

        //Check for knight attacks
        if (solo_knight_moves(bb, ally_pieces) & board.bitboards[7]) {
            return 1;
        }
        //Check for bishop attacks
        if (solo_bishop_moves(bb, ally_pieces, all_pieces) & (board.bitboards[8] | board.bitboards[10])) {
            return 1;
        }
        //Check for rook attacks
        if (solo_rook_moves(bb, ally_pieces, all_pieces) & (board.bitboards[9] | board.bitboards[10])) {
            return 1;
        }
        //Check for other king attacks
        if (solo_king_moves(bb, ally_pieces) & board.bitboards[11]) {
            return 1;
        }
        //Check for  pawn attacks
        if (solo_pawn_checks(bb, board.bitboards[6], color)) {
            return 1;
        }

    }
    else {
        enemy_pieces = board.bitboards[0] | board.bitboards[1] | board.bitboards[2] | board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
        ally_pieces = board.bitboards[6] | board.bitboards[7] | board.bitboards[8] | board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
        all_pieces = ally_pieces | enemy_pieces;

        //Check for knight attacks
        if (solo_knight_moves(bb, ally_pieces) & board.bitboards[1]) {
            return 1;
        }
        //Check for bishop attacks
        if (solo_bishop_moves(bb, ally_pieces, all_pieces) & (board.bitboards[2] | board.bitboards[4])) {
            return 1;
        }
        //Check for rook attacks
        if (solo_rook_moves(bb, ally_pieces, all_pieces) & (board.bitboards[3] | board.bitboards[4])) {
            return 1;
        }
        //Check for other king attacks
        if (solo_king_moves(bb, ally_pieces) & board.bitboards[5]) {
            return 1;
        }
        //Check for  pawn attacks
        if (solo_pawn_checks(bb, board.bitboards[0], color)) {
            return 1;
        }

    }

    //The square is not under attack
    return 0;
}

uint64_t solo_pawn_checks(uint64_t solo_king_bb, uint64_t enemy_pawns, int is_white) {
    int square = LSB(solo_king_bb);
    if (is_white) {
        return w_pawn_attack_collisions[square] & enemy_pawns;
    }
    else {
        return b_pawn_attack_collisions[square] & enemy_pawns;
    }
}

uint64_t solo_pawn_moves(uint64_t solo_pawn_bb, uint64_t all_pieces, int is_white) {
    uint64_t move, second_rank = 0xff00ULL, seventh_rank = 0xff000000000000ULL;
    if (is_white) {
        move = (solo_pawn_bb << 8) & ~all_pieces;
        if (move && (solo_pawn_bb & second_rank)) {
            move |= (move << 8) & ~all_pieces;
        }
    }
    else {
        move = (solo_pawn_bb >> 8) & ~all_pieces;
        if (move && (solo_pawn_bb & seventh_rank)) {
            move |= (move >> 8) & ~all_pieces;
        }
    }
    return move;
}

uint64_t solo_pawn_attacks(uint64_t solo_pawn_bb, uint64_t enemy_pieces, int is_white) {
    int square = LSB(solo_pawn_bb);

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

