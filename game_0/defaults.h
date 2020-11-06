
//estados do ciclo principal
enum State{ HELP, LETTER_FOUND, LETTER_NOT_FOUND, WORD_FOUND, WRONG_WORD, REPEAT, GAME_OVER};

//pontos vitória por adivinhar palavra e por adivinhar todas as letras
#define VICTORY 250
#define VICTORY_ATTEMPTS 10
#define VICTORY_LETTERS 200
#define VICTORY_LETTERS_ATTEMPTS 5

//estrutura com dados de jogo
typedef struct {
	size_t count;      //número de letras encontradas
	size_t size;	   //tamanho da palavra
	size_t attempts;   //contador de tentativas
	size_t points;     //pontos finais
} GameData;