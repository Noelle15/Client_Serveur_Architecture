#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

//Prototypes
int addition(int x1, int x2);
int soustraction(int x1, int x2);
int multiplication(int x1, int x2);
int puissance(int x1, int x2);


int main()
{
	//les ports des pipes utilisés pour l'addition
	//p1[0] -> lecture du message du client par le serveur
	//p1[1] -> ecriture du client vers le serveur

	//p2[0] -> lecture du message du serveur par le client
	//p2[1] -> ecriture du serveur vers le client

	//Les ports des pipes pour la multiplication 
	//p3[0] -> lecture du message du client par le serveur
	//p3[1] -> ecriture du client vers le serveur

	//p4[0] -> lecture du message du serveur par le client
	//p4[1] -> ecriture du serveur vers le client

	//creation des pipe
	int p1[2];
	int p2[2];
	int p3[2];

	pipe(p1);
	pipe(p2);
	pipe(p3); //Pipe qui envoie le choix de l'opération au serveur	


	//variable identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	int valEntrees[2];
	int resultat;
	int produit;
	int choixOpe;
	char ope;

	//le nombre de caractères dans le mot
	char mot[30];

	//creation du processus client
	pcs = fork();

	if (pcs == 0){
		//demande les chiffres à additionner
		do{
			printf("Veuillez choisir votre opération; [addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 \n");
			scanf("%d",&choixOpe);
		}while(choixOpe > 4 || choixOpe < 1);
		

		printf("Veuillez indiquer le 1er entier\n");
		scanf("%d",&valEntrees[0]);
		printf("Veuillez indiquer le 2nd entier\n");
		scanf("%d",&valEntrees[1]);

			//------ Addition ---------//
		//le client est le pcs qui écrit dans le tube
		close(p1[0]); // vérouille la lecture du père 
		close(p2[1]); //fermer l'ecriture sur le pipe du père
		close(p3[0]);//fermer la lecture du pipe sur le choix de l'operation 

		write(p3[1],&choixOpe,sizeof(int));
		write(p1[1],valEntrees,2*sizeof(int));//envoyer au serveur
		
		read(p2[0],&resultat,sizeof(int));
		printf(" le resultat renvoyé par le serveur: %d\n",resultat);	
		exit(0);

	}
	else{
		//le serveur lit le tube et répond 
		close(p1[1]);//femer l'écriture du client
		close(p2[0]);//fermer la lecture du client
		close(p3[1]);//fermer l'ecriture dans le pipe du choix de l'operation 

		read(p3[0],&choixOpe,sizeof(int));
		read(p1[0],valEntrees,2*sizeof(int));

		switch(choixOpe){
			case 1 : 
				resultat = addition(valEntrees[0],valEntrees[1]);
				printf("Le resultat: %d\n",resultat );
				ope = '+';
			break;
			case 2 : 
				resultat = soustraction(valEntrees[0],valEntrees[1]);
				printf("Le resultat: %d\n",resultat );
				ope = '-';
			break;
			case 3 : 
				resultat = multiplication(valEntrees[0],valEntrees[1]);
				printf("Le resultat: %d\n",resultat );
				ope = '*';
			break;
			case 4 : 
				resultat = puissance(valEntrees[0],valEntrees[1]);
				printf("Le resultat: %d\n",resultat );
				ope = '^';
			break; 

		}

		printf(" le client a dit: %d %c %d\n",valEntrees[0],ope,valEntrees[1]);
		write(p2[1],&resultat,sizeof(int));
		close(p2[1]);//fermer l'écriture
		wait(0);
	}

	return 0;
}

int addition(int x1, int x2){
	printf("Le resultat: %d\n",(x1 + x2) );
	return x1 + x2;
}

int soustraction(int x1, int x2){
	printf("Le resultat: %d\n",(x1 - x2) );
	return x1 - x2;
}

int multiplication(int x1, int x2){
	printf("Le resultat: %d\n",(x1 * x2) );
	return x1 * x2;
}

int puissance(int x1, int x2){
	printf("Le resultat: %f\n",pow(x1,x2));
	return (int)pow(x1,x2);
}