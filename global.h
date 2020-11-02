
#define MAX_LEN_NAME 50                       //tamanho máximo para nome de named pipes
#define MAX_LEN_COM 50                        //tamanho máximo definido para vários argumentos de linha de comandos
#define MAX_LEN_USER 10                       //Tamanho máximo do username (9) com espaço para null byte
#define SERVER_LOG_FIFO "/tmp/login_fifo"     // nome do FIFO do servidor para login
#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente    

enum State{LOGGED};

typedef enum State State;

//dados de login enviadas do cliente para o servidor
typedef struct{
    char nome[MAX_LEN_USER];     
    pid_t user_pid;
}UserLog;

//resposta a tentativa de login de cliente
typedef struct{
    char pipe_name[MAX_LEN_NAME];
    State state;
    pid_t server_pid;
}LogResponse;
