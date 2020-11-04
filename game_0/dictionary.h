
//tamanho máximo da palavra
#define WORDSIZE 20

//dicionário de palavras
const char WORDS[][WORDSIZE] = {"EMPATIA", "EMBUSTE", "SUBLIME", "SUCINTO", "INFERIR", "CINISMO", "REFUTAR", "REDIMIR", "CORDIAL", "EXORTAR", "EMERGIR", "IMPUTAR", "TRIVIAL", "ASPECTO",
                                "CANDURA", "VIRTUDE", "DEBOCHE", "MITIGAR", "ALMEJAR", "EXCERTO", "CULTURA", "SOBERBA", "AUSTERO", "ORIUNDO", "EXCESSO", "ALEGRIA", "CONTUDO", 
                                "EXILADO", "INERENTE", "DENEGRIR", "RESPEITO", "PRUDENTE" "DEFERIDO", "IMINENTE", "INVASIVO", "EQUIDADE", "ALIENADO", "EMINENTE", "ABSTRATO", "REITERAR", "PREMISSA", 
                                "ARDILOSO", "CONCEITO", "DEVANEIO", "RELATIVO", "ATIVISTA", "ALICERCE", "RESPALDO", "ANALOGIA", "ABNEGADO", "DESPEITO", "CONCERNE", "DESASTRE", "DISTINTO", "PRIMAZIA", 
                                "PROCEDER", "GENOCIDA", "SUSCITAR", "CONSISTE", "FOMENTAR", "DESFECHO", "COMPLEXO", "SUCUMBIR", "PORTANTO", "INSTIGAR", "PANDEMIA", "RESOLUTO", "EXPEDIDO", "SANIDADE", 
                                "DEMAGOGO", "SINGULAR", "REPUDIAR", "ABSTRAIR", "PROSAICO", "FULGURAS", "MESTRIA", "PRENDADO", "PARADOXO", "PRECEDER", "OBSTANTE", "PARALELO", "REFLETIR", "PROCESSO", 
                                "CONTEXTO", "IMINENTE", "CONTENDA", "ESPECTRO", "VIGOROSO", "PROBLEMA", "MOLESTAR", "OBJETIVO", "MEDIANTE", "PERVERSO", "SUBJUGAR", "ENXERGAR", "CONCEDER", "PERSPICAZ", 
                                "RETIFICAR", "ESSENCIAL", "PLENITUDE", "HEGEMONIA", "PARADIGMA", "DICOTOMIA", "RATIFICAR", "INCIDENTE", "DELIBERAR", "PERSUADIR", "RESIGNADO", "DEMASIADO", "MESQUINHO", 
                                "VAGABUNDO", "IMPETUOSO", "DESDENHAR", "REGOZIJAR", "INUSITADO", "DILIGENTE", "FIDEDIGNO", "TACITURNO", "COMPANHIA", "CONSTANTE", "DISCORRER", "ADJACENTE", "IMPARCIAL", 
                                "ESTAGNADO", "INTERESSE", "LUDIBRIAR", "ENTEDIADO", "OBSTINADO", "SUPLANTAR", "PEDERASTA", "EXCEDENTE", "NOSTALGIA", "CONSOANTE", "SUBJETIVO", "SALIENTAR", "AMBICIOSO", 
                                "ENFADONHO", "EXCELENTE", "RELEVANTE", "ARRAIGADO", "IMPONENTE", "PRESCINDIR", "CORROBORAR", "DETRIMENTO", "MATURIDADE", "INCIPIENTE", "INDULGENTE", "SAGACIDADE", 
                                "SERENIDADE", "AUSPICIOSO", "DISRUPTIVO", "PRECONIZAR", "SUBESTIMAR", "PREPOTENTE", "VISLUMBRAR", "DEPREENDER", "PRECEDENTE", "PERTINENTE", "SORORIDADE", "EQUIVOCADO", 
                                "PRELIMINAR", "ENTRETANTO", "INIQUIDADE", "LISONJEADO", "SINTETIZAR", "CONTEMPLAR", "COMPASSIVO", "RESTRINGIR", "COERCITIVO", "PRIMORDIAL", "TRANSEUNTE", "IMPORTANTE", 
                                "HIPOCRISIA", "PEJORATIVO", "DESPROVIDO", "FINALIDADE", "PERNICIOSO", "SUBJACENTE", "DISSEMINAR", "MOROSIDADE", "DIVERGENTE", "EXACERBADO", "CONSOLIDAR",
                                "CONCISO", "SENSATO", "ALCUNHA","SUCESSO", "SALUTAR", "PADECER", "SIGNIFICADO", "PERSPECTIVA", "DISSIMULADO", "COMPLACENTE", "VICISSITUDE", "CONTUNDENTE", 
                                "EXPECTATIVA", "BELIGERANTE", "INDIFERENTE", "PRESSUPOSTO", "CONTINGENTE", "CONTENCIOSO", "TRANSCENDER", "CONCERNENTE", "PROEMINENTE", "SUPERFICIAL", "ANTAGONISMO", 
                                "NECESSIDADE", "INTENSIDADE", "BENIGNIDADE", "PRECONCEITO", "REIVINDICAR", "SUPRACITADO","CUMPRIMENTO", "COMPREENDER", "PROVENIENTE","BENEVOLENTE", "ANIMOSIDADE", 
                                "DISPLICENTE", "SUBSEQUENTE", "CONVENIENTE", "ESTABELECER", "OPORTUNISTA", "IDENTIFICAR", "GENERALIZAR", "AMBIGUIDADE", "DISCRIMINAR", "SUBORDINADO", "REMANESCENTE", 
                                "PRERROGATIVA", "EXTROVERTIDO", "PROCRASTINAR", "INDEPENDENTE", "INTERMITENTE", "COMPARTILHAR", "CORDIALIDADE", "SISTEMATIZAR", "CONCOMITANTE", "CONVENCIONAL", 
                                "COMPLEXIDADE", "IMPERTINENTE", "CONHECIMENTO", "INTERLOCUTOR", "RESPLANDECER", "PROPORCIONAR", "CUMPLICIDADE", "INTEMPESTIVO", "CONFORMIDADE", "PROSELITISMO", 
                                "MEDIOCRIDADE", "LIBERTINAGEM", "ESTABILIDADE", "PERSEVERANTE", "PARCIALMENTE", "CIRCUNSPECTO", "ESTIGMATIZAR", "SUPERESTIMAR", "FRATERNIDADE", "COMPLEMENTAR", 
                                "ENCLAUSURADO", "INTROVERTIDO", "DESLUMBRANTE", "CARACTERIZAR", "PROTAGONISTA", "CONDICIONADO", "RECIPROCIDADE", "INCONVENIENTE", "DISCERNIMENTO", "PROMISCUIDADE", 
                                "SOLIDARIEDADE", "INTRANSIGENTE", "APROVISIONADO", "CREDIBILIDADE", "TRANSCENDENTE", "CONTRAPARTIDA", "PERPENDICULAR", "ESTEREOTIPADO", "EPISTEMOLOGIA", 
                                "RESSENTIMENTO", "RECENSEAMENTO", "PREPONDERANTE", "PORMENORIZADO", "VEEMENTEMENTE", "POSSIBILIDADE", "SUPERVENIENTE", "ELEGIBILIDADE", "SINGULARIDADE", 
                                "INSTABILIDADE", "LONGANIMIDADE", "HETEROSSEXUAL", "RESSIGNIFICAR", "RECALCITRANTE", "MULTIFACETADO", "SENSIBILIDADE", "EVENTUALMENTE", "AUTENTICIDADE", 
                                "SUPERESTIMADO", "INCREDULIDADE", "SUBJETIVIDADE", "INCONSEQUENTE", "INTROSPECTIVO", "SOBRESSALENTE", "DESMISTIFICAR", "INTEGRALMENTE", "HOMOGENEIDADE", 
                                "CONDESCENDENTE", "IDIOSSINCRASIA", "DEMASIADAMENTE", "CONSUBSTANCIAR", "ARBITRARIEDADE", "ESPECIFICIDADE", "POSTERIORMENTE", "EMPREENDIMENTO",
                                "CONTEXTUALIZAR", "ATENCIOSAMENTE", "ENTRETENIMENTO", "PERIODICAMENTE", "INFRAESTRUTURA", "PAULATINAMENTE", "TRANSCENDENTAL", "DILIGENTEMENTE", 
                                "ANCESTRALIDADE", "ESPONTANEIDADE", "INSIGNIFICANTE", "ARREPENDIMENTO", "RECONHECIMENTO", "RELACIONAMENTO", "APLICABILIDADE", "IMPARCIALIDADE", 
                                "DESPRETENSIOSO", "DESPRENDIMENTO", "IMPESSOALIDADE", "PROCRASTINADOR", "PRESTATIVIDADE", "SOCIOAMBIENTAL", "LICENCIOSIDADE", "EXEQUIBILIDADE", 
                                "PUSILANIMIDADE", "CREDENCIAMENTO", "EXCENTRICIDADE", "POTENCIALIDADE", "PRINCIPALMENTE", "RESPLANDECENTE", "IMPLICITAMENTE", "PLAUSIBILIDADE", 
                                "ABUNDANTEMENTE", "COERCITIVIDADE", "FUNCIONALIDADE", "INTERMUNICIPAL", "MULTIPLICIDADE", "DISSOLUTAMENTE" };

//estrututa com info do dicionário de palavras
typedef struct
{
    const char (*ptr)[WORDSIZE];
    int size;

} Dictionary;