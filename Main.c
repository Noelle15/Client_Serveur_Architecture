#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

//prototypes
int addition(char * donnees);
int soustraction(char * donnees);
int multiplication(char * donnees);
int puissance(char * donnees);
int distance(char * donnees);
int dist(int x1, int y1, int x2, int y2);
int matriceDistance(char * donnees);
int continuer();
int infoClientRAM(int pidServeur);
float infoClientTemps(int pidServeur);

//séparateur des données reçues est ';'
const char separators [1] = ";";
//nombre de coordonnées pour un point
const int coord = 2;

int main()
{
	//creation des pipes
	int p1[2];//pipe client -> serveur
	int p2[2];//pipe client <- serveur
	int p3[2];//pipe qui envoie le choix de l'opération au serveur
	int p4[4];//pipe pour envoyer la taille des donnees client(fils) au serveur (pere)	
	int p5[2];//pipe qui envoie les données du temps d'exec du serveur(pere) vers le client(fils)
	int p6[2];//pipe qui envoie les données de la RAM du serveur(pere) vers le client(fils)

	//variable d'identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	char donnees[100]; //données entrées par l'utilisateur 
	int tailleDonnees; //strlen(donnees) : nombre de caractère total avec les ;
	int tailleDonneesLuServer;
	int resultat;
	int choixOpe;
	int reponseClient = 0;
	float tempsEnSeconde = -1.0;
	int RAM = -1;

	//ouverture du fichier de résultat en écriture grâce à "w"
	FILE *fichierResultat;
	fichierResultat = fopen("res.txt" , "w" );
	
	//boucle pour faire plusieurs calculs à la suite
	do{
		pipe(p1);
		pipe(p2);
		pipe(p3); 
		pipe(p4);
		pipe(p5);
		pipe(p6);
		
		//creation du processus client
		pcs = fork();
		
		if (pcs == 0){ //client
				//ajout de la date dans le fichier de résultat
				if (fichierResultat != NULL) 
				{
					time_t now;
					time(&now); //renvoie l'heure actuelle
					//ecriture dans le fichier
					fprintf(fichierResultat,"\n%s",ctime(&now));
				}
				//demande du choix de l'operation et des données
				printf("Veuillez choisir votre opération suivi des donnees;\n");
				printf("[addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 : distance -> 5 : matriceDistance -> 6]\n");
				scanf(" %d",&choixOpe);
				scanf(" %s",donnees); 
				tailleDonnees = strlen(donnees);
				
				//ajout des données dans le fichier
				char donneesClient[100] = "Les données reçues par le client sont : ";
				strcat(donneesClient, donnees);
				if (fichierResultat != NULL) {
					fprintf(fichierResultat,"%s\n",donneesClient);
				}
				
				//le client est le pcs qui écrit dans le tube
				close(p4[0]);//ferme la lecture du pipe sur la taille des données
				close(p1[0]);//vérouille la lecture du père
				close(p2[1]);//ferme l'ecriture sur le pipe du père
				close(p3[0]);//ferme la lecture du pipe sur le choix de l'operation 
				close(p5[1]);//ferme l'ecriture sur le pipe du client(Temps d'exec)
				close(p6[1]);//ferme l'ecriture sur le pipe du client(RAM)
				
				write(p3[1],&choixOpe,sizeof(int));//envoie le choix de l'opération au serveur
				write(p4[1],&tailleDonnees,sizeof(int));//envoie la taille des donnees au serveur
				write(p1[1],donnees,tailleDonnees);//envoie les donnees brutes au serveur
				
				//lecture du resultat des calculs
				read(p2[0],&resultat,sizeof(int));
				
				//ajout du résultat dans le fichier resultat
				char resClient[100] = "Le résultat reçu par le client est : ";
				char resChar [10] = {0};
				sprintf(resChar, "%d", resultat);
				strcat(resClient, resChar);
				if (fichierResultat != NULL){
					fprintf(fichierResultat,"Opération n°%d\n",choixOpe);
					fprintf(fichierResultat,"%s\n",resClient);
				}

				//lecture des stats - temps d'execution
				read(p5[0],&tempsEnSeconde,sizeof(float));
				
				if(tempsEnSeconde != -1){
					char tempsExecChar[100] = "FIN DES CALCULS\nLe temps d'execution total : ";
					char TpsVal [50];
					sprintf(TpsVal, "%.2f", tempsEnSeconde);
					strcat(tempsExecChar, TpsVal);
					strcat(tempsExecChar, " s");
					printf("\n%s\n",tempsExecChar);
					
					if (fichierResultat != NULL) {
						fprintf(fichierResultat,"\n%s\n",tempsExecChar);
					}
				}
				
				//lecture des stats - taille des ressources (RAM)
				read(p6[0],&RAM,sizeof(int));
				
				if(RAM != -1) {
					char RAMChar[100] = "La taille des ressources utilisées : ";
					char RAMVal [50];
					sprintf(RAMVal, "%d", RAM);
					strcat(RAMChar, RAMVal);
					strcat(RAMChar, " ko");
					printf("%s\n",RAMChar);
					
					if (fichierResultat != NULL) {
						fprintf(fichierResultat,"%s\n",RAMChar);
					}
				}
				
				exit(0);
		}

		else{//serveur
			//le serveur lit le tube et envoie une réponse au client 
			close(p4[1]);//ferme l'ecriture dans le pipe de la taille des donnees
			close(p1[1]);//feme l'écriture du client
			close(p2[0]);//ferme la lecture du client
			close(p3[1]);//ferme l'ecriture dans le pipe du choix de l'operation
			close(p5[0]);//ferme la lecture sur le pipe du serveur(Temps d'exec)
			close(p6[0]);//ferme la lecture sur le pipe du serveur(RAM)			

			read(p3[0],&choixOpe,sizeof(int));//lit le choix de l'opération au serveur
			read(p4[0],&tailleDonneesLuServer,sizeof(int));//lit la taille des données
			read(p1[0],&donnees,tailleDonneesLuServer);//lit les donnees avec une taille de tailleDonneesLuServer			

			switch(choixOpe){
				case 1 : 
					resultat = addition(donnees);
					printf("Resultat de l'addition : %d\n", resultat);
				break;
				case 2 : 
					resultat = soustraction(donnees);
					printf("Resultat de la soustraction : %d\n", resultat);
				break;
				case 3 : 
					resultat = multiplication(donnees);
					printf("Resultat de la multiplication : %d\n", resultat);
				break;
				case 4 : 
					resultat = puissance(donnees);
					printf("Resultat du calcul de puissance : %d\n", resultat);
				break; 
				case 5 :				
					resultat = distance(donnees);
					printf("Resultat du calcul de distance : %d\n", resultat);
					//-1 représente une opération échouée
				break;
				case 6:
					resultat = matriceDistance(donnees);
					printf("Resultat du calcul de matrice de distance (O -> OK ; -1 -> Echec) :  %d\n", resultat);
					//-1 représente une opération échouée
				break;
				default:
					resultat = -1; //-1 représente une opération échouée
					printf("Resultat (échec) : %d\n",resultat);
				break;
			}
			
			//envoi du resultat au client
			write(p2[1],&resultat,sizeof(int));
			close(p2[1]);//fermer l'écriture du tube
			
			//envoi des stats si le client arrête le programme
			reponseClient = continuer();
			if(reponseClient != 0) {
				pid_t pid = getpid();
				int pidServeur = (int)pid;
				RAM = infoClientRAM(pidServeur);
				tempsEnSeconde = infoClientTemps(pidServeur);
			}

			//transmission des informations aux pipes
			write(p5[1],&tempsEnSeconde,sizeof(float));
			close(p5[1]);

			write(p6[1],&RAM,sizeof(int));
			close(p6[1]);

			wait(0);
		}
	}while(reponseClient == 0); //0 = continuer et autre = sortir
	
	//fermeture du fichier
	fclose(fichierResultat);
	
	return 0;
}

//calcul d'addition
int addition(char * donnees){
	int res = 0;
	//initialisation du parser
	char * strToken = strtok (donnees,separators);
	while ( strToken != NULL ) {
        res += atoi(strToken); //conversion en int
        //demande du token suivant.
        strToken = strtok ( NULL, separators );
    }
	return res;
}

//calcul de soustraction
int soustraction(char * donnees){
	int res;
	//initialisation du parser
	char * strToken = strtok (donnees,separators);
	//initialisation de res avec la première valeur
	res = atoi(strToken);
    //demande du token suivant.
    strToken = strtok ( NULL, separators );
	//réalisation de l'opération
	while ( strToken != NULL ) {
        res -= atoi(strToken);
        strToken = strtok ( NULL, separators );
    }
	return res;
}

//calcul de multiplication
int multiplication(char * donnees){
	int res = 1;
	char * strToken = strtok (donnees,separators);
	while ( strToken != NULL ) {
        res *= atoi(strToken);
        //demande du token suivant.
        strToken = strtok ( NULL, separators );
    }
	return res;
}

//calcul de puissance
int puissance(char * donnees){
	int res;	
	char * strToken = strtok (donnees,separators);
	res = atoi(strToken);
	//demande du token suivant.
	strToken = strtok ( NULL, separators );
	while ( strToken != NULL ) {
        res = pow(res,atoi(strToken));
        //demande du token suivant.
        strToken = strtok ( NULL, separators );
    }
	return res;
}

//calcul de distance
//Les quatre premiers nombres sont pris en compte, les autres sont ignorés
int distance(char * donnees){
	int res = 0;
	int cpt = 0;
	int xA, yA, xB, yB;	
	int tab[4];
	char * strToken = strtok (donnees,separators);
	
	while(cpt<4 && strToken!=NULL){
		cpt++;
		tab[cpt] = atoi(strToken);
		strToken = strtok ( NULL, separators );
	}
	if(cpt==4){
		xA = tab[0];
		yA = tab[1];
		xB = tab[2];
		yB = tab[3];

		res = sqrt(pow((xA-xB),2)+pow((yA-yB),2));
	}
	else{
		res = -1; // -1 représente une opération échouée
		printf("Le nombre de points fournis est inferieur à 4\n");
	}
	return res;
}

//sous-fonction utilisée pour la matrice de distance euclidienne
int dist(int x1, int y1, int x2, int y2){
	double distance = pow(x2 - x1, 2) + pow(y2 - y1, 2);
    distance = sqrt(distance);

    return (int)distance;
}

//calcul de la matrice de distance euclidienne
//retourne la somme des distances des points
int matriceDistance(char * donnees){
	int res = 0;
	int sum = 0;
	int i=0,j=0,k=0,cptPoints=0,x1,x2,y1,y2,nbPoint;

	//copie de la chaine pour faire strtok sans modifier de la chaîne de départ
	char donneesCopy[100] = {0};
	strcpy(donneesCopy,donnees);
	char * ptrDonneeCpy = donneesCopy;

	//création des tokens
	char * strToken;
	char * token;

	//comptage du nombre de coordonnées
	while((strToken = strtok_r(donnees,separators,&donnees))){
		cptPoints++;
	}

	//si le nombre de coordonnées est impaire alors on retourne -1
	if(cptPoints % 2 != 0){
		printf("Le nombre de coordonnées doit être pair\n");
		res = -1;
		return res;
	}

	//détermination du nombre de points donnés (un point a deux coordonnées x et y)
	nbPoint = cptPoints/2;
	//remplissage des tableaux pour calculer la matrice de distance
	int tabCoord[nbPoint][coord];
	int distances[nbPoint][nbPoint];
	int tabDonnees[nbPoint*2];	//tableau qui stockant les donnees
	int cptDonnees = 0;

	//remplissage du tableau des donnees
	while((token = strtok_r(ptrDonneeCpy,separators,&ptrDonneeCpy))){
		tabDonnees[cptDonnees] = atoi(token);
		cptDonnees++;
	}

	//remplissage du tableau de coordonnées
		for(i=0;i<nbPoint;i++){//ligne
			for(j=0;j<coord;j++){//colonne
				tabCoord[i][j] = tabDonnees[k];
				k++;
			}
		}

	//calcul des distances
	for(i=0;i<nbPoint;i++){
		for(j=0; j<nbPoint;j++){
			distances[i][j] = dist(tabCoord[i][0],tabCoord[i][1],tabCoord[j][0],tabCoord[j][1]);
		}
	}

	//affichage de la matrice
    for (i = 0; i < nbPoint; i++) //rows (lignes)
    {
        for (j = 0; j < nbPoint; j++) //cols (colonnes)
        {
            printf("%d ", distances[i][j]);
        }
        printf("\n");
    }

    //le resultat est 0 s'il n'y a pas de problème, sinon il est de -1
    return res;
}

//renvoie les informations de la capacité RAM
int infoClientRAM(int pidServeur){
	char chemin [50] = "/proc/";
	char pidChar[50];
	char fichierRAM[50] = "/statm";
	
	sprintf(pidChar, "%d", pidServeur);
	
	//chemin du fichier statm
	strcat(chemin,pidChar);
	strcat(chemin,fichierRAM);
	
	char fic;
	char infoRAM[10] = {0};
	int cpt = 0;
	int RamEntier;
    FILE *fp = fopen(chemin, "r"); //ouverture du fichier statm

    if (fp == NULL) {
        fprintf(stderr, "Non ouverture du fichier\n");
    } else {
		//récupération du premier nombre du fichier
        while ((fic = fgetc(fp)) != ' ') {
            infoRAM[cpt] = (fic);
			cpt++;
        }
		//conversion de la chaîne recuperees en int
        RamEntier = atoi(infoRAM);

        fclose(fp); //fermeture du fichier
    }
	return RamEntier;
}

//renvoie les informations du temps d'exécution
float infoClientTemps(int pidServeur){
	FILE* f;
    char path[255];
    sprintf(path, "/proc/%d/stat", pidServeur); //chemin
    f = fopen(path, "r"); //ouverture du fichier

    int values[4];
    int cpt = 0;
    char* t;
    char line[20000];
    fgets(line, 20000, f);
    int somme = 0;
    
    char * temps;
    temps = strtok (line," ");
    int c = 0;
    int i = 0;
	
	//récuperation des termes 14, 15, 16 et 17 du fichier stat
    while ((temps = strtok (NULL, " ")) != NULL)
    {
        if(c < 12) { //comptage des espaces pour arriver au bon niveau
            c++;
            continue;
        }
        values[i++] = atoi(temps);
        if(c > 15) break;
    }


    //calcul du temps total
    for(int i = 0; i < 4; ++i){
    	somme += values[i];
    }

    //tick to second - conversion du résultat en secondes
    const int tick = 100;
    float seconde;
    seconde = (float)somme/tick;

   	fclose(f); //fermeture du fichier

   	return seconde;
}

//demande au client s'il veut effectuer un autre traitement
int continuer(){
	char continuer = '\0';
	do{ //continuer ne doit pas être vide et doit correspondre à O ou N 
		printf("Voulez-vous continuer ? (O/N)\n");
		scanf(" %c",&continuer);
		continuer = (toupper(continuer)); //min et maj acceptés
	} while (continuer != 'O' && continuer !='N');
	
	if(continuer == 'N')
		return 1; //sortie du programme
	return 0; //reprise de la boucle depuis le début
}