//-------------------------------------------------------------------------//
//                  Universidade Federal de Santa Maria                    //
//                   Curso de Engenharia de Computação                     //
//                          Sistemas Operacionais                          //
//         								   //
//								 	   //
//   Autor: Iury Cleveston (201220748)                                     //
//   		                                                           //
//   Data: 11/11/2014                                                      //
//=========================================================================//
//                         Descrição do Programa                           //
//=========================================================================//
//   Escalonador time-sharing com prioridade para processos KERNEL         //
//                                                                         //
//-------------------------------------------------------------------------//

//Inclusão das bibliotecas
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

//Definição da Struct PROCESSO que contem as informacoes do processo. 

typedef struct {
    int id;
    int tipo;
    int tempo_execucao;
    struct PROCESSO *ant;
    struct PROCESSO *prox;
} PROCESSO;

//Struct que conterá a fila de processos

typedef struct {
    PROCESSO* fim;
} FILA;

//Definição da Struct f do tipo FILA
FILA *f;

//Contador de tempo
int tempo = 0;
//Tempo de quantum
int QUANTUM = 1;

//Constantes 
const int KERNEL = 1;
const int USUARIO = 0;

//Prototipos
PROCESSO* criaProcesso();
PROCESSO* retiraProcesso();
PROCESSO* escalonaProcesso();
void addProcesso(PROCESSO* p, int novo);
void listaProcesso();
int executaProcesso(PROCESSO* p);
void verificaProcessos();

int main() {

    f = (FILA*) malloc(sizeof (FILA)); //Cria fila de processos

    printf("Iniciando Simulacao.\n");

    //Tempo de execucao do processo.
    int contador = 0;
    //Tempo para terminar a simulacao
    int esperaEvento = 0;

    PROCESSO *p = NULL;

    while (1) {

        verificaProcessos(); //Verifica se chegou novos processos

        if (p == NULL)
            p = escalonaProcesso(); //Escolhe o processo a ser executado

        if (p != NULL) { //Verifica se algum processo foi escalonado.
            p = retiraProcesso(p);
            esperaEvento = 0;

            if (executaProcesso(p)) {
                contador++;
                if (contador == QUANTUM) { //Verifica se o tempo de quantum acabou
		    printf("%d segundos. Processo %d (%s) retornou para a fila\n", tempo + 1, p->id, (p->tipo == KERNEL) ? "kernel" : "usuario");  
		    contador = 0;
                    p = NULL;
		    
                }
            } else {
                contador = 0;
                p = NULL;
            }
        }

        tempo++;
        esperaEvento++;
        sleep(1); //Aguarda 1 segundo

        if (esperaEvento > 15) //Aguarda o acontecimento de algum evento durante 15s
            break; //Cai fora do laço e termina a simulação;

    }

    free(f);
    printf("Simulacao Finalizada.\n");
    return 0;
}

//Verifica a chegada de novos processos.

void verificaProcessos() {

    FILE *arquivo;
    char *string = (char*) malloc(100 * sizeof (char)); //Aloca a string que receberá a linha do arquivo
    char *pstr;

    arquivo = fopen("arquivo.txt", "r"); //Abre o arquivo

    if (arquivo != NULL) {

        fgets(string, 100, arquivo);
        fgets(string, 100, arquivo);

        if (QUANTUM != atoi(string)) {
            QUANTUM = atoi(string);
            printf("\nQUANTUM de %d\n\n", QUANTUM);
        }

        fgets(string, 100, arquivo);
        do {

            fgets(string, 100, arquivo); //Obtem uma linha do arquivo

            if (*(string) != '\n' && *(string) != '\0') {
                pstr = strtok(string, " ");
                int id = atoi(pstr);
                pstr = strtok(NULL, " ");
                int tempo_exec = atoi(pstr);
                pstr = strtok(NULL, " ");
                int tempo_cheg = atoi(pstr);
                pstr = strtok(NULL, " ");
                int tipo = atoi(pstr);

                if (tempo_cheg == tempo) { //Verifica se está no tempo do processo chegar.
                    PROCESSO * p = criaProcesso(); //Aloca recursos para o novo processo
                    pstr = strtok(string, " ");
                    p->id = id;
                    p->tempo_execucao = tempo_exec;
                    p->tipo = tipo;
                    addProcesso(p, 1); //Adiciona processo na fila de execucao.
                    printf("%d segundos. Processo %d (%s)(%d unidades) chegou.\n", tempo, p->id, (p->tipo == KERNEL) ? "kernel" : "usuario", p->tempo_execucao);
                }

            }

            *(string) = NULL;
            *(pstr) = NULL;

        } while (!feof(arquivo)); //Repete até o final do arquivo
        fclose(arquivo); //Fecha o arquivo
        free(string); //Libera a string
    } else
        printf("Arquivo nao encontrado. Verifique se o nome é 'arquivo.txt'.\n");

}

//Executa processo.

int executaProcesso(PROCESSO* p) {

    //Decrementa tempo de execucao
    p->tempo_execucao--;

    if (p->tempo_execucao == 0) {
        printf("%d segundos. Processo %d (%s) finalizado\n", tempo + 1, p->id, (p->tipo == KERNEL) ? "kernel" : "usuario");

        //Elimina recursos do processo.
        if (p == f->fim) {
            free(p);
            f->fim = NULL;
        } else {
            p->prox = NULL;
            p->ant = NULL;
            free(p);
            p = NULL;
        }
        return 0; //Retorna 0 quando o processo acabou.
    }
    //Adciona o processo na fila novamente.
    addProcesso(p, 0);
    
    return 1; //Retorna 1 quando o processo ainda precisa ser rodado.
}

//Função que aloca um novo novo processo 

PROCESSO * criaProcesso() {
    PROCESSO* p = (PROCESSO*) malloc(sizeof (PROCESSO));
    p->id = NULL;
    p->ant = NULL;
    p->prox = NULL;
    p->tempo_execucao = NULL;
    p->tipo = NULL;
    return p;
}

//Adiciona o processo na fla

void addProcesso(PROCESSO* p, int novo) {

    if (f->fim != NULL) {
        PROCESSO * prox = f->fim;
        PROCESSO * ant = prox->ant;

        p->prox = prox;
        p->ant = ant;
        ant->prox = p;
        prox->ant = p;

    } else {
        p->prox = p;
        p->ant = p;
    }

    //Se o processo for novo, atualiza o fim da fila.
    if (novo)
        f->fim = p->prox;

}

//Retira o processo da fila

PROCESSO * retiraProcesso(PROCESSO * p) {

    PROCESSO * prox = p->prox;
    PROCESSO * ant = p->ant;

    ant->prox = prox;
    prox->ant = ant;

    //Se o processo a ser retirado for o fim da fila, atualiza fim da fila.
    if (p == f->fim) {
        f->fim = p->prox;
    }

    return p;
}

//Seleciona o processo para ser executado.

PROCESSO * escalonaProcesso() {

    PROCESSO *p = NULL;

    if (f->fim != NULL) {
        p = f->fim;
        //Percorre a fila em busca de processo KERNEL
        do {
            if (p->tipo == KERNEL)
                break;
            else
                p = p->prox;
        } while (p != f->fim);

        printf("%d segundos. Processo %d (%s)(%d unidades) foi pra execução\n", tempo, p->id, (p->tipo == KERNEL) ? "kernel" : "usuario", p->tempo_execucao);
    }

    return p;
}