
//Verifica argumentos da linha de comandos e estabelece valores para tempo de espera e tempo de jogo retorna flags de erro específicas
CmdArgs command_line_arguments(long *wait_time, long *duration, int argc, char **argv);
//Verifica variável de ambiente MAXPLAYER e estabelece o seu valor, retorna -1 se valor for inválido
int get_maxplayer();