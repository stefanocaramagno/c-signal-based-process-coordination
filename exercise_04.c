/*
Scrivere un programma in C su Linux in cui un processo P0 genera cinque processi figli.

Il processo padre dopo aver generato tutti i processi figli, ciclicamente e per 5 iterazione:
- invia un segnale di tipo SIGUSR1 ad ogni figlio.
- va in sleep per 1 secondo
Dopo 5 iterazioni P0 invia a tutti i processi figli il segnale SIQUIT, ne attende la terminazione e termina esso stesso.

Ogni processo figlio, ciclicamente:
- Attende l'arrivo di un segnale,
  - se il segnale è SIGQUIT il processo figlio termina.
  - se il segnale è SIGUSR1:
       - se il figlio ha PID pari, stampa a video il testo "Pari" ed il proprio PID, 
       - se il figlio ha un PID dispari, stampa a video il testo "Dispari" ed il proprio PID.

I Processi figli non devono mai terminare prima di aver ricevuto il segnale SIGQUIT.

Far si che l'esecuzione del gestore del segnale SIGUSR1 non possa essere interrotta dall'arrivo del segnale SIGQUIT.
*/

// LIBRERIE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// COSTANTI
#define N 5

// PROTOTIPI 
void signal_handler(int sig); // Funzione che si occupa di Gestori i Segnali del Processo Figlio

// MAIN
int main(void){
	pid_t figli[N];
	int i;
	int n;
	int result;

    /* 
	  La struttura di tipo Sigaction viene definita nel Processo Padre, prima della fork().
      I Processi Figli ereditano:
        - Una Copia della Struct Sigaction (stessi valori iniziali, che però possono essere cambiati).
        - Lo stesso comportamento dei Segnali impostato dal Padre (ad esempio: stessa Signal_Handler, che ovviamente può essere cambiato)
	*/  
		
	struct sigaction action;
	result = sigemptyset(&(action.sa_mask));
	if(result == -1){
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}
	action.sa_flags = 0;
	
    /*
    La funzione Signal Handeler è definita nel Figlio di solito, ma ci possono essere problemi, 
	se il figlio viene eseguito dopo che il padre invoca la kill().
	Quindì in questo caso la soluzione migliore è settare la Sigaction che serve al figlio prima della creazione di esso stesso.
	Questa soluzione potrebbe essere adottata sempre, al di là del problema della Kill().
	*/	
	
	action.sa_handler = signal_handler;
	
	result = sigaction(SIGUSR1, &action, NULL);
	if(result == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	
	for(i=0; i<N; i++){
		figli[i] = fork();
		switch(figli[i]){
			case -1:
				perror("Errore fork()");
				exit(EXIT_FAILURE);
			case 0:
				// Inizio Codice Processo Figlio
				pause();
				exit(0);
				// Fine Codice Processo Figlio				
			default:
				break;	
		}
	}
	
	// Codice Processo Padre
	for(i=0; i<5; i++){
		result = kill(figli[i], SIGUSR1);
		if(result == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
		sleep(1);
	}
	for(i=0; i<5; i++){	
	    result = kill(figli[i], SIGQUIT);
	    if(result == -1){
	    	perror("kill");
	    	exit(EXIT_FAILURE);
		}
	}
}

// FUNZIONI
void signal_handler(int sig){
	int n;
	if(sig == SIGUSR1){
		n = getpid();
		if(n%2 == 0){
			printf("\nPARI. Il Pid del Processo Figlio e': %d",n);
		}else{
			printf("\nDISPARI. Il Pid del Processo Figlio e': %d",n);
		}
	}
	if(sig == SIGQUIT){
		printf("Il Figlio Termina QUI.");
	}
}