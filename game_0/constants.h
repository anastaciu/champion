
//tamanho máximo do output
#define OUTPUTSIZE 150

//descritores de entrada e saída
#define FD_OUT fileno(stdout)
#define FD_IN fileno(stdin)

//imprime cabeçaho com instruções
#define HEADER "\n\
                           ** Jogo da Forca ** \n\
-------------------------------------------------------------------------------\n\
|        Neste jogo você deve tentar adivinhar qual a palavra escondida       |\n\
|      Você pode escolher uma letra de cada vez enquanto tiver tentativas     |\n\
|          Apenas letras de 'A' a 'Z' conforme a tabela ASCII 7 bits          |\n\
| O número de tentativas corresponde a metade do número de letras na palavra  |\n\
|       Pedir letra ou tentar adivinhar palavra descontam uma tentativa       |\n\
|         Se a letra for encontrada o número de tentativas mantém-se          |\n\
-------------------------------------------------------------------------------\n\
|                     Cada letra adivinhada vale 5 pontos                     |\n\
|      Caso não acerte, a pontuaçao final corresponde às letras descobertas   |\n\
-------------------------------------------------------------------------------\n\
|    Caso acerte os pontos das letras descobertas não serão contabilizados    |\n\
|    Adivinhar a palavra tem uma pontuação de 250 pontos + 10 x tentativas    |\n\
| Adivinhar todas as letras tem uma pontuação de 200 pontos + 5 x tentativas  |\n\
-------------------------------------------------------------------------------"

#define MENU "\n\n* Escoha uma letra\n* ? para ver regras\n* Ou tente adivinhar a palavra\n>"
#define LETTER_FOUND_OUT "\n\t\t'%c' foi encontrado. Continua com %zu tentativas"
#define LETTER_NOT_FOUND_OUT "\n\t\t'%c' não foi encontrado. Tem mais %zu tentativas"
#define WORD_FOUND_OUT "\n\n\n\t\tVocê acertou, '%s' era a palavra, ganhou %zu pontos!!\n\n"
#define WRONG_WORD_OUT "\n\n\n\t\tVocê não acertou na palavra. Restam %zu tentativas\n\n"
#define GAME_OVER_OUT "\n\n\n\t\tVocê não acertou, a palavra era '%s', ganhou %zu pontos!!\n\n"
#define REPEAT_OUT "\t\t\tEssa letra já existe na palavra!!"

//imprime espaçador para palavra
#define SPACER "\n\n\t\t\t\t"

//estados do ciclo principal
enum State{ HELP, LETTER_FOUND, LETTER_NOT_FOUND, WORD_FOUND, WRONG_WORD, REPEAT, GAME_OVER};

//pontos vitória
#define VICTORY 250
#define VICTORY_LETTERS 200

//estrutura com dados de jogo
typedef struct {
	size_t count;      //numero de letras encontradas
	size_t size;	   //tamanho da palavra
	size_t attempts;   //contador de tentativas
	size_t points;     //pontos finais
} GameData;
