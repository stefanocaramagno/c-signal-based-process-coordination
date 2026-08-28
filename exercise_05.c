/*
Scrivere un programma in C su Linux in cui un processo padre P0 genera un processo figlio P1
• Il processo figlio (P1) deve gestire 2 segnali (SIGUSR1 e SIGQUIT). 
  P1 ciclicamente attende un segnale.
  • Se il segnale ricevuto è SIGUSR1, P1 stampa a video la stringa "Ping\n" e invia un segnale di tipo SIGUSR1 al processo padre (P0)
  • Se il segnale ricevuto è SIGQUIT, P1 stampa a video la stringa "Bye!" e termina.
  
• Il processo padre (P0) dopo aver generato il processo figlio:
  • Attende 1 secondo
  • Imposta un gestore del segnale SIGUSR1 per far si che alla ricezione di tale segnale, P0 stampa a video la stringa "Pong\n".
  • Invia un segnale SIGUSR1 al processo figlio.
  • Si mette ciclicamente per 5 volte in attesa di ricevere un segnale, all'arrivo P1 attende 1 secondo e invia al processo figlio (P1) un segnale SIGUSR1
  • Dopo 5 volte che ha ricevuto il segnale SIGUSR1, P0 invia il segnale SIGQUIT al processo figlio.
  • Attende la terminazione del processo figlio e termina esso stesso
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// PROTOTIPI
void signal_handler(int sig); // Funzione che si occupa di Gestire i Segnali inviati al Processo Padre
void child_signal_handler(int sig); // Funzione che si occupa di Gestire i Segnali inviati al Processo Figlio

// COSTANTI
#define N 5

// MAIN
int main(void){
	pid_t figlio;
	int result;
	int i;
	
	struct sigaction action;
	result = sigemptyset(&(action.sa_mask));
	if(result == -1){
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}
	action.sa_flags = 0;
	
	action.sa_handler = signal_handler;	
	
	figlio = fork();
	switch(figlio){
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			// Inizio Codice Processo Figlio
			action.sa_handler = child_signal_handler;
			
			result = sigaction(SIGUSR1, &action, NULL);
			if(result == -1){
				perror("sigaction");
				exit(EXIT_FAILURE);
			}
			
			result = sigaction(SIGQUIT, &action, NULL);
			if(result == -1){
				perror("sigaction");
				exit(EXIT_FAILURE);
			}	
					
			while(1){
				pause();
			}
			exit(0);
			// Fine Codice Processo Figlio	
		default:
			break;
	}
	
	// Codice Processo Padre
	result = sigaction(SIGUSR1, &action, NULL);
	if(result == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}	
	
	for(i=0; i<N; i++){
		sleep(1);
		result = kill(figlio, SIGUSR1);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}	
		pause();	
	}
	
	result = kill(figlio, SIGQUIT);
	if(result == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
	
	wait(NULL);
}

// FUNZIONI
void signal_handler(int sig){
	int result;
	if(sig == SIGUSR1){
		printf("\n[PROCESSO PADRE]: Pong.");
	}
}

void child_signal_handler(int sig){
	int result;
	int n;
	
	if(sig == SIGUSR1){
		printf("\n[PROCESSO FIGLIO]: Ping.");
		
		sleep(1);
		n = getppid();
		result = kill(n, SIGUSR1);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}			
	}
	
	if(sig == SIGQUIT){
		printf("\n[PROCESSO FIGLIO]: Bye.\n");
	}
}

