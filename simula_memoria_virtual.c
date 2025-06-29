/*
 * Trabalho Final de Sistemas Operacionais
 * Simulador de Algoritmos de Substituicao de Pagina (FIFO, LRU e OPT)
 *
 * Alunas: Lize Ana Zabote e Hellen Ramos
 * Professora: Prof. Marisangila Alves, MSc
 * Data: 30/06/2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // Usado para o INT_MAX no algoritmo Otimo e LRU

// --- Funcoes Auxiliares e de Simulacao ---

/**
 * @brief Verifica se uma pagina ja esta presente nos quadros da memoria RAM.
 * @param pagina A pagina a ser procurada.
 * @param quadros O array que representa os quadros da memoria.
 * @param num_quadros O numero total de quadros disponiveis.
 * @return 1 se a pagina for encontrada (hit), 0 caso contrario (miss).
 */
int busca_pagina(int pagina, int quadros[], int num_quadros) {
    for (int i = 0; i < num_quadros; i++) {
        if (quadros[i] == pagina) {
            return 1; // Hit!
        }
    }
    return 0; // Miss!
}

/**
 * @brief Simula o algoritmo de substituicao de pagina First-In, First-Out (FIFO).
 * @param refs A string de referencias de paginas.
 * @param num_refs O total de referencias na string.
 * @param num_quadros O numero de quadros na memoria.
 * @return O numero total de faltas de pagina (page faults).
 */
int simula_fifo(int refs[], int num_refs, int num_quadros) {
    if (num_quadros == 0) return num_refs;

    int quadros[num_quadros];
    for (int i = 0; i < num_quadros; i++) quadros[i] = -1; // -1 indica um quadro vazio

    int falta_de_pagina = 0;
    int ponteiro_vitima = 0; // Ponteiro que aponta para a pagina mais antiga (a proxima a ser substituida)

    for (int i = 0; i < num_refs; i++) {
        // Se a pagina nao esta na memoria, ocorre uma falta de pagina
        if (!busca_pagina(refs[i], quadros, num_quadros)) {
            falta_de_pagina++;
            // Coloca a nova pagina no quadro indicado pelo ponteiro
            quadros[ponteiro_vitima] = refs[i];
            // Avanca o ponteiro para o proximo quadro, de forma circular
            ponteiro_vitima = (ponteiro_vitima + 1) % num_quadros;
        }
    }
    return falta_de_pagina;
}

/**
 * @brief Simula o algoritmo de substituicao de pagina Least Recently Used (LRU).
 * @param refs A string de referencias de paginas.
 * @param num_refs O total de referencias na string.
 * @param num_quadros O numero de quadros na memoria.
 * @return O numero total de faltas de pagina (page faults).
 */
int simula_lru(int refs[], int num_refs, int num_quadros) {
    if (num_quadros == 0) return num_refs;

    int quadros[num_quadros];
    // Array auxiliar para rastrear o "tempo" da ultima vez que uma pagina foi usada
    int tempo_ultimo_uso[num_quadros];

    for (int i = 0; i < num_quadros; i++) {
        quadros[i] = -1;
        tempo_ultimo_uso[i] = 0;
    }

    int falta_de_pagina = 0;
    int tempo_atual = 1; // Relogio logico que incrementa a cada referencia

    for (int i = 0; i < num_refs; i++) {
        int pagina_atual = refs[i];
        int indice_encontrado = -1;

        // Busca a pagina na memoria para ver se e um hit ou miss
        for (int j = 0; j < num_quadros; j++) {
            if (quadros[j] == pagina_atual) {
                indice_encontrado = j;
                break;
            }
        }

        if (indice_encontrado != -1) { // Hit: a pagina esta na memoria
            // Atualiza o tempo de ultimo uso da pagina para o tempo atual
            tempo_ultimo_uso[indice_encontrado] = tempo_atual;
        } else { // Miss: falta de pagina
            falta_de_pagina++;

            int indice_vitima = -1;
            int menor_tempo = INT_MAX;

            // Procura por um quadro vazio primeiro
            for (int j = 0; j < num_quadros; j++) {
                if (quadros[j] == -1) {
                    indice_vitima = j;
                    break;
                }
            }

            // Se nao ha quadros vazios, encontra a pagina menos recentemente usada (LRU)
            if (indice_vitima == -1) {
                for (int j = 0; j < num_quadros; j++) {
                    if (tempo_ultimo_uso[j] < menor_tempo) {
                        menor_tempo = tempo_ultimo_uso[j];
                        indice_vitima = j;
                    }
                }
            }
            
            // Substitui a pagina vitima pela nova pagina
            quadros[indice_vitima] = pagina_atual;
            tempo_ultimo_uso[indice_vitima] = tempo_atual;
        }
        tempo_atual++; // Avanca o relogio
    }
    return falta_de_pagina;
}

/**
 * @brief Simula o algoritmo de substituicao de pagina Otimo (OPT).
 * @param refs A string de referencias de paginas.
 * @param num_refs O total de referencias na string.
 * @param num_quadros O numero de quadros na memoria.
 * @return O numero total de faltas de pagina (page faults).
 */
int simula_opt(int refs[], int num_refs, int num_quadros) {
    if (num_quadros == 0) return num_refs;

    int quadros[num_quadros];
    for (int i = 0; i < num_quadros; i++) quadros[i] = -1;

    int falta_de_pagina = 0;

    for (int i = 0; i < num_refs; i++) {
        if (!busca_pagina(refs[i], quadros, num_quadros)) { // Miss
            falta_de_pagina++;
            
            int indice_vazio = -1;
            for(int j=0; j < num_quadros; j++) {
                if(quadros[j] == -1) {
                    indice_vazio = j;
                    break;
                }
            }

            // Se ha um quadro vazio, usa-o
            if (indice_vazio != -1) {
                quadros[indice_vazio] = refs[i];
            } else {
                // Se a memoria esta cheia, precisamos encontrar a pagina vitima
                // A vitima e a pagina que sera usada mais longe no futuro (ou nunca)
                int indice_vitima = -1;
                int maior_distancia_futura = -1;

                for (int j = 0; j < num_quadros; j++) {
                    int distancia_proximo_uso = INT_MAX; // Assume que a pagina nao sera mais usada
                    
                    // Olha para frente na string de referencia
                    for (int k = i + 1; k < num_refs; k++) {
                        if (quadros[j] == refs[k]) {
                            distancia_proximo_uso = k; // Encontra o proximo uso
                            break;
                        }
                    }

                    // Se a distancia desta pagina e maior que a maior ja encontrada,
                    // ela se torna a candidata a vitima
                    if (distancia_proximo_uso > maior_distancia_futura) {
                        maior_distancia_futura = distancia_proximo_uso;
                        indice_vitima = j;
                    }
                }
                // Substitui a pagina otima
                quadros[indice_vitima] = refs[i];
            }
        }
    }
    return falta_de_pagina;
}

// --- Funcao Principal ---

/**
 * @brief Funcao principal do programa.
 * * Responsavel por:
 * 1. Validar os argumentos de entrada (numero de quadros).
 * 2. Ler a sequencia de referencias de paginas da entrada padrao.
 * 3. Invocar as funcoes de simulacao para os algoritmos FIFO, LRU e OPT.
 * 4. Imprimir os resultados formatados.
 * 5. Liberar a memoria alocada.
 * * @param argc Contador de argumentos da linha de comando.
 * @param argv Vetor de strings com os argumentos.
 * @return 0 em caso de sucesso, 1 em caso de erro.
 */
int main(int argc, char *argv[]) {

    // --- 1. Validacao dos argumentos da linha de comando ---
    // O programa espera exatamente um argumento: o numero de quadros.
    if (argc != 2) {
        fprintf(stderr, "Erro: Numero incorreto de argumentos.\n");
        fprintf(stderr, "Uso: %s <numero de quadros>\n", argv[0]);
        return 1;
    }

    // Converte o argumento de string para inteiro.
    int num_quadros = atoi(argv[1]);
    if (num_quadros <= 0) {
        fprintf(stderr, "Erro: O numero de quadros deve ser um inteiro positivo.\n");
        return 1;
    }

    // --- 2. Leitura da string de referencias da entrada padrao (stdin) ---
    int *referencias = NULL;      // Ponteiro para o array dinamico de paginas referenciadas.
    int num_referencias = 0;      // Contador de quantas referencias ja foram lidas.
    int capacidade = 10;          // Capacidade inicial do nosso array dinamico.

    // Aloca a memoria inicial para guardar as referencias.
    referencias = malloc(capacidade * sizeof(int));
    if (referencias == NULL) {
        fprintf(stderr, "Erro: Falha na alocacao de memoria inicial.\n");
        return 1;
    }

    int pagina_lida;
    // Laco para ler cada numero (pagina) da entrada ate o fim (EOF).
    // scanf retorna 1 se a leitura do inteiro for bem-sucedida.
    while (scanf("%d", &pagina_lida) == 1) {
        // Verifica se o array de referencias esta cheio.
        if (num_referencias >= capacidade) {
            // Se estiver cheio, dobramos a capacidade para evitar chamadas excessivas de realloc.
            capacidade *= 2;
            int *temp = realloc(referencias, capacidade * sizeof(int));
            if (temp == NULL) {
                fprintf(stderr, "Erro: Falha ao realocar memoria para as referencias.\n");
                free(referencias); // Libera a memoria antiga antes de sair.
                return 1;
            }
            referencias = temp; // Aponta para o novo bloco de memoria maior.
        }
        // Armazena a pagina lida no array e incrementa o contador.
        referencias[num_referencias++] = pagina_lida;
    }

    // --- 3. Executa as simulacoes para cada algoritmo ---
    int faltas_fifo = simula_fifo(referencias, num_referencias, num_quadros);
    int faltas_lru = simula_lru(referencias, num_referencias, num_quadros);
    int faltas_opt = simula_opt(referencias, num_referencias, num_quadros);

    // --- 4. Imprime o resultado final no formato especificado pelo trabalho ---
    printf("%5d quadros, %7d refs: FIFO: %5d PFs, LRU: %5d PFs, OPT: %5d PFs\n",
           num_quadros, num_referencias, faltas_fifo, faltas_lru, faltas_opt);

    // --- 5. Libera a memoria alocada dinamicamente ---
    // Boa pratica de programacao para evitar vazamento de memoria (memory leak).
    free(referencias);
    
    return 0; // Termina o programa com sucesso.
}