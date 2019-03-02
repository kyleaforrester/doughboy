
double white_pawn_evals[64] = {0.74,0.92,0.92,0.92,0.92,0.92,0.92,0.74,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,0.87,1.08,1.08,1.08,1.08,1.08,1.08,0.87,0.94,1.18,1.18,1.18,1.18,1.18,1.18,0.94,1.02,1.28,1.28,1.28,1.28,1.28,1.28,1.02,1.11,1.38,1.38,1.38,1.38,1.38,1.38,1.11,2.20,2.50,2.50,2.50,2.50,2.50,2.50,2.20,1.30,1.63,1.63,1.63,1.63,1.63,1.63,1.30};

double black_pawn_evals[64] = {1.30,1.63,1.63,1.63,1.63,1.63,1.63,1.30,2.20,2.50,2.50,2.50,2.50,2.50,2.50,2.20,1.11,1.38,1.38,1.38,1.38,1.38,1.38,1.11,1.02,1.28,1.28,1.28,1.28,1.28,1.28,1.02,0.94,1.18,1.18,1.18,1.18,1.18,1.18,0.94,0.87,1.08,1.08,1.08,1.08,1.08,1.08,0.87,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,0.74,0.92,0.92,0.92,0.92,0.92,0.92,0.74};

double evaluate(struct Board board) {
    struct Node my_node, *min_child, *max_child, **child_itr;
    int my_move, i;
    double ret_val;

    my_node.board = board;
    my_node.visits = 0;
    my_node.depth = 0;
    my_node.height = 0;
    my_node.eval = 0.5;
    my_node.is_checkmate = 0;
    my_node.is_stalemate = 0;
    my_node.children = NULL;
    my_node.child_count = 0;
    my_node.parent = NULL;
    m_bloom_node(&my_node, &evaluate_board);

    if (root->board.white_moves == board.white_moves) {
        my_move = 1;
    }
    else {
        my_move = 0;
    }

    if (my_node.is_checkmate) {
        if (my_move) {
            return 0;
        }
        else {
            return 1;
        }
    }

    if (my_node.is_stalemate) {
        return 0.5;
    }

    //Iterate through children and find min and max children
    min_child = *(my_node.children);
    max_child = min_child;
    for (child_itr = my_node.children; *child_itr; child_itr++) {
        if ((*child_itr)->eval > max_child->eval) {
            max_child = *child_itr;
        }
        if ((*child_itr)->eval < min_child->eval) {
            min_child = *child_itr;
        }
    }

    if (my_move) {
        ret_val = max_child->eval;
    }
    else {
        ret_val = min_child->eval;
    }

    for (i = 0; i < my_node.child_count; i++) {
        free_node(my_node.children[i]);
    }
    free(my_node.children);
    
    return ret_val;
}

double evaluate_board(struct Board board) {
    //Count the basic material of both sides
    //Queen = 9
    //Rook = 5
    //Bishop = 3
    //Knight = 3
    //Pawn = 1, but ramping up as move along the board
    double my_score = 0, opponent_score = 0, total_score;
    int my_color, opponent_color;

    if (board.halfmove_clock == 100) {
        return 0.5;
    }

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
    
    total_score = (my_score - opponent_score)*100;

    //Convert centipawn score to 0-1 win percentage
    if (total_score > 0) {
        return (total_score*total_score + 10000)/(total_score*total_score + 20000);
    }
    else if (total_score < 0) {
        return 1 - (total_score*total_score + 10000)/(total_score*total_score + 20000);
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
