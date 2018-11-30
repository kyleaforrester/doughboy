
void do_move(struct Board *board, char *move) {
    int bb, from = -1, to = -1, length = strlen(move);
    uint64_t from_bb, to_bb;

    //Move command invalid length
    if (length < 4 || length > 5) {
        return;
    }

    //Create indexes of source and destination
    from = (move[0] - 'a') + 8 * (move[1] - '1');
    to = (move[2] - 'a') + 8 * (move[3] - '1');

    //Source or destination not valid square
    if (from < 0 || from > 63 || to < 0 || to > 63) {
        return;
    }

    //Create bitboards of source and destination
    from_bb = 0x1 << from;
    to_bb = 0x1 << to;

    //Find bitboard with piece at source
    for (bb = 0; bb < 12 && !(board->bitboards[bb] & from_bb); bb++);

    //Bitboard not found for that from square
    if (bb >= 12) {
        return;
    }

    //Check if it is a promotion
    if (length == 5 && ((bb == 0 && from >= 48) || (bb == 6 && from <= 15))) {

    }

    //Check if it is a castle
    if (bb 

}

void set_to_fen(struct Board *board, char *piece_placement, char *color, char *castling, char *en_passent, char *halfmove, char *fullmove) {
    set_piece_placement(board, piece_placement);
    set_color(board, color);
    set_castling(board, castling);
    set_en_passent(board, en_passent);
    set_halfmove(board, halfmove);
    set_fullmove(board, fullmove);
}

void set_piece_placement(struct Board *board, char *piece_placement) {
    int i, x, y, skip;

    for (i = 0; i < 12; i++) {
        board->bitboards[i] = 0;
    }

    for (i = 0, x = 0, y = 7; x < 8 && y >= 0 && piece_placement[i]; i++) {
        if (piece_placement[i] == 'P') {
            board->bitboards[0] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'N') {
            board->bitboard[1] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'B') {
            board->bitboard[2] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'R') {
            board->bitboard[3] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'Q') {
            board->bitboard[4] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'K') {
            board->bitboard[5] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'p') {
            board->bitboard[6] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'n') {
            board->bitboard[7] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'b') {
            board->bitboard[8] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'r') {
            board->bitboard[9] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'q') {
            board->bitboard[10] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'k') {
            board->bitboard[11] += 0x1 << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == '/') {
            y--;
            x = 0;
        }
        else if ((skip = piece_placement[i] - '0') >= 1 && skip < 10) {
            if (skip < 8) {
                x += skip;
            }
            else {
                x = 0;
            }
        }
    }
}

void set_color(struct Board *board, char *color) {
    int length = strlen(color);
    if (length != 1) {
        return;
    }

    if (strcmp(color, "w") == 0) {
        board->white_moves = 1;
    }
    else if (strcmp(color, "b") == 0) {
        board->white_moves = 0;
    }
}

void set_castling(struct Board *board, char *castling) {
    int i, length = strlen(color);
    if (length > 4) {
        return;
    }

    board->white_king_castle = 0;
    board->white_queen_castle = 0;
    board->black_king_castle = 0;
    board->black_queen_castle = 0;

    for (i = 0; i < length; i++) {
        if (castling[i] == 'K') {
            board->white_king_castle = 1;
        }
        else if (castling[i] == 'Q') {
            board->white_queen_castle = 1;
        }
        else if (castling[i] == 'k') {
            board->black_king_castle = 1;
        }
        else if (castling[i] == 'q') {
            board->black_queen_castle = 1;
        }
    }
}

void set_en_passent(struct Board *board, char *en_passent) {
    int x, y;
    int length = strlen(color);
    if (length != 2) {
        return;
    }

    x = en_passent[0] - 'a';
    y = en_passent[1] - '1';

    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return;
    }

    board->en_passent = 0x1 << (x + 8*y);
}

void set_halfmove(struct Board *board, char *halfmove) {
    board->halfmove_clock = atoi(halfmove);
}

void set_fullmove(struct Board *board, char *fullmove) {
    board->fullmove_clock = atoi(fullmove);
}

void fill_char_board_bb(char *board, size_t board_size, uint64_t bb, char piece) {
    int i, num_bits = bit_count(bb);
    uint64_t temp_bb = bb;

    if (board_size != 64) {
        return;
    }

    for (i = 0; temp_bb && i < num_bits; i++) {
        board[lsb_index(temp_bb)] = piece;
        //Remove the LSB
        temp_bb &= temp_bb - 1;
    }
}


void fill_char_board(char *board, size_t board_size) {
    int i;

    if (board_size != 64) {
        return;
    }

    //Populate empty squares
    for (i = 0; i < 64; i++) {
        board[i] = '-';
    }

    //Populate white pawns
    fill_char_board_bb(board, board_size, curr_board->bitboards[0], 'P');
    //Populate white knights 
    fill_char_board_bb(board, board_size, curr_board->bitboards[1], 'N');
    //Populate white bishops
    fill_char_board_bb(board, board_size, curr_board->bitboards[2], 'B');
    //Populate white rooks
    fill_char_board_bb(board, board_size, curr_board->bitboards[3], 'R');
    //Populate white queens
    fill_char_board_bb(board, board_size, curr_board->bitboards[4], 'Q');
    //Populate white king
    fill_char_board_bb(board, board_size, curr_board->bitboards[5], 'K');

    //Populate black pawns
    fill_char_board_bb(board, board_size, curr_board->bitboards[6], 'p');
    //Populate black knights 
    fill_char_board_bb(board, board_size, curr_board->bitboards[7], 'n');
    //Populate black bishops
    fill_char_board_bb(board, board_size, curr_board->bitboards[8], 'b');
    //Populate black rooks
    fill_char_board_bb(board, board_size, curr_board->bitboards[9], 'r');
    //Populate black queens
    fill_char_board_bb(board, board_size, curr_board->bitboards[10], 'q');
    //Populate black king
    fill_char_board_bb(board, board_size, curr_board->bitboards[11], 'k');
}
