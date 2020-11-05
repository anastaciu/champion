//tamanho máximo do output
#define OUTPUTSIZE 150

//imprime str no ficheiro com o descritor fd_out
void print(const char* str, int fd_out);

//verifica se ainda existem characteres no stdin, e caso se verifique, limpa-o
void _clear_buffer(int fd);

//lê linha do stdin, acrescenta o null byte e remove \n \t ou espaços do fim
size_t get_user_input(char* input, int fd, int size);