#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DOUGHBOY_VERSION "v0.1.0"
#define LOG(x,y) if (x) printf(y)
#define MAX_BUF_SIZE 4096
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))

struct Option {
    char name[50];
    int check;
    char string[50];
    int spin;
};
struct Option **options;

#include "utilities.c"
#include "main.c"
