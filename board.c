
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
