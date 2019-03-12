/*
Formula to convert from centipawns to 0-1 win percentage eval:
y = (x**2 + 10000)/(x**2 + 20000)

Formula to convert from 0-1 win percentage eval to centipawns:
y = ((20000x - 10000)/(1-x))**(1/2)

Formula to rank a nodes' visit calculation score:
y = (59049**eval)*math.log(sum_visits + 2)/(math.log(visits + 2)

*/

int get_pv(struct Node *r_node, char *buffer, size_t buf_size) {
    int i;
    struct Node *node_itr = r_node, **child_itr;
    struct Node *min_child, *max_child;
    int is_user_white = r_node->board.white_moves;

    //The tree should be populated correctly depending on the user's move
    while (node_itr->child_count > 0) {

        //Output a space except if it is the first entry
        if (node_itr != r_node) {
            strcat(buffer, " ");
        }

        //Iterate through children and find min and max children
        min_child = *(node_itr->children);
        max_child = min_child;
        for (child_itr = node_itr->children; *child_itr; child_itr++) {
            if ((*child_itr)->visits > max_child->visits) {
                max_child = *child_itr;
            }
            if ((*child_itr)->visits < min_child->visits) {
                min_child = *child_itr;
            }
        }

        /*
        //It is my move!
        //Get the Max child node
        if (node_itr->height % 2 == 0) {
            strcat(buffer, max_child->last_move);
            node_itr = max_child;
        }
        //It is opponent's move
        //Get the Min child node
        else {
            strcat(buffer, min_child->last_move);
            node_itr = min_child;
        }
        */
        strcat(buffer, max_child->last_move);
        node_itr = max_child;
    }

    return 0;
}

void *go_worker(void *argument) {
    //The worker manager thread will be the worker with index=0
    //This worker manager will do any printing required
    //This worker manager will decide when to quit searching
    struct Go_Args args = *((struct Go_Args *)argument);
    struct Node *my_node;
    uint64_t start_time = get_nanos(), curr_time;
    uint64_t search_time_nanos;
    //Seed with random number, diff for each thread
    uint64_t prng_state = (start_time*(args.index+1))^(0x8c248aedad57084dULL);
    int curr_depth = 0, eval_display, is_lock_acquired;
    char pv[MAX_BUF_SIZE];
    char **tokens;
    double temp_eval;

    //Manager thread: Estimate how much time to search
    if (args.index == 0) {
        curr_time = get_nanos();

        if (!args.movestogo) {
            //Assume 30 moves remain in game
            args.movestogo = 30;
        }

        //Use 1/movestogo of your banked time, and all your increment
        //Keep 1 second in the bank at all times
        //I don't care about opponent's time management situation
        //I am white
        if (root->board.white_moves) {
            search_time_nanos = (((args.wtime - 10000)/args.movestogo) + args.winc)*1000000L;
        }
        //I am black
        else {
            search_time_nanos = (((args.btime - 10000)/args.movestogo) + args.binc)*1000000L;
        }

        //If movetime was specified, use exactly that amount
        if (args.movetime) {
            search_time_nanos = args.movetime * 1000000L;
        }
            
    }

    //Every thread keeps searching while pondering is on
    while (stop_pondering == 0) {
        //Balance exploration/exploitation to traverse randomly in the game tree
        //Stop traversing when you have a leaf node locked
        is_lock_acquired = 0;
        while (!is_lock_acquired && stop_pondering == 0) {
            //Managerial duties for the manager thread = 0
            if (args.index == 0) {
                curr_time = get_nanos();
                /*
                Giving all possible reasons to stop searching:
                1) Calculated search time is expired
                2) Depth is exhausted
                3) Nodes is exhausted
                */
                if (!args.infinite && !args.ponder && ((curr_time - start_time > search_time_nanos && ((args.wtime && args.btime) || args.movetime)) || (root->depth >= args.depth && args.depth) || (root->visits >= args.nodes && args.nodes))) {
                    //Stop all searching
                    stop_pondering = 1;
                }
            }
            //Start search from root
            my_node = root;

            //Keep searching until our node has no children
            while (my_node->child_count > 0) {
                //Navigate to a worthy child by considering variables:
                //1) Total nodes visited
                //2) Each child eval
                //3) Each child visits
                my_node = select_child_nav(my_node, &prng_state);
            }
            //Lock the leaf node
            pthread_mutex_lock(&(my_node->mutex));

            //If we have no children now, then we can stop searching
            if (!my_node->children && !my_node->is_checkmate) {
                is_lock_acquired = 1;
            }
            else {
                //Someone got to our leaf node first.  Restart search.
                //Release lock
                pthread_mutex_unlock(&(my_node->mutex));
            }
        }

        if (is_lock_acquired) {
            //Bloom the leaf node
            m_bloom_node(my_node, &evaluate);

            //Push the new nodes' values from the parents all the way to root
            collapse_values(my_node);

            //Release lock
            pthread_mutex_unlock(&(my_node->mutex));
        }

        //Managerial duties for the manager thread = 0
        if (args.index == 0) {
            //Print out depth information if new depth is reached
            if (root->depth > curr_depth) {
                curr_time = get_nanos();
                curr_depth = root->depth;
                get_pv(root, pv, sizeof(pv));
                //Display cp version of eval
                //Remember eval is a double between 0 and 1
                if (root->eval > 0.5) {
                    //We are winning
                    temp_eval = sqrt((20000*root->eval - 10000)/(1 - root->eval));
                    //Round to nearest int
                    eval_display = (int) (temp_eval + 0.5);
                }
                else if (root->eval < 0.5) {
                    //We are losing
                    temp_eval = 1 - root->eval;
                    temp_eval = sqrt((20000*temp_eval - 10000)/(1 - temp_eval));
                    //Round to nearest int
                    eval_display = (int) (-temp_eval - 0.5);
                }
                else {
                    eval_display = 0;
                }
                printf("info depth %d seldepth %d multipv %d score cp %d nodes %d nps %d tbhits %d time %d pv %s\n", curr_depth, curr_depth, 1, eval_display, root->visits, (int)(root->visits / (double)((double)(1+curr_time-start_time)/1000000000L)), 0, (curr_time-start_time)/(1000000L), pv);
                fflush(stdout);
                pv[0] = 0;
            }
        }
    }

    //Manager thread prints information back to GUI
    if (args.index == 0) {
        get_pv(root, pv, sizeof(pv));
        tokens = m_tokenize_input(pv, strlen(pv));
        if (args.ponder_enabled) {
            printf("bestmove %s ponder %s\n", tokens[0], tokens[1]);
        }
        else {
            printf("bestmove %s\n", tokens[0]);
        }
        free_tokenize_input(tokens);
        pv[0] = 0;
    }
    fflush(stdout);
}

struct Node *select_child_nav(struct Node *parent, uint64_t *prng_state) {
    double soft_max_sum = 0, iteration_chance;
    struct Node *child_iter;
    double soft_max_scores[MAX_CHILDREN];
    int i, child_found = 0, my_move;

    my_move = 1 - parent->height % 2;
    //Calculate softmax for each child and store sum
    for (i = 0; i < parent->child_count; i++) {
        child_iter = parent->children[i];
        //soft_max_scores[i] = pow(59049, child_iter->eval) * log(parent->visits + 2)/log(child_iter->visits + 2);
        //It is my move!
        if (my_move) {
            soft_max_scores[i] = pow(4, 10 * child_iter->eval);
        }
        else {
            //Opponent's move!
            soft_max_scores[i] = pow(4, 10 * (1 - child_iter->eval));
        }
        //soft_max_scores[i] *= soft_max_scores[i];
        soft_max_sum += soft_max_scores[i];
    }

    //Iterate through moves until a child's percentage is hit
    iteration_chance = 1;
    for (i = 0; i < parent->child_count - 1; i++) {
        if ((double)spcg32(prng_state) / (double)4294967295 <= soft_max_scores[i] / (soft_max_sum * iteration_chance)) {
            //Child found!
            return parent->children[i];
        }
        iteration_chance *= 1 - (soft_max_scores[i] / (soft_max_sum * iteration_chance));
    }

    //No child was found so far.  Therefore we send the last child!
    return parent->children[parent->child_count - 1];
}

void collapse_values(struct Node *my_node) {
    int i, visit_sum = 0;
    double min_eval = 1, max_eval = 0;
    struct Node *child;

    //Update with child information
    for (i = 0; i < my_node->child_count; i++) {
        child = my_node->children[i];
        //Gathering visitation information
        visit_sum += child->visits;
        //Setting depth for info and search purposes
        if (child->depth >= my_node->depth) {
            my_node->depth = child->depth + 1;
        }
        //Find min_eval
        if (child->eval > max_eval) {
            max_eval = child->eval;
        }
        //Find max_eval
        if (child->eval < min_eval) {
            min_eval = child->eval;
        }
    }

    //Update Visits
    my_node->visits = visit_sum;

    //Update Eval
    if (my_node->height % 2 == 0) {
        //It is my move!
        my_node->eval = max_eval;
    }
    else {
        //It is opponents move
        my_node->eval = min_eval;
    }

    //Perform for each parent
    if (my_node->parent) {
        collapse_values(my_node->parent);
    }

}

void set_root_node(int ponder) {

}

void spawn_go_workers(int ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int movetime, int infinite) {

    int result_code, thread_count = 1, i, ponder_enabled = 0;
    struct Option **option_itr;

    //Check engine options
    for (option_itr = options; *option_itr; option_itr++) {
        if (strcmp((*option_itr)->name, "Threads") == 0) {
            thread_count = (*option_itr)->spin;
        }
        else if (strcmp((*option_itr)->name, "Ponder") == 0) {
            ponder_enabled = (*option_itr)->check;
        }
    }

    //If ponder is disabled and GUI wants to ponder, exit go command
    if (!ponder_enabled && ponder) {
        return;
    }

    //Make sure we are searching with as much pre-computed tree as possible
    set_root_node(ponder);

    //Allow worker threads to run
    stop_pondering = 0;

    //Populate thread args including the index.
    //Thread with index 0 is the 'reporter/manager' thread.
    struct Go_Args *thread_args = malloc(sizeof(struct Go_Args) * thread_count);
    for (i = 0; i < thread_count; i++) {
        thread_args[i].ponder = ponder;
        thread_args[i].ponder_enabled = ponder_enabled;
        thread_args[i].wtime = wtime;
        thread_args[i].btime = btime;
        thread_args[i].winc = winc;
        thread_args[i].binc = binc;
        thread_args[i].movestogo = movestogo;
        thread_args[i].depth = depth;
        thread_args[i].nodes = nodes;
        thread_args[i].movetime = movetime;
        thread_args[i].infinite = infinite;
        thread_args[i].index = i;
    }

    pthread_t threads[thread_count];

    //Spawn the worker threads
    for (i = 0; i < thread_count; i++) {
        result_code = pthread_create(&threads[i], NULL, go_worker, &thread_args[i]);
        assert(!result_code);

        result_code = pthread_detach(threads[i]);
        assert(!result_code);
    }
}
