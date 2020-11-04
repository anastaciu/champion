
 Aplicações cliente servidor UNIX CHAMPION

Convenções de código:

 -  Código em inglês, comentários em português.
 -  Macros e constantes com maiúscula e separador underscore: MAX_SIZE, TRACKING_ID_INFO.
 -  Nomes de estruturas e typedefs em camelcase: GameData, OrderInfo.
 -  Enums com nome em camelcase e conteúdo em maiúscula com separador underscore: enum Enum{DOIS, DOIS_E_MEIO, TRES}
 -  Variáveis: minúsculas, uso de underscore para separação quando necessário: var_x, var_y.
 -  Funções em C clássico com minúsculas, uso de underscore para separação: get_user_input(), tracking_order().
 -  Ponteiros sem typedefes, sem ofuscação: *ptr, *var_ptr.
 -  Não se usam variáveis globais.
 -  Funções especiais para uso interno, para não serem chamadas pela API: _clear_buffer(), _destroy_data().

 
 game_000

 
                           ** Jogo da Forca **
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
