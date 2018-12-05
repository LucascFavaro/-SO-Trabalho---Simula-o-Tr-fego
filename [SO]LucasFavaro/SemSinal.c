/*Simula��o de Tr�fego - Sistemas Operacionais. 
2018.2
Lucas C. Favaro*/


#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <time.h>  
#define N 4
#define VAZIO 0
#define ESQUERDA (rdnum + 3) % N
#define DIREITA (rdnum+1) % N

int estado[N];
int entrou[N]={0,0,0,0}; //carros que entraram
int pista[N] = {0,1,2,3};//controle das pistas
int contador = 0; //contador geral de carros
int sinal = 0; //(0: threads 0 e 2 avan�am; 1: threads 1 e 3)
int cont_loc[N]={0,0,0,0};//contador local

double seconds;

sem_t mutex;

void teste(int rdnum)
{
	if (estado[rdnum] > VAZIO && estado[DIREITA] == VAZIO){
		//estado[rdnum]--;
		sleep(0);
		//printf ("Carro vindo da pista %d est� passando no cruzamento\n", rdnum+1);
		entrou[rdnum]=1;
		contador++;
	}
}

void entra(int rdnum){
	//carro entra no cruzamento
	int gerador;
	sem_wait(&mutex);
	teste(rdnum);
	sem_post(&mutex);
	sleep(0);
	
}

void sai(int rdnum){
	//carro sai do cruzamento
	sem_wait(&mutex);
	if (estado[rdnum]>0&&entrou[rdnum]==1){
		estado[rdnum]--;//tira carro da fila
		//printf ("Carro vindo da pista %d passou no cruzamento\n", rdnum+1); //para debugar
		cont_loc[rdnum]++;//aumenta contador local da pista
		entrou[rdnum]=0;
	}
	sem_post(&mutex);
}

void* pistas(void *num){
	//fun��o das threads
	while (1){
		int* i = num;
		sleep(0);
		entra(*i);
		sleep(0);
		sai(*i);
        if (seconds>1000)
        	break;
		
	}
}

void* controle(){
	//thread para gerar carros e controlar o cruzamento
	int gerador;
	int i;
	srand(time(0));
	while(1){
		int rd = 0;
		sem_wait(&mutex);
			rd = (rand() % 4);
			gerador = rand() % 3;
			estado[rd]=estado[rd]+gerador;
		seconds=seconds+.5;
		//printf("Seconds: %f\n",seconds); //para debugar
		sem_post(&mutex);
		if (seconds>1000)
			break;
	}
}
int main() 
{ 
  	//inicia as threads e chama as mesmas
    int i; 
    pthread_t thread_id[N]; 
    pthread_t ctrl;
    srand(time(NULL));
    sem_init(&mutex, 0, 1); //inicia sem�foro
  	pthread_create(&ctrl, NULL, controle,NULL);
    for (i = 0; i < N; i++) { 
        pthread_create(&thread_id[i], NULL, pistas, &pista[i]); 
    } 
  
    for (i = 0; i < N; i++) 
  
        pthread_join(thread_id[i], NULL); 
    printf("Contador: %d \n", contador);
    for (i = 0; i < N; i++){
		printf("Fila %d: %d       |      ",i+1,estado[i]);
		printf("Local %d: %d\n", i+1,cont_loc[i]) ;
	}
	printf("\n Pressiona tecla para sair");
	getchar();
} 
