
//imprime cabeçaho com instruções
#define HEADER "\n\
                           ** Jogo da Forca ** \n\
+-----------------------------------------------------------------------------+\n\
|        Neste jogo você deve tentar adivinhar qual a palavra escondida       |\n\
|      Você pode escolher uma letra de cada vez enquanto tiver tentativas     |\n\
|                 Apenas letras de 'A' a 'Z' ou de 'a' a 'z'                  |\n\
| O número de tentativas corresponde a metade do número de letras na palavra  |\n\
|            Pode tentar adivinhar a palavra escrendo-a na consola            |\n\
|       Pedir letra ou tentar adivinhar palavra descontam uma tentativa       |\n\
|         Se a letra for encontrada o número de tentativas mantém-se          |\n\
+-----------------------------------------------------------------------------+\n\
|                     Cada letra adivinhada vale 1 pontos                     |\n\
|      Caso não acerte, a pontuaçao final corresponde às letras descobertas   |\n\
+-----------------------------------------------------------------------------+\n\
|   Caso acerte, os pontos das letras descobertas não serão contabilizados    |\n\
|    Adivinhar a palavra tem uma pontuação de 10 pontos + 2 x tentativas      |\n\
| Adivinhar todas as letras tem uma pontuação de 5 pontos + 1 x tentativas    |\n\
+-----------------------------------------------------------------------------+\n"

#define MENU "\n\n* Escoha uma letra\n* ? para ver regras\n* Ou tente adivinhar a palavra\n>"
#define LETTER_FOUND_OUT "\n\n\t\t'%c' foi encontrado. Continua com %zu tentativas\n\n"
#define LETTER_NOT_FOUND_OUT "\n\n\t\t'%c' não foi encontrado. Tem mais %zu tentativas\n\n"
#define WORD_FOUND_OUT "\n\n\t\tVocê acertou, '%s' é palavra correcta, tem %d pontos!!\n\n"
#define WRONG_WORD_OUT "\n\n\t\tVocê não acertou na palavra. Restam %zu tentativas\n\n"
#define GAME_OVER_OUT "\n\n\t\tVocê não acertou, a palavra era '%s', tem %d pontos!!\n\n"
#define REPEAT_OUT "\n\n\t\t\tEssa letra já existe na palavra!!\n\n"

//imprime espaçador para palavra
#define SPACER "\n\n\t\t\t\t"
