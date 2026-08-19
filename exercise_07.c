/*
Scrivere un programma in C su Linux che simuli il lancio contemporaneo di N dadi.
• Il programma prevede un processo padre (P0) che:
  • Genera N processi figli (P1-PN)
  • Invia un segnale SIGUSR1 ad ogni processo figlio
  • Attende la terminazione di ogni processo figlio e ne legge lo stato di terminazione di ciascuno (quest'ultimo conterrà il valore di ciascun dado lanciato).
  • Stampa a video la somma di tutti i dadi e termina.
  
• Ciascun processo figlio:
  • Attende l'arrivo di SIGUSR1 dal processo padre.
  • Quando arriva il segnale calcola un numero intero random (chiamato num) tra 1 e 6
  • Termina con stato di terminazione "num".
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

// PROTOTIPI
void signal_handler(int sig);

// COSTANTI
#define N 5

// MAIN
int main(void){
	int result;
	int i;
	pid_t figli[N];
	int status;
	int somma = 0;
	
	struct sigaction action;
	result = sigemptyset(&(action.sa_mask));
	if(result == -1){
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}	
	action.sa_flags = 0;
	
	action.sa_handler = signal_handler;
	
	result = sigaction(SIGUSR1,&action, NULL);
	if(result == -1){
		perror("sigaction");
	 	exit(EXIT_FAILURE);
	}	
	
	for(i=0; i<N; i++){
		figli[i] = fork();
		switch(figli[i]){
			case -1:
				perror("fork");
				exit(EXIT_FAILURE);
			case 0:	
				pause();
				exit(0);
			default:
				break;
				
		}
	}
	
	for(i=0; i<N; i++){
		result = kill(figli[i],SIGUSR1);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
	}
	
	for(i=0; i<N; i++){
		wait(&status);
		if(WIFEXITED(status)){
			somma = somma + WEXITSTATUS(status);
		}
	}
	printf("\nLa Somma di Tutti i Numeri Usciti e': %d",somma);
}

// FUNZIONI
void signal_handler(int sig){
	srand(getpid());
	int num;
	if(sig == SIGUSR1){
		num = (rand()%(6-1+1))+1;
		printf("Numero: %d\n",num);
		exit(num);
	}
}
