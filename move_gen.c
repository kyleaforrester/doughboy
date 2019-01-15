


int bloom_node(struct Node *node) {
    
    struct Board *board = node->board;
    uint64_t bishops, rooks, knights, king, queens;
    uint64_t all_pieces, ally_pieces, enemy_pieces;

    if (node->board.white_moves) {
        knights = board->bitboards[1];
        bishops = board->bitboards[2];
        rooks = board->bitboards[3];
        queens = board->bitboards[4];
        king = board->bitboards[5];

        ally_pieces = board->bitboards[0] | knights | bishops | rooks | queens | king;
        enemy_pieces = board->bitboards[6] | board->bitboards[7] | board->bitboards[8] | board->bitboards[9] | board->bitboards[10] | board->bitboards[11];
        all_pieces = ally_pieces | enemy_pieces;
    }
    else {
        knights = board->bitboards[7];
        bishops = board->bitboards[8];
        rooks = board->bitboards[9];
        queens = board->bitboards[10];
        king = board->bitboards[11];

        ally_pieces = board->bitboards[6] | knights | bishops | rooks | queens | king;
        enemy_pieces = board->bitboards[0] | board->bitboards[1] | board->bitboards[2] | board->bitboards[3] | board->bitboards[4] | board->bitboards[5];
        all_pieces = ally_pieces | enemy_pieces;
    }
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

