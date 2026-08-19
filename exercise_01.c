/*
Scrivere un programma in C su Linux che emuli una struttura di 2 processi organizzati in modo master-slave.

Un processo padre (master) P0 genera 1 processo figlio (slave) P1

Il master
	- va in sleep per 1 secondo
	- autorizza lo slave alla trasmissione mediante invio di un segnale SIGUSR1
	- attende la ricezione del segnale SIGUSR2 dallo slave
	- quando il master riceve il segnale SIGUSR2 effettua una stampa a video 
	- "uccide" lo slave inviando SIGKILL
	 
Lo slave
	- attende l'autorizzazione alla trasmissione del master (ricezione del segnale SIGUSR1)
	- quando lo slave riceve il segnale SIGUSR1 effettua una stampa a video 
	- genera un numero causale r compreso tra 1 e 10
	- va in sleep per r secondi
	- invia un segnale di tipo SIGUSR2 al master
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

// PROTOTIPI
void master_signal_handler (int sig);	
void slave_signal_handler (int sig);

// MAIN
int main(void){
	int result;
	pid_t slave;
	
	struct sigaction action;
	result = sigemptyset(&(action.sa_mask));
	if(result == -1){
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}
	action.sa_flags = 0;
	 
	action.sa_handler = master_signal_handler;	
	
	slave = fork();
	switch(slave){
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			// Inizio Codice Processo Slave
			action.sa_handler = slave_signal_handler;
			
			result = sigaction(SIGUSR1, &action, NULL);
			if(result == -1){
				perror("sigaction");
				exit(EXIT_FAILURE);
			}
					
			printf("\n[PROCESSO SLAVE]: Sono in Attesa dell'arrivo del Segnale SIGUSR1");
			pause();				
			break;
			// Fine Codice Processo Slave
		default:
			break;
	}
	
	// Codice Processo Master 
	sleep(1);
	
	result = sigaction(SIGUSR2, &action, NULL);
	if(result == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	
	result = kill(slave, SIGUSR1);
	if(result == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
	
	printf("\n[PROCESSO MASTER]: Sono in Attesa dell'arrivo del Segnale SIGUSR2");
	pause();
	
	result = kill(slave, SIGKILL);	
    if(result == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
	printf("\n[PROCESSO MASTER]: Il Processo Slave e' Morto.");
}

// FUNZIONI
void slave_signal_handler(int sig){
	int r;
	int n;
	int result;
	
	if(sig == SIGUSR1){
		printf("\n[PROCESSO SLAVE]: Il Segnale SIGUSR1 e' Arrivato!");
	
		srand(time(NULL));
		r = (rand()%(10-1+1))+1;
	
		sleep(r);
		
		n = getppid();
		result = kill(n, SIGUSR2);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
	}
}

void master_signal_handler(int sig){
	int result;	
	
	if(sig == SIGUSR2){
		printf("\n[PROCESSO MASTER]: Il Segnale SIGUSR2 e' Arrivato!");
	}
}