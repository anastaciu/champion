
//imprime o caracter c, size numero de vezes
char *print_chars(size_t size, const char* c);

//imprime no ficheiro com descritor fd_out com espaços
char *print_word(size_t size, const char* str);

//imprime o interface do jogo
void print_user_interface(const char *word, size_t size, const char *spacer, const char *menu);

//preenche a palavra str com o character c
void fill_word(char* str, size_t size, const char* c);