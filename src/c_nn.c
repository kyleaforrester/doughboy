
//This flips the board and the colors of all the pieces
//Since white always moves first in our NN
void flip_board(struct Board *board) {
    struct Board original = *board;

    board->bitboards[0] = reverse_bits(original.bitboards[6]);
    board->bitboards[1] = reverse_bits(original.bitboards[7]);
    board->bitboards[2] = reverse_bits(original.bitboards[8]);
    board->bitboards[3] = reverse_bits(original.bitboards[9]);
    board->bitboards[4] = reverse_bits(original.bitboards[10]);
    board->bitboards[5] = reverse_bits(original.bitboards[11]);

    board->bitboards[6] = reverse_bits(original.bitboards[0]);
    board->bitboards[7] = reverse_bits(original.bitboards[1]);
    board->bitboards[8] = reverse_bits(original.bitboards[2]);
    board->bitboards[9] = reverse_bits(original.bitboards[3]);
    board->bitboards[10] = reverse_bits(original.bitboards[4]);
    board->bitboards[11] = reverse_bits(original.bitboards[5]);

    if (original.white_king_castle) {
        board->black_king_castle = 1;
    }
    else {
        board->black_king_castle = 0;
    }

    if (original.white_queen_castle) {
        board->black_queen_castle = 1;
    }
    else {
        board->black_queen_castle = 0;
    }

    if (original.black_king_castle) {
        board->white_king_castle = 1;
    }
    else {
        board->white_king_castle = 0;
    }

    if (original.black_queen_castle) {
        board->white_queen_castle = 1;
    }
    else {
        board->white_queen_castle = 0;
    }

}

void board_to_image(struct Board board, double *image) {
    int i, j, counter = 0;
    
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 64; j++) {
            image[counter] = (board.bitboards[i] >> j) & 1;
            counter++;
        }
    }

}

double evaluate(struct Board board, int recursion) {

    //Make sure our perspective is from white to play
    if (!board.white_moves) {
        flip_board(&board);
    }

}

double fen_fire(char *fen, int **weights_layers_conv, int **weights_layers_fc, int **biases_layers_conv, int **biases_layers_fc, double *weights_conv, double *weights_fc, double *biases_conv, double *biases_fc) {
    struct Board board;
    char **fen_tokens = m_tokenize_input(fen, strlen(fen));

    set_to_fen(&board, fen_tokens[0], fen_tokens[1], fen_tokens[2], fen_tokens[3], fen_tokens[4], fen_tokens[5]);
    
    //Flip the fen if black to move
    if (!board.white_moves) {
        flip_board(&board);
    }

    free_tokenize_input(fen_tokens);

    return fire(board, weights_layers_conv, weights_layers_fc, biases_layers_conv, biases_layers_fc, weights_conv, weights_fc, biases_conv, biases_fc);
}

double fire(struct Board board, int **weights_layers_conv, int **weights_layers_fc, int **biases_layers_conv, int **biases_layers_fc, double *weights_conv, double *weights_fc, double *biases_conv, double *biases_fc) {

    int *layer, i, j;
    //New image dimensions
    int n_i_depth, n_i_width, n_i_height, filter_size;
    int w, h, d, d2;
    int bias_idx = 0, ss_idx;
    double *filter = weights_conv, activation, power;

    //Get the max sized image possible
    image_size = 8*8 * weights_layers_conv[0][3];

    //If less than input image then set to input image size (8*8*12)
    if (image_size < 768) {
        image_size = 768;
    }

    double image[image_size];
    double new_image[image_size];
    double snapshot[image_size];
    
    board_to_image(board, image);

    //Execute the Convolution layers first
    for (layer = *weights_layers_conv; *layer; layer++) {
        n_i_depth = layer[3];

        filter_size = 9 * layer[2];
        counter = 0;
        for (d = 0; d < n_i_depth; d++) {
            for (h = 0; h < n_i_height; h++) {
                for (w = 0; w < n_i_width; w++) {
                    activation = biases_conv[bias_idx];

                    //Populate the previous image's filter snapshot
                    ss_idx = 0;
                    for (d2 = 0; d2 < layer[2]; d2++) {
                        //Southwest
                        if (w > 0 && h > 0) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h-1) + w-1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //South
                        if (h > 0) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h-1) + w];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //Southeast
                        if (w < 7 && h > 0) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h-1) + w+1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //West
                        if (w > 0) {
                            snapshot[ss_idx] = image[64*d2 + 8*h + w-1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //Center
                        snapshot[ss_idx] = image[64*d2 + 8*h + w];
                        ss_idx++;
                        //East
                        if (w < 7) {
                            snapshot[ss_idx] = image[64*d2 + 8*h + w+1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //Northwest
                        if (w > 0 && h < 7) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h+1) + w-1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //North
                        if (h < 7) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h+1) + w];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                        //Northeast
                        if (w < 7 && h < 7) {
                            snapshot[ss_idx] = image[64*d2 + 8*(h+1) + w+1];
                        }
                        else {
                            snapshot[ss_idx] = 0
                        }
                        ss_idx++;
                    }

                    activation += mat_mult(filter, snapshot, filter_size);

                    //Now apply ReLU activation
                    if (activation < 0) {
                        activation = 0;
                    }
                    new_image[64*d + 8*h + w] = activation;
                }
            }
            filter += filter_size;
            bias_idx++;
        }

        //This convolution layer is now complete
        //The new image becomes the current image
        for (i = 0; i < image_size; i++) {
            image[i] = new_image[i];
        }
    }

    //Convolution logic complete, now execute fully connected layers
    //Image should now be a flattened 64-length double array
    //Add castling rights
    //My kingside castle
    image[64] = board.white_king_castle;
    //My queenside castle
    image[65] = board.white_queen_castle;
    //Enemy kingside castle
    image[66] = board.black_king_castle;
    //Enemy queenside castle
    image[67] = board.black_queen_castle;

    bias_idx = 0;
    filter = weights_fc;
    for (layer = *weights_layers_fc; *layer; layer++) {

        for (i = 0; i < layer[1]; i++) {
            activation = biases_fc[bias_idx];
            for (j = 0; j < layer[0]; j++) {
                activation += image[j]*filter[j];
            }

            //Implement ReLU
            //But not for the last layer
            if (activation < 0 && *(layer + 1)) {
                activation = 0;
            }

            new_image[i] = activation;

            filter += layer[0];
            bias_idx++;
        }

        //This FC layer is now complete
        //Update the image
        for (i = 0; i < layer[1]; i++) {
            image[i] = new_image[i];
        }

    }

    //All FC layers are complete now
    //Our output node is the first index of hte image
    //Take a sigmoid output
    power = pow(2.0, image[0]);
    return power / (power + 1);

}

double mat_mult(double *a, double *b, size_t size) {
    int i;
    double total = 0;

    for (i = 0; i < size; i++) {
        total += a[i]*b[i];
    }

    return total;
}
