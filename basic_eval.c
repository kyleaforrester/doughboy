
double white_pawn_evals[64] = {0.77,0.96,0.96,0.96,0.96,0.96,0.96,0.77,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,0.83,1.04,1.04,1.04,1.04,1.04,1.04,0.83,0.86,1.08,1.08,1.08,1.08,1.08,1.08,0.86,0.89,1.12,1.12,1.12,1.12,1.12,1.12,0.89,0.93,1.16,1.16,1.16,1.16,1.16,1.16,0.93,1.96,2.20,2.20,2.20,2.20,2.20,2.20,1.96,1.00,1.24,1.24,1.24,1.24,1.24,1.24,1.00};

double black_pawn_evals[64] = {1.00,1.24,1.24,1.24,1.24,1.24,1.24,1.00,1.96,2.20,2.20,2.20,2.20,2.20,2.20,1.96,0.93,1.16,1.16,1.16,1.16,1.16,1.16,0.93,0.89,1.12,1.12,1.12,1.12,1.12,1.12,0.89,0.86,1.08,1.08,1.08,1.08,1.08,1.08,0.86,0.83,1.04,1.04,1.04,1.04,1.04,1.04,0.83,0.80,1.00,1.00,1.00,1.00,1.00,1.00,0.80,0.77,0.96,0.96,0.96,0.96,0.96,0.96,0.77};

double mid_king_evals[64] = {4.00,3.70,2.30,2.00,2.00,2.30,3.70,4.00,3.70,3.40,2.00,1.70,1.70,2.00,3.40,3.70,2.30,2.00,0.60,0.30,0.30,0.60,2.00,2.30,2.00,1.70,0.30,0.00,0.00,0.30,1.70,2.00,2.00,1.70,0.30,0.00,0.00,0.30,1.70,2.00,2.30,2.00,0.60,0.30,0.30,0.60,2.00,2.30,3.70,3.40,2.00,1.70,1.70,2.00,3.40,3.70,4.00,3.70,2.30,2.00,2.00,2.30,3.70,4.00};

double end_king_evals[64] = {0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00,0.00,0.10,0.30,0.40,0.40,0.30,0.10,0.00};

double dummy_eval(struct Board board) {
    return 0.5;
}

double knight_score(uint64_t knights, uint64_t allies, uint64_t enemies) {
    uint64_t lsb_knight, moves;
    double score = 0;

    //Loop for each knight
    for (; knights; knights &= knights - 1) {
        lsb_knight = knights & (~knights + 1);
        //Pretend the knight has no allies for this mobility score
        moves = solo_knight_moves(lsb_knight, 0);

        score += bit_count(moves);
    }

    return score;
}

double bishop_score(uint64_t bishops, uint64_t allies, uint64_t enemies) {
    uint64_t lsb_bishop, moves;
    double score = 0;

    //Loop for each bishop
    for (; bishops; bishops &= bishops - 1) {
        lsb_bishop = bishops & (~bishops + 1);
        //Pretend the bishop has no allies for this mobility score
        moves = solo_bishop_moves(lsb_bishop, 0, allies | enemies);

        score += bit_count(moves);
    }

    return score;

}

double rook_score(uint64_t rooks, uint64_t allies, uint64_t enemies) {
    uint64_t lsb_rook, moves;
    double score = 0;

    //Loop for each rook
    for (; rooks; rooks &= rooks - 1) {
        lsb_rook = rooks & (~rooks + 1);
        //Pretend the rook has no allies for this mobility score
        moves = solo_rook_moves(lsb_rook, 0, allies | enemies);

        score += bit_count(moves);
    }

    return score;

}

double queen_score(uint64_t queens, uint64_t allies, uint64_t enemies) {
    uint64_t lsb_queen, moves;
    double score = 0;

    //Loop for each queen
    for (; queens; queens &= queens - 1) {
        lsb_queen = queens & (~queens + 1);
        //Pretend the queen has no allies for this mobility score
        moves = solo_bishop_moves(lsb_queen, 0, allies | enemies);
        moves |= solo_rook_moves(lsb_queen, 0, allies | enemies);

        score += bit_count(moves);
    }

    return score;

}

double mobility_score(struct Board board, int color) {
    double score = 0;
    uint64_t bishops, rooks, knights, king, queens;
    uint64_t all_pieces, ally_pieces, enemy_pieces;

    if (color) {
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

    score += 3*knight_score(knights, ally_pieces, enemy_pieces);
    score += 2*bishop_score(bishops, ally_pieces, enemy_pieces);
    score += rook_score(rooks, ally_pieces, enemy_pieces);
    score += 0.2*queen_score(rooks, ally_pieces, enemy_pieces);

    return score;
}

double mid_game_king_eval(struct Board board, int color) {
    double score;

    if (color) {
        //Evaluate for White
        score = mid_king_evals[LSB(board.bitboards[5])];
    }
    else {
        //Evaluate for Black
        score = mid_king_evals[LSB(board.bitboards[11])];
    }

    return score;
}

double end_game_king_eval(struct Board board, int color) {
    double score;

    if (color) {
        //Evaluate for White
        score = end_king_evals[LSB(board.bitboards[5])];
    }
    else {
        //Evaluate for Black
        score = end_king_evals[LSB(board.bitboards[11])];
    }

    return score;

}

double mid_game_eval(struct Board board) {
    //Count the basic material of both sides
    //Queen = 9
    //Rook = 5
    //Bishop = 3
    //Knight = 3
    //Pawn = 1, but ramping up as move along the board
    double my_score = 0, opponent_score = 0, total_score;
    int my_color, opponent_color, i;

    if (root->board.white_moves) {
        my_color = 1;   
        opponent_color = 0;
    }
    else {
        my_color = 0;
        opponent_color = 1;
    }

    my_score += mobility_score(board, my_color) / 40;
    opponent_score += mobility_score(board, opponent_color) / 40;

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

    my_score += mid_game_king_eval(board, my_color);
    opponent_score += mid_game_king_eval(board, my_color);

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

double end_game_eval(struct Board board) {
    return mid_game_eval(board);
}

double game_phase(struct Board board) {
    return 0.5;
}

double evaluate(struct Board board, int recursion) {

    //We are done recursing
    if (recursion == 0) {
        return mid_game_eval(board);
    }
    // We are still recursing!
    else {
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
        m_bloom_node(&my_node, recursion - 1);

        if (root->board.white_moves == board.white_moves) {
            my_move = 1;
        }
        else {
            my_move = 0;
        }

        if (my_node.is_checkmate) {
            if (my_move) {
                return 0.01;
            }
            else {
                return 0.99;
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
