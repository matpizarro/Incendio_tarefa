#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TAM 30

typedef struct {
    char mapa[TAM][TAM];
    pthread_mutex_t travas[TAM][TAM];
    pthread_cond_t condicoes[TAM][TAM];
} FlorestaMonitor;

void inicializa_mapa(FlorestaMonitor *floresta);
void bloquear_posicao(FlorestaMonitor *floresta, int linha, int coluna);
void liberar_posicao(FlorestaMonitor *floresta, int linha, int coluna);
void aguardar_fogo(FlorestaMonitor *floresta, int linha, int coluna);
void avisar_fogo(FlorestaMonitor *floresta, int linha, int coluna);
void alertar_vizinhos(FlorestaMonitor *floresta, int linha, int coluna);
void mostrar_mapa(FlorestaMonitor *floresta);
void *sensor_funcao(void *arg);
void *gerar_fogo(void *arg);
void *controle_funcao(void *arg);
void extinguir_fogo(FlorestaMonitor *floresta, int linha, int coluna);

int main() {
    FlorestaMonitor floresta;
    pthread_t sensores[TAM * TAM], gerador_thread, controle_thread;

    inicializa_mapa(&floresta);

    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            pthread_create(&sensores[i * TAM + j], NULL, sensor_funcao, (void *)&floresta);
        }
    }

    pthread_create(&gerador_thread, NULL, gerar_fogo, (void *)&floresta);
    pthread_create(&controle_thread, NULL, controle_funcao, (void *)&floresta);

    for (int i = 0; i < TAM * TAM; i++) {
        pthread_join(sensores[i], NULL);
    }

    pthread_join(gerador_thread, NULL);
    pthread_join(controle_thread, NULL);

    return 0;
}
