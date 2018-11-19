void parse_uci(char *buffer, size_t buf_size) {

    printf("id name Doughboy %s 64\n", DOUGHBOY_VERSION);
    printf("id author Kyle Forrester\n");
    printf("\n");
    printf("option name Threads type spin default 1 min 1 max 128\n");
    printf("option name Ponder type check default false\n");
    printf("uciok\n");
    return;
}

void parse_setoption(char *buffer, size_t buf_size) {
    char **parsed_string = m_tokenize_input(buffer, buf_size);   
    char **p_itr;
    int parsed_size;
    struct Option **o_itr;
    for (p_itr = parsed_string; *p_itr; p_itr++);
    parsed_size = p_itr - parsed_string;

    if (parsed_size != 5) {
        return;
    }
    if (strcmp(parsed_string[0], "setoption") != 0 || strcmp(parsed_string[1], "name") != 0 || strcmp(parsed_string[3], "value") != 0) {
        return;
    }

    for (o_itr = options; *o_itr; o_itr++) {
        if (strcmp((*o_itr)->name, parsed_string[2]) == 0) {
            if (strcmp((*o_itr)->name, "Threads") == 0) {
                (*o_itr)->spin = atoi(parsed_string[4]);
            }
            else if (strcmp((*o_itr)->name, "Ponder") == 0) {
                if (strcmp(parsed_string[4], "true") == 0) {
                    (*o_itr)->check = 1;
                }
                else {
                    (*o_itr)->check = 0;
                }
            }
        }
    }

    free_tokenize_input(parsed_string);    
}

void parse_isready(char *buffer, size_t buf_size) {
    printf("readyok\n");
}

void parse_printoptions(char *buffer, size_t buf_size) {
    struct Option **o_itr;
    for (o_itr = options; *o_itr; o_itr++) {
        if (strcmp((*o_itr)->name, "Threads") == 0) {
            printf("Threads: %d\n", (*o_itr)->spin);
        }
        else if (strcmp((*o_itr)->name, "Ponder") == 0) {
            if ((*o_itr)->check == 0) {
                printf("Ponder: false\n");
            }
            else {
                printf("Ponder: true\n");
            }
        }
    }
}

void initialize_options() {
    struct Option **o_itr;
    options = malloc(sizeof(struct Option *) * 3);
    o_itr = options;
    *o_itr = malloc(sizeof(struct Option));
    **o_itr = (struct Option){.name="Threads", .check=0, .string="", .spin=1};
    o_itr++;
    *o_itr = malloc(sizeof(struct Option));
    **o_itr = (struct Option){.name="Ponder", .check=0, .string="", .spin=0};
    o_itr++;
    *o_itr = 0;
}

int main(int argc, char **argv) {

    printf("Doughboy %s 64 by Kyle Forrester\n", DOUGHBOY_VERSION);

    size_t buf_size = MAX_BUF_SIZE;
    char *buffer = malloc(sizeof(char) * buf_size);
    char first_word[MAX_BUF_SIZE];
    int debug = 0;

    initialize_options();

    while (getline(&buffer, &buf_size, stdin) <= MAX_BUF_SIZE) {
        //Strip the newline char off buffer
        buffer[strlen(buffer)-1] = 0;
        if (!str_first_word(first_word, MAX_BUF_SIZE, buffer)) {
            printf("Failed to parse first word of:\n%s\n",buffer);
            continue;
        }
        if (strcmp(first_word, "uci") == 0) {
            LOG(debug, "UCI detected!");
            parse_uci(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "setoption") == 0) {
            LOG(debug, "SetOption detected!");
            parse_setoption(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "isready") == 0) {
            LOG(debug, "IsReady detected!");
            parse_isready(buffer, strlen(buffer));
        }
        else if (strcmp(first_word, "printoptions") == 0) {
            LOG(debug, "Print detected!");
            parse_printoptions(buffer, strlen(buffer));
        }
        else {
            LOG(debug, "Received Unknown command.");
        }
    }


    //TODO: Parse the sent command
}

