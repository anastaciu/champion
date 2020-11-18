
//imprime o caracter c, size numero de vezes
void print_chars(size_t size, const char* c, int fd_out);

//imprime no ficheiro com descritor fd_out com espaços
void print_word(size_t size, const char* str, int fd_out);

//imprime o interface do jogo
void print_user_interface(const char *word, size_t size, int fd, const char *spacer, const char *menu);

//preenche a palavra str com o character c
void fill_word(char* str, size_t size, const char* c);