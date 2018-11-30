
void clear_bitboards(struct Board *board, uint64_t bb_clear) {
    int i;
    for (i = 0; i < 12; i++) {
        board->bitboards[i] &= ~bb_clear;
    }
}

void do_move(struct Board *board, char *move) {
    int bb, from = -1, to = -1, length = strlen(move);
    uint64_t from_bb, to_bb, zero_bb = 1;

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
    from_bb = zero_bb << from;
    to_bb = zero_bb << to;

    //Find bitboard with piece at source
    for (bb = 0; bb < 12 && !(board->bitboards[bb] & from_bb); bb++);

    //Bitboard not found for that from square
    if (bb >= 12) {
        return;
    }

    //Check if it is a promotion
    if (length == 5 && ((bb == 0 && from >= 48) || (bb == 6 && from <= 15))) {
        //Delete the source square
        board->bitboards[bb] &= ~from_bb;

        //Delete any pieces at destination square
        clear_bitboards(board, to_bb);

        //White pawn
        if (bb == 0) {
            //Add the new piece to appropriate destination bitboard
            if (move[4] == 'n') {
                board->bitboards[1] |= to_bb;
            }
            else if (move[4] == 'b') {
                board->bitboards[2] |= to_bb;
            }
            else if (move[4] == 'r') {
                board->bitboards[3] |= to_bb;
            }
            else if (move[4] == 'q') {
                board->bitboards[4] |= to_bb;
            }
        }
        //Black pawn
        else if (bb == 6) {
            //Add the new piece to appropriate destination bitboard
            if (move[4] == 'n') {
                board->bitboards[7] |= to_bb;
            }
            else if (move[4] == 'b') {
                board->bitboards[8] |= to_bb;
            }
            else if (move[4] == 'r') {
                board->bitboards[9] |= to_bb;
            }
            else if (move[4] == 'q') {
                board->bitboards[10] |= to_bb;
            }
        }
        board->en_passent = 0;
    }
    //Check if it is a castle
    else if ((bb == 5 && from == 4 && (to == 6 || to == 2)) || (bb == 11 && from == 60 && (to == 62 || to == 58))) {
        //Clear source and destination squares
        board->bitboards[bb] &= ~from_bb;
        clear_bitboards(board, to_bb);

        //Move King
        board->bitboards[bb] |= to_bb;

        //White King
        if (bb == 5) {
            //Clear and move rook squares
            //Kingside
            if (to == 6) {
                board->bitboards[3] &= ~0x80;
                board->bitboards[3] |= 0x20;
                board->white_king_castle = 0;
            }
            //Queenside
            else if (to == 2) {
                board->bitboards[3] &= ~0x1;
                board->bitboards[3] |= 0x8;
                board->white_queen_castle = 0;
            }
        }
        //Black King
        else if (bb == 11) {
            //Clear and move rook squares
            //Kingside
            if (to == 62) {
                board->bitboards[9] &= ~0x8000000000000000;
                board->bitboards[9] |= 0x2000000000000000;
                board->black_king_castle = 0;
            }
            //Queenside
            else if (to == 58) {
                board->bitboards[9] &= ~0x100000000000000;
                board->bitboards[9] |= 0x800000000000000;
                board->black_queen_castle = 0;
            }
        }
        board->en_passent = 0;
    }
    //Check if it is en passent
    else if (board->en_passent & to_bb && (bb == 0 || bb == 6)) {
        //Move pawn
        board->bitboards[bb] &= ~from_bb;
        clear_bitboards(board, to_bb);
        board->bitboards[bb] |= to_bb;
        //White pawn captures black
        if (bb == 0) {
            board->bitboards[6] &= ~(to_bb >> 8);
        }
        //Black pawn capture white
        else if (bb == 6) {
            board->bitboards[0] &= ~(to_bb << 8);
        }
        board->en_passent = 0;
    }
    //Pawn moves up twice
    else if ((bb == 0 && from >= 8 && from <= 15 && to >= 24 && to <= 31) || (bb == 6 && from >= 48 && from <= 55 && to >= 32 && to <= 39)) {
        //Move pawn, no captures made though
        board->bitboards[bb] &= ~from_bb;
        board->bitboards[bb] |= to_bb;

        //White pawn
        if (bb == 0) {
            board->en_passent = to_bb >> 8;
        }
        //Black pawn
        else if (bb == 6) {
            board->en_passent = to_bb << 8;
        }
    }
    //It is a regular move
    else {
        board->bitboards[bb] &= ~from_bb;
        clear_bitboards(board, to_bb);
        board->bitboards[bb] |= to_bb;
        board->en_passent = 0;
    }
}

void set_piece_placement(struct Board *board, char *piece_placement) {
    int i, x, y, skip;
    uint64_t zero_bb = 1;

    for (i = 0; i < 12; i++) {
        board->bitboards[i] = 0;
    }

    for (i = 0, x = 0, y = 7; x < 9 && y >= 0 && piece_placement[i]; i++) {
        if (piece_placement[i] == 'P') {
            board->bitboards[0] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'N') {
            board->bitboards[1] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'B') {
            board->bitboards[2] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'R') {
            board->bitboards[3] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'Q') {
            board->bitboards[4] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'K') {
            board->bitboards[5] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'p') {
            board->bitboards[6] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'n') {
            board->bitboards[7] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'b') {
            board->bitboards[8] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'r') {
            board->bitboards[9] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'q') {
            board->bitboards[10] |= zero_bb << (x+8*y);
            x++;
        }
        else if (piece_placement[i] == 'k') {
            board->bitboards[11] |= zero_bb << (x+8*y);
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

    if (color[0] == 'w') {
        board->white_moves = 1;
    }
    else if (color[0] == 'b') {
        board->white_moves = 0;
    }
}

void set_castling(struct Board *board, char *castling) {
    int i, length = strlen(castling);
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
    int x, y, length = strlen(en_passent);
    uint64_t zero_bb = 1;
    if (length != 2) {
        return;
    }

    x = en_passent[0] - 'a';
    y = en_passent[1] - '1';

    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return;
    }

    board->en_passent = zero_bb << (x + 8*y);
}

void set_halfmove(struct Board *board, char *halfmove) {
    board->halfmove_clock = atoi(halfmove);
}

void set_fullmove(struct Board *board, char *fullmove) {
    board->fullmove_clock = atoi(fullmove);
}

void set_to_fen(struct Board *board, char *piece_placement, char *color, char *castling, char *en_passent, char *halfmove, char *fullmove) {
    set_piece_placement(board, piece_placement);
    set_color(board, color);
    set_castling(board, castling);
    set_en_passent(board, en_passent);
    set_halfmove(board, halfmove);
    set_fullmove(board, fullmove);
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
