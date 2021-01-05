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
int dist(int x1, int y1, int x2, int y2);
int matriceDistance(char * donnees);
int continuer();
void infoClientRAM(int pidServeur);
void infoClientTemps(int pidServeur);

//Le séparateur des données reçues est ';'
const char separators [1] = ";";
//représente le nombre de coordonnées pour un point
const int coord = 2;

int main()
{
	
	printf("another loop");
	//creation des pipes
	int p1[2];//Pipe client -> serveur
	int p2[2];//Pipe client <- serveur
	int p3[2];//Pipe qui envoie le choix de l'opération au serveur
	int p4[4];//pipe pour envoyer la taille des donnees client(fils) au serveur(pere)	

	//variable d'identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	char donnees[100]; //Taille de la chaîne entrée par l'utilisateur (nombre de caractère total avec les ;)
	int tailleDonnees; //strlen(donnees)
	int tailleDonneesLuServer;
	int resultat;
	int produit;
	int choixOpe;
	int reponseClient = 0;
	
	do{
		
		pipe(p1);
		pipe(p2);
		pipe(p3); 
		pipe(p4);
		
		//creation du processus client
		pcs = fork();
		
		if (pcs == 0){ //client
				do{
					//GERER LE CAS oU oN TAPE UN MAUVAIS CHOIX COMME 1;
					printf("Veuillez choisir votre opération suivi des donnees;\n");
					printf("[addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 : distance -> 5 : matriceDistance -> 6]\n");
					scanf("%d",&choixOpe); //5;4;4;6;9
				}while(choixOpe > 6 || choixOpe < 1);
				
				scanf(" %s",donnees); 
				
				printf("Les données entrées: %s\n",donnees);
				tailleDonnees = strlen(donnees);
				printf("La taille des données entrées: %d\n",tailleDonnees);
				
				//le client est le pcs qui écrit dans le tube
				close(p4[0]);//fermer la lecture du pipe sur la taille des données
				close(p1[0]); // vérouille la lecture du père 
				close(p2[1]); //fermer l'ecriture sur le pipe du père
				close(p3[0]);//fermer la lecture du pipe sur le choix de l'operation 
				
				write(p3[1],&choixOpe,sizeof(int));
				write(p4[1],&tailleDonnees,sizeof(int));//envoyer la taille des donnees au serveur
				write(p1[1],donnees,tailleDonnees);//envoyer les donnees brutes au serveur
				
				read(p2[0],&resultat,sizeof(int));

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
			printf("La taille des données reçue: %d\n", tailleDonneesLuServer);
			read(p1[0],&donnees,tailleDonneesLuServer);//on lit ce qui se trouve dans le tube p1 et on le met dans donnees avec une taille de tailleDonneesLuServer
			printf("Les données reçues sont: %s\n", donnees );

			switch(choixOpe){
				case 1 : 
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
					printf("Le resultat: %d\n",resultat );	
				break;
				case 6:
					resultat = matriceDistance(donnees);
					printf("Resultat du calcul de matrice de distance:  %d\n", resultat);
					//on dit, resultat = 0 sinon -1 si echec
				break;
				default:
					resultat = 0;
					printf("Opération non reconnue. Resultat: %d\n",resultat);
				break;
			}
			
			write(p2[1],&resultat,sizeof(int));
			close(p2[1]);//fermer l'écriture
			
			//affichage des informations si le client arrête le programme
			reponseClient = continuer();
			if(reponseClient != 0) {
				pid_t pid = getpid();
				int pidServeur = (int)pid;
				printf("pid serveur %d",pidServeur);
				infoClientRAM(pidServeur);
				infoClientTemps(pidServeur);
				//transmettre à pipe
			}
			wait(0);
		}
	}while(reponseClient == 0); //0 = continuer et autre = sortir
	
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
	int res = 0;
	int cpt = 0;
	int xA, yA, xB, yB;	
	int tab[4];
	char * strToken = strtok (donnees,separators);
	for(int c = 0; c<4; c++){
		printf(" Le tableau avant la boucle: %d\n", tab[c]);
	}
	while(cpt<4 && strToken!=NULL){
		cpt++;
		tab[cpt] = atoi(strToken);
		printf("Le contenu de tab: ----> %d\n",tab[cpt] );
		strToken = strtok ( NULL, separators );
	}
	if(cpt==4){
		xA = tab[0];
		yA = tab[1];
		xB = tab[2];
		yB = tab[3];

		res = sqrt(pow((xA-xB),2)+pow((yA-yB),2));
		printf("DISTANCE: %d\n", res);
	}
	else{
		printf("Le nombre de points fourni est inferieur à 4: Resultat:  %d\n", res);
	}

	return res;
}

//utilisé pour clalculer la matrice de distance euclidienne
int dist(int x1, int y1, int x2, int y2){
	double distance = pow(x2 - x1, 2) + pow(y2 - y1, 2);
    distance = sqrt(distance);

    return (int)distance;
}

//calul de la matrice de distance euclidienne -> retourne la somme des distances des points
int matriceDistance(char * donnees){
	int res = 0;
	int sum = 0;
	int i=0,j=0,k=0,cptPoints=0,x1,x2,y1,y2,nbPoint;

	//copier la chaine de caractère car strtok modifie directement
	char donneesCopy[100] = {0};
	strcpy(donneesCopy,donnees);
	char * ptrDonneeCpy = donneesCopy;

	//création des tokens
	char * strToken;
	char * token;

	//pour compter le nombre de coordonnées
	while((strToken = strtok_r(donnees,separators,&donnees))){
		cptPoints++;
	}

	//si le nombre de coordonnées est impaire alors on retourne -1
	if(cptPoints % 2 != 0){
		printf("Le nombre de coordonnées doit être pair\n");
		res = -1;
		return res;
	}

	//détermination du nb de points donné
	nbPoint = cptPoints/2;
	//remplissage des tableaux pour calculer la matrice de distance
	int tabCoord[nbPoint][coord];
	int distances[nbPoint][nbPoint];
	//tableau qui va stocker les donnees
	int tabDonnees[nbPoint*2];
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

	//afficher la matrice
    for (i = 0; i < nbPoint; i++) //rows
    {
        for (j = 0; j < nbPoint; j++) //cols
        {
            printf("%d ", distances[i][j]);
        }
        printf("\n");
    }

    //si tout va bien le resultat est 0 sinon c'est -1
    return res;
}

//renvoie les informations de la capacité RAM
void infoClientRAM(int pidServeur){
	//int[] info = {0,1};
	char chemin [50] = "/proc/";
	char pidChar[50];
	char fichierRAM[50] = "/statm";
	
	sprintf(pidChar, "%d", pidServeur);
	printf("--------------- %s",pidChar);
	
	//chemin du fichier statm
	strcat(chemin,pidChar);
	strcat(chemin,fichierRAM);
	printf("\n%s",chemin);
	
	char fic;
	char infoRAM[10] = {0};
	int cpt = 0;
    FILE *fp = fopen(chemin, "r");

    if (fp == NULL) {
        fprintf(stderr, "Non ouverture du fichier\n");
    } else {
		
        while ((fic = fgetc(fp)) != ' ') {
            infoRAM[cpt] = fic;
			cpt++;
        }
		printf("\ninfoRAM = %s",infoRAM);
        fclose(fp);
    }
	
	
}

//renvoie les informations du temps d'exécution
void infoClientTemps(int pidServeur){

	int
	
	FILE* f;
    char path[255];
    sprintf(path, "/proc/%d/stat", pidServeur);
    f = fopen(path, "r");

    if(f== NULL) printf("lol");

    long int values[4];
    int cpt = 0;
    char* t;
    char line[20000];
    fgets(line, 20000, f);
    if(line != NULL) {
        printf("%s", line);
    }
    
    char * pch;
    pch = strtok (line," ");
    int c = 0;
    int i = 0;
    while ((pch = strtok (NULL, " ")) != NULL)
    {
        if(c < 12) {
            c++;
            continue;
        }
        values[i++] = strtol(pch, NULL, 10);
        if(c > 15) break;
    }

    for(int i = 0; i < 4; ++i)printf(" :: %ld\n", values[i]);

   	fclose(f);
}

int continuer(){
	char continuer = '\0';
	do{
		printf("Voulez-vous continuer ? (O/N)\n");
		scanf(" %c",&continuer);
		continuer = (toupper(continuer));
	} while (continuer != 'O' && continuer !='N'); //continuer ne doit pas être vide
	
	if(continuer == 'N')
		return 1;
	return 0;
}