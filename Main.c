#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	//les ports des pipes utilisés
	//p1[0] -> lecture du message du fils par le père
	//p1[1] -> ecriture du fils vers le père
	//p2[0] -> lecture du message du père par le fils
	//p2[0] -> ecriture du père vers le fils

	//creation des pipe
	int p1[2];
	int p2[2];
	pipe(p1);
	pipe(p2);	
	//variable identification processus
	pid_t pcs;

	//le nombre de caractères dans le mot
	char mot[30];

	//creation du processus fils
	pcs = fork();

	if (pcs == 0){
		//le fils est le pcs qui écrit dans le tube
		close(p1[0]); // vérouille la lecture du pipe 1 
		close(p2[1]); //fermer l'ecriture sur le pipe 2
		write(p1[1],"hello, je suis le processus A",30);//envoyer au père
		//sleep(2);//attendre 2 sec avant la lecture du msg du père
		read(p2[0],mot,30);
		printf(" le pere a dit: %s\n",mot);	
		exit(0);

	}
	else{
		//le père lit le tube et répond 
		close(p1[1]);//femer l'écriture
		close(p2[0]);
		read(p1[0],mot,30);
		printf(" le fils a dit: %s\n",mot);
		write(p2[1],"hello, je suis le processus B",30);
		close(p2[1]);//fermer l'écriture
		wait(0);
	}

	return 0;
}