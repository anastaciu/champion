#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <ctype.h>
#include <stdlib.h>

#include "../global_interface.h"

char *print_chars(size_t size, const char c)
{
    char *temp_chars = malloc(size + 1);
    size_t i = 0;
    for (; i < size; i++)
        temp_chars[i] = c;
    temp_chars[i] = '\0';
    return temp_chars;
}

char* print_word(size_t size, const char *str)
{
    char *t_str = malloc(size * 2 + 2);
    size_t i = 0, j = 0;
    for ( ; i < size; i++, j += 2)
    {
        t_str[j] = str[i];
        t_str[j + 1] = ' ';
    }
    t_str[j] = '\n';
    t_str[j + 1] = '\0';
    return t_str;
}

void print_user_interface(const char *word, size_t size, const char *spacer, const char *menu)
{
    char temp_ui[2048];   
    char *temp = print_word(size, word);
    char *temp_chars = print_chars(size * 2 - 1, '*');
    sprintf(temp_ui, "%s%s%s%s%s%s",  spacer, temp_chars, spacer, temp, spacer, temp_chars);
    free(temp);
    free(temp_chars);
    print(temp_ui, STDOUT_FILENO);
    print(menu, STDOUT_FILENO);
    
}

void fill_word(char *str, size_t size, const char *c)
{
    size_t i;
    for (i = 0; i < size; i++)
        str[i] = c[0];
    str[i] = '\0';
}