
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

 