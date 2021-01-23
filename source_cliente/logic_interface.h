

//Função da thread de comunicação com o servidor
void *com_thread(void *arg);

//handler para signal
void sig_handler(int sig);

void *cli_thread(void *arg);

int fd_is_valid(int fd);

