#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>

// 64kB stack
#define FIBER_STACK 1024*64

// Definindo a estrutura de contexto para compartilhar informações entre threads
typedef struct {
    char message[100]; // Mensagem de exemplo
} thread_context;

// The child thread will execute this function
int threadFunction(void* argument) {
    thread_context *ctx = (thread_context*)argument;
    printf("Child thread sees message: %s\n", ctx->message);
    strcpy(ctx->message, "Hello from child");
    printf("Child thread exiting\n");
    return 0;
}

int main() {
    void* stack;
    pid_t pid;
    thread_context *ctx;

    // Allocate the stack
    stack = malloc(FIBER_STACK);
    if (stack == 0) {
        perror("malloc: could not allocate stack");
        exit(1);
    }

    // Allocate memory for thread_context
    ctx = malloc(sizeof(thread_context));
    if (ctx == 0) {
        perror("malloc: could not allocate thread context");
        exit(1);
    }
    strcpy(ctx->message, "Hello from parent");

    printf("Creating child thread\n");

    // Call the clone system call to create the child thread
    pid = clone(&threadFunction, (char*) stack + FIBER_STACK,
        SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, ctx);
    if (pid == -1) {
        perror("clone");
        exit(2);
    }

    // Wait for the child thread to exit
    pid = waitpid(pid, 0, 0);
    if (pid == -1) {
        perror("waitpid");
        exit(3);
    }

    // Check the message from the child
    printf("Parent sees message: %s\n", ctx->message);

    // Free the stack
    free(stack);

    // Free the thread_context
    free(ctx);

    printf("Child thread returned and stack freed.\n");

    return 0;
}

