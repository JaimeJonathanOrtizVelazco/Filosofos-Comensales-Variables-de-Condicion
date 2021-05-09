#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#define tenedores 2

struct filosofo {
    struct filosofo *next;
    struct filosofo *before;
    int status;
};
struct filosofo filosofos[5];
pthread_mutex_t tenedor=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jarra=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ocupados= PTHREAD_COND_INITIALIZER;
int enuso=0;
void *filosofo(int i) {
    while (1) {
        pthread_mutex_lock(&tenedor);
            while (enuso==tenedores){
                pthread_cond_wait(&ocupados,&tenedor);
            }
            if(filosofos[i].before->status==0&&filosofos[i].next->status==0){
                enuso++;
                filosofos[i].status=1;
                printf("El filosofo%d %lu esta comiendo\n",i,pthread_self());
                pthread_mutex_unlock(&tenedor);
                sleep(2);
                filosofos[i].status=0;
                enuso--;
                printf("El filosofo%d %lu termino de comiendo\n",i,pthread_self());
                pthread_cond_signal(&ocupados);
            }else{
                printf("El filosofo%d %lu esta esperando comer\n",i,pthread_self());
                if (pthread_mutex_trylock(&jarra)==0){
                    printf("El filosofo%d %lu esta tomando agua\n",i,pthread_self());
                    pthread_mutex_unlock(&jarra);
                }
                pthread_mutex_unlock(&tenedor);
                sleep(2);
            }
    }
}
int main() {
    pthread_t th[5];
    for (int i = 0; i < 5; ++i) {
        if (i == 0){
            filosofos[i].before = &filosofos[4];}
        else
        {filosofos[i].before = &filosofos[i - 1];}
        if (i == 4)
        {filosofos[i].next = &filosofos[0];}
        else
        {filosofos[i].next = &filosofos[i + 1];}
        filosofos[i].status=0;//0 libre y 1 comiendo
    }
    for (int i = 0; i < 5; i++) {
        pthread_create(&th[i], NULL, (void *) filosofo, (void *) i);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}