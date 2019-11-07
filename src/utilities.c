#include "chess.h"
//Taken from KnR Second Edition Section 7.7
char *knr_fgets(char *s, int n, FILE *iop) {
    register int c;
    register char *cs;

    cs = s;
    while (--n > 0 && (c = getc(iop)) != EOF) {
        if ((*cs++ = c) == '\n')
            break;
    }
    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}

int str_first_word(char *buffer, int buf_size, char *input) {
    int i;
    for (i = 0; i < strlen(input) && *(input+i) != ' ' && *(input+i) != '\n' && i < buf_size-1; i++) {
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

void print_tokenized_input(char **input) {
    int i;

    printf("Entered: ");
    for (i = 0; input[i]; i++) {
        printf("%s ", input[i]);
    }
    printf("\n");
}

char **m_tokenize_input(char *input, size_t input_size) {
    int space_count = char_count(input, input_size, ' ');
    int start = 0, end = 0, creations = 0, i;
    char **ret_val = malloc(sizeof(char *) * (space_count+2));

    while (start < input_size && input[start] && creations < space_count + 1) {
        while (start < input_size && input[start] && input[start] == ' ') {
            start++;
        }

        end = start;
        while (end < input_size && input[end] && input[end] != ' ') {
            end++;
        }

        ret_val[creations] = malloc(sizeof(char) * (1 + (end - start)));

        for (i = 0; i < (end - start); i++) {
            ret_val[creations][i] = input[start + i];
        }
        ret_val[creations][i] = '\0';
        //printf("Created string %s\n", ret_val[creations]);
        creations++;
        start = end;
    }
    ret_val[creations] = NULL;

    //print_tokenized_input(ret_val);
    return ret_val;
}

void strip_line_endings(char *input, size_t input_size) {
    int i;
    char *start = input;
    //Find the end of the input
    for (i = 0; *input && i < input_size; input++, i++);

    //Walk backwards deleting line endings
    for (i = 1; (input - i) >= start && (input[-i] == '\r' || input[-i] == '\n'); i++) {
        input[-i] = 0;
    }
}

void free_tokenize_input(char **input) {
    char ** input_itr;

    for (input_itr = input; *input_itr; input_itr++) {
        free(*input_itr);
    }
    free(input);
}

void free_node(struct Node *my_node) {
    int i;
    for (i = 0; i < my_node->child_count; i++) {
        free_node(my_node->children[i]);
    }

    free(my_node->children);
    free(my_node);
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
uint32_t spcg32(uint64_t s[1])
{
    uint64_t m = 0x9b60933458e17d7dULL;
    uint64_t a = 0xd737232eeccdf7edULL;
    *s = *s * m + a;
    int shift = 29 - (*s >> 61);
    return *s >> shift;
}

//POSIX Timestamp method
//Credit:
//https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c/36095407#36095407
uint64_t get_nanos() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

//Converts win percentages to centipawn evaluations
int eval_to_cp(double win_percent) {
    double temp_eval;

    //Display cp version of eval
    //Remember eval is a double between 0 and 1
    if (win_percent > 0.5) {
        //We are winning
        temp_eval = sqrt((20000*win_percent - 10000)/(1 - win_percent));
        //Round to nearest int
        return (int) (temp_eval + 0.5);
    }
    else if (win_percent < 0.5) {
        //We are losing
        temp_eval = 1 - win_percent;
        temp_eval = sqrt((20000*temp_eval - 10000)/(1 - temp_eval));
        //Round to nearest int
        return (int) (-temp_eval - 0.5);
    }
    else {
        return 0;
    }
}

//Reverses the bits of an unsigned integer
uint64_t reverse_bits(uint64_t orig) {
    int count = 63;
    uint64_t new = 0;

    while (orig) {
        new |= orig & 1;
        orig >>= 1;
        new <<= 1;
        count--;
    }

    new <<= count;
    return new;
}
