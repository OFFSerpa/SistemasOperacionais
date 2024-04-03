#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define FIBER_STACK 1024*64
#define NUM_THREADS 10 // Definindo o número de threads para criar

struct c {
    int saldo;
    pthread_mutex_t lock; 
};
typedef struct c conta;

conta from, to;
int valor;

// Função de transferência modificada para ser compatível com pthreads
void* transferencia(void *arg) {
    pthread_mutex_lock(&from.lock); 
    if (from.saldo >= valor) {
        from.saldo -= valor;
        to.saldo += valor;
    }
    pthread_mutex_unlock(&from.lock); 

    printf("Transferência concluída com sucesso!\n");
    printf("Saldo de c1: %d\n", from.saldo);
    printf("Saldo de c2: %d\n", to.saldo);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    
    pthread_mutex_init(&from.lock, NULL);
    pthread_mutex_init(&to.lock, NULL);

    from.saldo = 100;
    to.saldo = 100;
    valor = 10;

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&threads[i], NULL, transferencia, NULL);
        if (rc) {
            printf("ERRO; o código de retorno de pthread_create() é %d\n", rc);
            exit(-1);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

	    printf("Transferências concluídas e memória liberadas.\n");

    pthread_mutex_destroy(&from.lock);
    pthread_mutex_destroy(&to.lock);

    return 0;
}

