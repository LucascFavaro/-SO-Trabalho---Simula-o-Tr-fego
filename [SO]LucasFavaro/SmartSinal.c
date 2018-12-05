/*Simulação de Tráfego - Sistemas Operacionais. 
2018.2
Lucas C. Favaro*/


#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <time.h>  
#include <math.h>
#define N 4
#define VAZIO 0
#define ESQUERDA (rdnum + 3) % N
#define DIREITA (rdnum+1) % N

int estado[N];
int entrou[N]={0,0,0,0}; //carros que entraram
int pista[N] = {0,1,2,3};//controle das pistas
int contador = 0; //contador geral de carros
int sinal = 0; //(0: threads 0 e 2 avançam; 1: threads 1 e 3)
int cont_loc[N]={0,0,0,0};//contador local

double seconds;

sem_t mutex;

void teste(int rdnum)
{
	//sinal em zero: 0 e 2 avançam
	//sinal em 1: 1 e 3 avançam
	if ((estado[rdnum] > VAZIO && sinal == 0 && ((rdnum == 0)||rdnum == 2))||(estado[rdnum] > VAZIO && sinal == 1 && ((rdnum == 1)||rdnum == 3))) {
		sleep(0);
		//printf ("Carro vindo da pista %d está passando no cruzamento\n", rdnum+1); //para debugar
		entrou[rdnum]=1;
		contador++;
	}
}

void entra(int rdnum){
	//carro entra no cruzamento
	int gerador;
	sem_wait(&mutex);
	//if (estado[rdnum]>0)
	//	printf("Ha um carro na pista %d\n",rdnum+1);
	//printf("... "); //para debugar
	teste(rdnum);
	sem_post(&mutex);
	sleep(0);
	
}

void sai(int rdnum){
	//carro sai do cruzamento
	sem_wait(&mutex);
	if (estado[rdnum]>0&&entrou[rdnum]==1){
		estado[rdnum]--;//tira carro da fila
	//	printf ("Carro vindo da pista %d passou no cruzamento\n", rdnum+1);//para debugar
		cont_loc[rdnum]++;//aumenta contador local da pista
		entrou[rdnum]=0;
	}
	sem_post(&mutex);
}

void* pistas(void *num){
	//funçao base de cada pista, carros entram e saem constantemente;
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
	//controla quando os carro chegam e quando o sinal abre 
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
	//	printf("Seconds: %f\n",seconds);//para debugar
		if (((fmod(seconds, 20) ==0)&&(sinal==0))||((sinal==0)&&((estado[0]+estado[2])<(50+estado[1]+estado[3]))))
				sinal = 1;
		if (((fmod(seconds, 20) ==0)&&(sinal==1))||
		((sinal==1)&&((estado[1]+estado[3])<(50+estado[0]+estado[2]))))
				sinal = 0;
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
    srand(time(0));
    sem_init(&mutex, 0, 1); 
  	pthread_create(&ctrl, NULL, controle,NULL);
    for (i = 0; i < N; i++) { 
        pthread_create(&thread_id[i], NULL, pistas, &pista[i]); 
    } 
  
    for (i = 0; i < N; i++) 
        pthread_join(thread_id[i], NULL); 
    printf("Contador: %d \n", contador);
    for (i = 0; i < N; i++){
		printf("Fila %d: %d      |    ",i+1,estado[i]);
		printf("Local %d: %d\n", i+1,cont_loc[i]) ;
	}
} 
