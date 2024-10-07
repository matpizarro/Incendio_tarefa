#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TAM 30

typedef struct {
    char mapa[TAM][TAM];
    pthread_mutex_t travas[TAM][TAM];
    pthread_cond_t condicoes[TAM][TAM];
} FlorestaMonitor;

void inicializa_mapa(FlorestaMonitor *floresta) {
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            floresta->mapa[i][j] = '-';
            pthread_mutex_init(&floresta->travas[i][j], NULL);
            pthread_cond_init(&floresta->condicoes[i][j], NULL);
        }
    }
}

void bloquear_posicao(FlorestaMonitor *floresta, int linha, int coluna) {
    pthread_mutex_lock(&floresta->travas[linha][coluna]);
}

void liberar_posicao(FlorestaMonitor *floresta, int linha, int coluna) {
    pthread_mutex_unlock(&floresta->travas[linha][coluna]);
}

void aguardar_fogo(FlorestaMonitor *floresta, int linha, int coluna) {
    pthread_cond_wait(&floresta->condicoes[linha][coluna], &floresta->travas[linha][coluna]);
}

void avisar_fogo(FlorestaMonitor *floresta, int linha, int coluna) {
    pthread_cond_signal(&floresta->condicoes[linha][coluna]);
}

void mostrar_mapa(FlorestaMonitor *floresta) {
    printf("\nMapa da floresta:\n");
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            printf("%c ", floresta->mapa[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void alertar_vizinhos(FlorestaMonitor *floresta, int linha, int coluna) {
    for (int i = linha - 1; i <= linha + 1; i++) {
        for (int j = coluna - 1; j <= coluna + 1; j++) {
            if (i >= 0 && i < TAM && j >= 0 && j < TAM && (i != linha || j != coluna)) {
                bloquear_posicao(floresta, i, j);
                if (floresta->mapa[i][j] == '-') {
                    floresta->mapa[i][j] = '!';
                    printf("Posição [%d][%d] alertada sobre o fogo!\n", i, j);
                }
                liberar_posicao(floresta, i, j);
            }
        }
    }
}

void extinguir_fogo(FlorestaMonitor *floresta, int linha, int coluna) {
    if (floresta->mapa[linha][coluna] == '@') {
        floresta->mapa[linha][coluna] = '/';
    }
}

void *sensor_funcao(void *arg) {
    FlorestaMonitor *floresta = (FlorestaMonitor *)arg;
    int linha = rand() % TAM;
    int coluna = rand() % TAM;

    while (1) {
        bloquear_posicao(floresta, linha, coluna);

        if (floresta->mapa[linha][coluna] == '@') {
            printf("Sensor na posição [%d][%d] detectou fogo!\n", linha, coluna);
            alertar_vizinhos(floresta, linha, coluna);
            mostrar_mapa(floresta);
        }

        liberar_posicao(floresta, linha, coluna);
        usleep(150000);
    }

    return NULL;
}

void *gerar_fogo(void *arg) {
    FlorestaMonitor *floresta = (FlorestaMonitor *)arg;

    while (1) {
        int linha = rand() % TAM;
        int coluna = rand() % TAM;

        bloquear_posicao(floresta, linha, coluna);

        if (floresta->mapa[linha][coluna] == '-') {
            floresta->mapa[linha][coluna] = '@';
            printf("Fogo iniciado em [%d][%d]\n", linha, coluna);
            mostrar_mapa(floresta);
        }

        liberar_posicao(floresta, linha, coluna);
        sleep(2);
    }

    return NULL;
}

void *controle_funcao(void *arg) {
    FlorestaMonitor *floresta = (FlorestaMonitor *)arg;

    while (1) {
        for (int i = 0; i < TAM; i++) {
            for (int j = 0; j < TAM; j++) {
                if ((i == 0 || i == TAM - 1 || j == 0 || j == TAM - 1) && floresta->mapa[i][j] == '!') {
                    printf("Central detectou fogo na borda [%d][%d], apagando fogo!\n", i, j);
                    extinguir_fogo(floresta, i, j);
                }
            }
        }
        usleep(200000);
    }

    return NULL;
}
