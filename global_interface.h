
//imprime str no ficheiro com o descritor fd_out
void print(const char* str, int fd_out);

//verifica se ainda existem characteres no ficheiro com o descritor fd
//caso se verifique, limpa-o
void _clear_buffer(int fd);

//lê palavra do ficheiro com o descritor fd 
//acrescenta o null byte e remove newline, tab ou espaços do fim
//size é o tamanho máximo da string, caracteres a mais serão ignorados, espaços em branco no fim e no início são contabilizados apesar de serem removidos
//retorna o comprimento final de input
size_t get_user_input(char *input, int fd, int size);

//converte str para maiúsculas
void _to_upper(char *str);
