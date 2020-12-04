#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>

//Prototypes
int addition(int x1, int x2);


int main()
{
	//les ports des pipes utilisés
	//p1[0] -> lecture du message du client par le serveur
	//p1[1] -> ecriture du client vers le serveur

	//p2[0] -> lecture du message du serveur par le client
	//p2[1] -> ecriture du serveur vers le client

	//creation des pipe
	int p1[2];
	int p2[2];
	pipe(p1);
	pipe(p2);	

	//variable identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	int valEntrees[2];
	int somme;

	//le nombre de caractères dans le mot
	char mot[30];

	//creation du processus client
	pcs = fork();

	if (pcs == 0){
		//demande les chiffres à additionner
		printf("Veuillez indiquer le 1er entier à additionner\n");
		scanf("%d",&valEntrees[0]);
		printf("Veuillez indiquer le 2nd entier à additionner\n");
		scanf("%d",&valEntrees[1]);


		//le client est le pcs qui écrit dans le tube
		close(p1[0]); // vérouille la lecture du père 
		close(p2[1]); //fermer l'ecriture sur le pipe du père
		write(p1[1],valEntrees,2*sizeof(int));//envoyer au serveur
		//sleep(2);//attendre 2 sec avant la lecture de la solution
		read(p2[0],&somme,sizeof(int));
		printf(" la somme renvoyé par le serveur: %d\n",somme);	
		exit(0);

	}
	else{
		//le serveur lit le tube et répond 
		close(p1[1]);//femer l'écriture du client
		close(p2[0]);//fermer la lecture du client
		read(p1[0],valEntrees,2*sizeof(int));
		somme = addition(valEntrees[0],valEntrees[1]);
		printf(" le client a dit: %d + %d\n",valEntrees[0],valEntrees[1]);
		write(p2[1],&somme,sizeof(int));
		close(p2[1]);//fermer l'écriture
		wait(0);
	}

	return 0;
}

int addition(int x1, int x2){
		return x1 + x2;
	}