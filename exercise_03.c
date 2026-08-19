/*
Scrivere un programma in C su Linux in cui un processo padre P0 genera 1 processo figlio P1.
Successivamente, il padre:
	- va in sleep per 2 secondi
	- ferma l'esecuzione del figlio usando il segnale SIGTSTP
	- va in sleep per 5 secondi
	- invio al figlio il segnale SIGCONT per farlo ripartire
	- attende la terminazione del figlio e ne stampa lo stato di terminazione
	- esce

Il figlio esegue per 5 iterazioni le seguenti operazioni:	
	- va in sleep per 1 secondo
	- stampa "Ciao mondo\n"
Successivamente il figlio genera un numero casuale n compreso tra 1 e 5 ed esce con stato di terminazione n.
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

// COSTANTI
#define N 5

// MAIN
int main(void){
	pid_t figlio;
	int n;
	int result;
	int status;
	int num;
		
	figlio = fork();
	switch(figlio){
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			// Inizio Codice Processo Figlio
			srand(getpid());
			for(int i=0; i<N; i++){
				sleep(1);
				printf("Ciao Mondo!\n");
			}					
			num = (rand()%(5-1+1))+1;
			exit(num);		
			// Fine Codice Processo Figlio
		default:
			break;
	}
	
	// Codice Processo Padre;
	sleep(2);
	n = getpid();
	result = kill(n, SIGTSTP);
	if(result == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
	
	sleep(5);
	n = getpid();
	result = kill(n, SIGCONT);
	if(result == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
	
	wait(&status);
	if(WIFEXITED(status)){
		printf("\n\nLo Stato di Terminazione del Figlio e': %d",WEXITSTATUS(status));
	}	
}
