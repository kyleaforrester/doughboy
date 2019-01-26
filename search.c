
//This struct is used so I can pass multiple arguments to a thread.
//pthread only allows a single address to pass to a new thread.
struct Go_Args {
    int ponder;
    int ponder_enabled;
    int wtime;
    int btime;
    int winc;
    int binc;
    int movestogo;
    int depth;
    int nodes;
    int movetime;
    int infinite;
    int index;
};

int get_pv(struct Node *r_node, char *buffer, size_t buf_size) {
    int i;
    struct Node *node_itr = r_node, *child_itr;
    struct Node *min_child, *max_child;
    int is_user_white = r_node->board.white_moves;

    //The tree should be populated correctly depending on the user's move
    while (*(node_itr->children)) {

        //Iterate through children and find min and max children
        min_child = *(node_itr->children);
        max_child = min_child;
        for (child_itr = *(node_itr->children); child_itr; child_itr++) {
            if (child_itr->eval > max_child->eval) {
                max_child = child_itr;
            }
            if (child_itr->eval < min_child->eval) {
                min_child = child_itr;
            }
        }

        //It is my move!
        //Get the Max child node
        if ((is_user_white && node_itr->board.white_moves) || (!is_user_white && !node_itr->board.white_moves)) {
            strcat(buffer, " ");
            strcat(buffer, max_child->last_move);
            node_itr = max_child;
        }
        //It is opponent's move
        //Get the Min child node
        else {
            strcat(buffer, " ");
            strcat(buffer, min_child->last_move);
            node_itr = min_child;
        }
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
    uint64_t prng_state = (start_time*(args.index+1))^(0x8c248aedad57084d);
    int curr_depth = 0, eval_display, is_lock_acquired;
    char pv[MAX_BUF_SIZE];
    char **tokens;

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
            search_time_nanos = (((args.wtime - 1000)/args.movestogo) + args.winc)*1000000L;
        }
        //I am black
        else {
            search_time_nanos = (((args.btime - 1000)/args.movestogo) + args.binc)*1000000L;
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
        while (!is_lock_acquired) {
            //Start search from root
            my_node = root;

            //Keep searching until our node has no children
            while (my_node->children) {
                //Grade each of the children by considering variables:
                //1) Best child eval
                //2) Total nodes visited
                //3) Each child eval
                //4) Each child visits

                //Sort the children by their grade

                //Move our node to the winning child
            }
            //Lock the leaf node

            //If we have no children now, then we can stop searching
            if (!my_node->children) {
                is_lock_acquired = 1;
            }
            else {
                //Someone got to our leaf node first.  Restart search.
                //Release lock
            }
        }

        //Bloom the leaf node
        m_bloom_node(root);

        //Push the new nodes' values to the parents all the way to root

        //Managerial duties for the manager thread = 0
        if (args.index == 0) {

            curr_time = get_nanos();

            //Print out depth information if new depth is reached
            if (root->depth > curr_depth) {
                curr_depth = root->depth;
                get_pv(root, pv, sizeof(pv));
                //Display integer version of eval
                if (root->eval > 0) {
                    eval_display = (int) (root->eval + 0.5);
                }
                else if (root->eval < 0) {
                    eval_display = (int) (root->eval - 0.5);
                }
                else {
                    eval_display = 0;
                }
                printf("info depth %d seldepth %d multipv %d score cp %d nodes %d nps %d tbhits %d time %d pv %s\n", curr_depth, curr_depth, 1, eval_display, root->visits, 1, 0, (curr_time-start_time)/(1000000L), pv);
            }

            /*
            Giving all possible reasons to stop searching:
            1) Calculated search time is expired
            2) Depth is exhausted
            3) Nodes is exhausted
            */
            if (!args.infinite && !args.ponder && ((curr_time - start_time > search_time_nanos) || root->depth >= args.depth || root->visits >= args.nodes)) {
                //Stop all searching
                stop_pondering = 1;
            }
        }
    }

    //Manager thread prints information back to GUI
    if (args.index == 0) {
        get_pv(root, pv, sizeof(pv));
        tokens = m_tokenize_input(pv, sizeof(pv));
        if (args.ponder_enabled) {
            printf("bestmove %s ponder %s\n", tokens[0], tokens[1]);
        }
        else {
            printf("bestmove %s\n", tokens[0]);
        }
        free_tokenize_input(tokens);
    }

}

void set_root_node(int ponder) {

}

void spawn_go_workers(int ponder, int wtime, int btime, int winc, int binc, int movestogo, int depth, int nodes, int movetime, int infinite) {

    int result_code, thread_count = 1, i, ponder_enabled = 0;
    struct Option *option_itr;

    //Check engine options
    for (option_itr = *options; option_itr; option_itr++) {
        if (strcmp(option_itr->name, "Threads") == 0) {
            thread_count = option_itr->spin;
        }
        else if (strcmp(option_itr->name, "Ponder") == 0) {
            ponder_enabled = option_itr->check;
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
