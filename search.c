

void *update_tree(void *args) {
    int sleep_time, index = *((int *)args);
    //Seed with random number, diff for each thread
    uint64_t prng_state = (time(NULL)*index)^(0x8c248aedad57084d);
    uint32_t random = spcg32(&prng_state);

    sleep_time = 1 + random%10;
    sleep(sleep_time);

    printf("Ending thread %d after sleeping for %d seconds\n", index, sleep_time);
}

void m_populate_game_tree() {
    int thread_count = 1, i, result_code;
    struct Option *option_itr;

    //Set the Thread Count option
    for (option_itr = *options; option_itr; option_itr++) {
        if (strcmp(option_itr->name, "Threads") == 0) {
            thread_count = option_itr->spin;
            break;
        }
    }

    pthread_t threads[thread_count];
    int thread_args[thread_count];

    //Spawn the threads
    for (i = 0; i < thread_count; i++) {
        thread_args[i] = i;
        result_code = pthread_create(&threads[i], NULL, update_tree, &thread_args[i]);
        assert(!result_code);
    }

    //Wait for each thread to complete
    for (i = 0; i < thread_count; i++) {
        result_code = pthread_join(threads[i], NULL);
        assert(!result_code);

        printf("Thread %d has been rejoined!\n", i);
    }
    
}

void free_populate_game_tree() {

}
