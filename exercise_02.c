/*
Scrivere un programma in C su Linux in cui un processo padre P0 genera N processi figli P1...PN.

Il padre, dopo aver creato gli N  figli, esegue per 10 iterazioni le seguenti operazioni:
	- va in sleep per 2 secondi
	- genera un numero casuali r1 compreso tra 1 e 2 
	- genera un numero casuali r2 compreso tra 1 e N
	- invia il segnale SIGUSR<r1> al figlio P<r2>

In conclusione, il processo padre "uccide" i figli inviando SIGKILL
	 
Ogni processo figlio in un loop infinito
	- attende l'arrivo dei segnali SIGUSR1 o SIGUSR2
	- ogni volta che riceve uno dei due segnali stampa il proprio PID ed il segnale ricevuto
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// PROTOTIPI 
void signal_handler(int sig); // Funzione che si occpua di gestire i Segnali iniviati al Processo Figlio

// COSTANTI, VARIABILI GLOBALI
#define N 5

// MAIN
int main(void){
	pid_t figli[N];
	int result;
	int i;
	int r1;
	int r2;
	
	struct sigaction action;
	result = sigemptyset(&(action.sa_mask));
	if(result == -1){
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}
	action.sa_flags = 0;
	
	action.sa_handler = signal_handler;	
	
	result = sigaction(SIGUSR1, &action, NULL);
	if(result == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}	

	result = sigaction(SIGUSR2, &action, NULL);
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
				// Inizio Codice Processo Figlio					
				while(1){
					printf("\n\n[PROCESSO FIGLIO]: Sto aspettando l'arrivo del SEGNALE: SIGUSR1 o SIGUSR2");
					pause();
				}
				break;
				// Fine Codice Processo Figlio
			default:
				break;
		}
	}
	
	// Codice Processo Padre
	srand(getpid());
	for(i=0; i<10; i++){
		sleep(2);
		r1 = (rand()%(2-1+1))+1;
		r2 = (rand()%(N-1+1))+1;
		
		if(r1 == 1){
			result = kill(figli[r2-1],SIGUSR1);
	       	if(result == -1){
		    	perror("kill");
		    	exit(EXIT_FAILURE);
		    }
    	}
    	
		if(r1 == 2){
			result = kill(figli[r2-1],SIGUSR2);
	       	if(result == -1){
		    	perror("kill");
		    	exit(EXIT_FAILURE);
		    }
		}
	}
	
	for(int i=0; i<N; i++){	
		printf("\n[PROCESSO PADRE]: Sto inviando il SEGNALE: SIGKILL al Processo Figlio n° %d",i+1);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
	} 
}

// FUNZIONI
void signal_handler(int sig){
	int n = getpid();
	printf("\n[PROCESSO FIGLIO]: Il Mio PID e': %d",n);
	
	if(sig == SIGUSR1)
		printf("\n[PROCESSO FIGLIO]: Ho ricevuto il SEGNALE: SIGUSR1 dal Processo Padre.");
	                                                             
	if(sig == SIGUSR2)
		printf("\n[PROCESSO FIGLIO]: Ho ricevuto il SEGNALE: SIGUSR2 dal Processo Padre.");
}