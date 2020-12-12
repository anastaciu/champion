
#define OUTPUT_SIZE 150     // tamanho máximo de mensagens de jogo

//estados do ciclo principal
enum State{ HELP, LETTER_FOUND, LETTER_NOT_FOUND, WORD_FOUND, WRONG_WORD, REPEAT, GAME_OVER};

//pontos vitória por adivinhar palavra e por adivinhar todas as letras
#define VICTORY 10
#define VICTORY_ATTEMPTS 2
#define VICTORY_LETTERS 5
#define VICTORY_LETTERS_ATTEMPTS 1

//estrutura com dados de jogo
typedef struct {
	size_t count;      //número de letras encontradas
	size_t size;	   //tamanho da palavra
	size_t attempts;   //contador de tentativas
	int points;     //pontos finais
} GameData;
