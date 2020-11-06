
//imprime str no ficheiro com o descritor fd_out
void print(const char* str, int fd_out);

//verifica se ainda existem characteres no stdin, e caso se verifique, limpa-o
void _clear_buffer(int fd);

//lê palavra do stdin, acrescenta o null byte e remove newline, tab ou espaços do fim
size_t get_user_input(char* input, int fd, int size);