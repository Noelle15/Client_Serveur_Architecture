#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

//Prototypes
int addition(char * donnees);
int soustraction(char * donnees);
int multiplication(char * donnees);
int puissance(char * donnees);
int distance(char * donnees);

//Le séparateur des données reçues est ';'
const char separators [1] = ";";
const char separatorDistance[1] = ",";

int main()
{
	//creation des pipe
	int p1[2];//Pipe client -> serveur
	int p2[2];//Pipe client <- serveur
	int p3[2];//Pipe qui envoie le choix de l'opération au serveur
	int p4[4];//pipe pour envoyer la taille des donnees client(fils) au serveur(pere)	

	pipe(p1);
	pipe(p2);
	pipe(p3); 
	pipe(p4);

	//variable d'identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	char donnees[100]; //Taille de la chaîne entrée par l'utilisateur (nombre de caractère total avec les ;)
	int tailleDonnees; //strlen(donnees)
	int tailleDonneesLuServer;
	int resultat;
	int produit;
	int choixOpe;

	//creation du processus client
	pcs = fork();

	if (pcs == 0){ //client
			do{
				printf("Veuillez choisir votre opération; [addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 : distance -> 5\n");
				scanf("%d",&choixOpe);
			}while(choixOpe > 5 || choixOpe < 1);
			
			printf("Veuillez les valeurs à calculer separees de ;\n");
			if(choixOpe = 5)
				printf("l'ordre des valeurs : xA;yA;xB;yB (les valeurs en plus seront ignorées)\n");
			scanf("%s",donnees); //5;4;4;6;9
			
			printf("Les données entrées: %s\n",donnees);
			tailleDonnees = strlen(donnees);
			printf("La taille des données entrées: %d\n",tailleDonnees);
			
			//le client est le pcs qui écrit dans le tube
			close(p4[0]);//fermer la lecture du pipe sur la taille des données
			close(p1[0]); // vérouille la lecture du père 
			close(p2[1]); //fermer l'ecriture sur le pipe du père
			close(p3[0]);//fermer la lecture du pipe sur le choix de l'operation 
			
			printf("envoie le choix de l'operation au serveur");
			write(p3[1],&choixOpe,sizeof(int));
			printf("envoyer la taille des donnees au serveur");
			write(p4[1],&tailleDonnees,sizeof(int));//envoyer la taille des donnees au serveur
			write(p1[1],donnees,tailleDonnees);//envoyer les donnees brutes au serveur
			
			read(p2[0],&resultat,sizeof(int));
			printf(" le resultat renvoyé par le serveur: %d\n",resultat);

			exit(0);
		}

	else{
		//le serveur lit le tube et répond 
		close(p4[1]);//fermer l'ecriture dans le pipe de la taille des donnees
		close(p1[1]);//femer l'écriture du client
		close(p2[0]);//fermer la lecture du client
		close(p3[1]);//fermer l'ecriture dans le pipe du choix de l'operation 
		
		read(p3[0],&choixOpe,sizeof(int));
		read(p4[0],&tailleDonneesLuServer,sizeof(int));// on lit la taille des données
		read(p1[0],&donnees,tailleDonneesLuServer);//on lit ce qui se trouve dans le tube p1 et on le met dans donnees avec une taille de tailleDonneesLuServer

		switch(choixOpe){
			case 1 : 
				printf("partie addition");
				resultat = addition(donnees);
				printf("Le resultat: %d\n",resultat );
			break;
			case 2 : 
				resultat = soustraction(donnees);
				printf("Le resultat: %d\n",resultat );
			break;
			case 3 : 
				resultat = multiplication(donnees);
				printf("Le resultat: %d\n",resultat );
			break;
			case 4 : 
				resultat = puissance(donnees);
				printf("Le resultat: %d\n",resultat );
			break; 
			case 5 :
				resultat = distance(donnees);
				printf("Le resultat : %d\n", resultat);
			break;

		}

		printf(" le server confirme que le client a dit: %s",donnees);
		write(p2[1],&resultat,sizeof(int));
		close(p2[1]);//fermer l'écriture
		wait(0);
	}

	return 0;
}

int addition(char * donnees){
	int res = 0;
	//initialisation du parser
	char * strToken = strtok (donnees,separators);
	while ( strToken != NULL ) {
        res += atoi(strToken); //conversion en int
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
    }
    printf("ADDITION: %d\n", res );
	return res;
}

int soustraction(char * donnees){
	int res;
	//initialisation du parser
	char * strToken = strtok (donnees,separators);
	//Initialisation de res avec la première valeur
	res = atoi(strToken);
    // On demande le token suivant.
    strToken = strtok ( NULL, separators );
	//Réalisation de l'opération
	while ( strToken != NULL ) {
        res -= atoi(strToken);
        strToken = strtok ( NULL, separators );
    }
    printf("SOUSTRACTION: %d\n", res );
	return res;
}

int multiplication(char * donnees){
	int res = 1;
	char * strToken = strtok (donnees,separators);
	while ( strToken != NULL ) {
        res *= atoi(strToken);
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
    }
    printf("MULTIPLICATION: %d\n", res );
	return res;
}

int puissance(char * donnees){
	int res;	
	char * strToken = strtok (donnees,separators);
	res = atoi(strToken);
	// On demande le token suivant.
	strToken = strtok ( NULL, separators );
	while ( strToken != NULL ) {
        res = pow(res,atoi(strToken));
        // On demande le token suivant.
        strToken = strtok ( NULL, separators );
    }
    printf("PUISSANCE: %d\n", res );
	return res;
}

int distance(char * donnees){
	int res;
	int xA, yA, xB, yB;	
	char * strToken = strtok (donnees,separators);
	xA = atoi(strToken);
	strToken = strtok ( NULL, separators );
	yA = atoi(strToken);
	strToken = strtok ( NULL, separators );
	xB = atoi(strToken);
	strToken = strtok ( NULL, separators );
	yB = atoi(strToken);
	
	res = sqrt(pow((xA-xB),2)+pow((yA-yB),2));
	
    printf("DISTANCE: %d\n", res );
	return res;
}