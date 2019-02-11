
double white_pawn_evals = {0.64,0.80,0.80,0.80,0.80,0.80,0.80,0.64,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,1.00,1.25,1.25,1.25,1.25,1.25,1.25,1.00,1.24,1.55,1.55,1.55,1.55,1.55,1.55,1.24,1.55,1.93,1.93,1.93,1.93,1.93,1.93,1.55,1.93,2.41,2.41,2.41,2.41,2.41,2.41,1.93,2.40,3.00,3.00,3.00,3.00,3.00,3.00,2.40,2.99,3.74,3.74,3.74,3.74,3.74,3.74,2.99};

double black_pawn_evals = {2.99,3.74,3.74,3.74,3.74,3.74,3.74,2.99,2.40,3.00,3.00,3.00,3.00,3.00,3.00,2.40,1.93,2.41,2.41,2.41,2.41,2.41,2.41,1.93,1.55,1.93,1.93,1.93,1.93,1.93,1.93,1.55,1.24,1.55,1.55,1.55,1.55,1.55,1.55,1.24,1.00,1.25,1.25,1.25,1.25,1.25,1.25,1.00,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,0.64,0.80,0.80,0.80,0.80,0.80,0.80,0.64};

double evaluate(struct Board board) {
    //Count the basic material of both sides
    //Queen = 9
    //Rook = 5
    //Bishop = 3
    //Knight = 3
    //Pawn = 1, but ramping up as move along the board
    double my_score = 0, opponent_score = 0, total_score;
    int my_color, opponent_color;

    if (root->board.white_moves) {
        my_color = 1;   
        opponent_color = 0;
    }
    else {
        my_color = 0;
        opponent_color = 1;
    }

    my_score += evaluate_pawns(board, my_color);
    my_score += evaluate_knights(board, my_color);
    my_score += evaluate_bishops(board, my_color);
    my_score += evaluate_rooks(board, my_color);
    my_score += evaluate_queens(board, my_color);

    opponent_score += evaluate_pawns(board, opponent_color);
    opponent_score += evaluate_knights(board, opponent_color);
    opponent_score += evaluate_bishops(board, opponent_color);
    opponent_score += evaluate_rooks(board, opponent_color);
    opponent_score += evaluate_queens(board, opponent_color);
    
    total_score = my_score - opponent_score;

    //Convert centipawn score to 0-1 win percentage
    if (total_score > 0) {
        return (total_score*total_score + 10000)/(total_score*total_score + 20000);
    }
    else if (total_score < 0) {
        return 1 - (total_score*total_score + 10000)/(total_score*total_score + 20000)
    }
    else {
        return 0.5;
    }

}

double evaluate_pawns(struct Board board, int color) {
    uint64_t pawns, lsb_pawn;
    double score = 0;
    int index;

    if (color) {
        //Evaluate for White
        for (pawns = board.bitboards[0]; pawns; pawns &= pawns - 1) {
            lsb_pawn = pawns & (~pawns + 1);
            score += white_pawn_evals[LSB(lsb_pawn)];
        }
    }
    else {
        //Evaluate for Black
        for (pawns = board.bitboards[6]; pawns; pawns &= pawns - 1) {
            lsb_pawn = pawns & (~pawns + 1);
            score += black_pawn_evals[LSB(lsb_pawn)];
        }
    }

    return score;
}

double evaluate_knights(struct Board board, int color) {
    uint64_t knights;
    double score = 0;
    int index;

    if (color) {
        //Evaluate for White
        for (knights = board.bitboards[1]; knights; knights &= knights - 1) {
            score += 3;
        }
    }
    else {
        //Evaluate for Black
        for (knights = board.bitboards[7]; knights; knights &= knights - 1) {
            score += 3;
        }
    }

    return score;
}

double evaluate_bishops(struct Board board, int color) {
    uint64_t bishops;
    double score = 0;
    int index;

    if (color) {
        //Evaluate for White
        for (bishops = board.bitboards[2]; bishops; bishops &= bishops - 1) {
            score += 3;
        }
    }
    else {
        //Evaluate for Black
        for (bishops = board.bitboards[8]; bishops; bishops &= bishops - 1) {
            score += 3;
        }
    }

    return score;
}

double evaluate_rooks(struct Board board, int color) {
    uint64_t rooks;
    double score = 0;
    int index;

    if (color) {
        //Evaluate for White
        for (rooks = board.bitboards[3]; rooks; rooks &= rooks - 1) {
            score += 5;
        }
    }
    else {
        //Evaluate for Black
        for (rooks = board.bitboards[9]; rooks; rooks &= rooks - 1) {
            score += 5;
        }
    }

    return score;
}

double evaluate_queens(struct Board board, int color) {
    uint64_t queens;
    double score = 0;
    int index;

    if (color) {
        //Evaluate for White
        for (queens = board.bitboards[4]; queens; queens &= queens - 1) {
            score += 9;
        }
    }
    else {
        //Evaluate for Black
        for (queens = board.bitboards[10]; queens; queens &= queens - 1) {
            score += 9;
        }
    }

    return score;
}
