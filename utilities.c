
int str_first_word(char *buffer, int buf_size, char *input) {
    int i;
    for (i = 0; *(input+i) != ' ' && *(input+i) != '\n' && i < buf_size-1; i++) {
        *(buffer+i) = *(input+i);
    }
    *(buffer+i) = '\0';
    return i;
}

int char_count(char *str, size_t str_len, char match) {
    int count = 0, i;
    for (i = 0; str[i]; i++) {
        if (str[i] == match) {
            count++;
        }
    }
    return count;
}

char **m_tokenize_input(char *input, size_t input_size) {
    char *temp = malloc(sizeof(char) * input_size);
    strcpy(temp, input);
    int space_count = char_count(temp, strlen(temp), ' ');
    char **ret_val = malloc(sizeof(char *) * (space_count+2));

    int i = 0;
    ret_val[i] = strtok(temp, " ");
    while (ret_val[i] != NULL && i < space_count) {
        i++;
        ret_val[i] = strtok(NULL, " ");
    }
    ret_val[space_count+1] = NULL;
    return ret_val;
}

void free_tokenize_input(char **input) {
    free(*input);
    free(input);
}

int lsb_index(uint64_t bb) {
    int i;
    bb &= ~bb + 1;
    for (i = -1; bb; i++) {
        bb = bb >> 1;
    }
    return i;
}

int bit_count(uint64_t bb) {
    int i;
    for (i = 0; bb; i++) {
        bb &= bb - 1;
    }
    return i;
}

//PRNG Algorithm
//Credit:
//https://nullprogram.com/blog/2017/09/21/
uint32_t
spcg32(uint64_t s[1])
{
    uint64_t m = 0x9b60933458e17d7d;
    uint64_t a = 0xd737232eeccdf7ed;
    *s = *s * m + a;
    int shift = 29 - (*s >> 61);
    return *s >> shift;
}

//POSIX Timestamp method
//Credit:
//https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c/36095407#36095407
uint64_t get_nanos() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (uint64_t)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
