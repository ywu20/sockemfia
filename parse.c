#include "parse.h"

int countDelimiters(char *string, char delim) {
    char *n = string;
    int num = 1;
    while (*n) {
        if (*n == delim) {
            num++;
        }
        n++;
    }
    return num;
}

char ** parse_args(char *string, char delim) {
    int num = countDelimiters(string, delim) + 1;
    char *current;
    char ** result = malloc(num * sizeof(char *));
    int i = 0;
    char seperatorChars[2] = "\n";
        seperatorChars[1] = delim;
    while ((current = strsep(&string, seperatorChars)))
    {
        if (*current){
            result[i] = current;
            i++;
        }
    }
    result[i] = NULL;
    result = realloc(result, (i + 1) * sizeof(char *));
    return result;
}