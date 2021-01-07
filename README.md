### 1. Membres du groupe:

VERMA Noëlle:  [verma.noelle@gmail.com](mailto:verma.noelle@gmail.com)

RABEARISOA VOLOLONIAINA Léïya Océane: [leiya.rabe@gmail.com](mailto:leiya.rabe@gmail.com)

### 2. Description

Notre programme est une implémentation d'une calculatrice avec une architecture client/serveur.  La calculatrice peut effectuer **6 opérations.**

1. Addition
2. Soustraction
3. Multiplication
4. Exponentiation (**bonus**)
5. Calcul de la distance entre les 2 points
6. Calcul de la matrice de distance entre plusieurs points

Les données, avec le choix de l'opération, sont envoyées par le client au serveur.

Le résultat de l'opération choisi est lui renvoyé par le serveur vers le client.

Les logs (la date, les données, le choix de l'opération et les résultats) sont également stockés dans un fichier text qui se situe dans le même répertoire que le programme. (**bonus)**

### 3. Architecture

L'architecture utilisé dans le programme est celui du client/server. Pour cela, nous avons utilisé  **2 processus**, un qui sera le client et un autre qui sera le serveur. Le programme principale tourne dans une boucle infini qui ne s'arrête que lorsque le client décide d'arrêter.

- Afin de créer les processus nous avons utilisé **un fork.**

Client→ le processus fils 

```c
pid_t pcs;
pcs = fork();
if (pcs == 0){ //client envoie les données
...}
```

Serveur → le processus père

```c
else{
			//le serveur lit le tube et répond
...}
```

- Transfert des données entre le client et le serveur

Les données sont passées entre les processus par le biais des tubes anonymes: **pipe**

```c
//creation des pipes
	int p1[2];//pipe client -> serveur
	int p2[2];//pipe client <- serveur
	int p3[2];//pipe qui envoie le choix de l'opération au serveur
	int p4[4];//pipe pour envoyer la taille des donnees client(fils) au serveur (pere)	
	int p5[2];//pipe qui envoie les données du temps d'exec du serveur(pere) vers le client(fils)
	int p6[2];//pipe qui envoie les données de la RAM du serveur(pere) vers le client(fils)

```

- Le choix des opérations

Les opérations sont prédéfinies, ainsi l'utilisateur n'a qu'à écrire l'entier correspondant à l'opération souhaité. L'entier qui représente l'opération est passé avec les données par le client, ensuite le serveur récupère ce dernier et selon l'entier entré il effectuera l'opération correspondante et renverra le resultat.

```c
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
```

- Parsage des données

Les données sont envoyées par le client au serveur et elles sont composées d'une succession d'entier séparée par des **;** 

Le premier entier représente l'opération, et le reste les données pour effectuer l'opération.

> Exemple: 4;5;2 → 5^2 ou encore 1;2;3;4;5 → 2+3+4+5

Le parsage des données se fait grâce à la fonction **strtok** qui va se charger de la récupération des données séparées de délimiteurs. 

- Les opérations

Les opérations sont des fonctions qui sont executées suivant le choix de l'opération de l'utilisateur. Elles prennent les données en paramètre afin de les parser et ensuite retourne le résultat de l'opération qui sera à son tour envoyé au client par le biais d'un pipe dédié.

```c
//Exemple d'opération
int puissance(char * donnees){
	int res;	
	char * strToken = strtok (donnees,separators);
	res = atoi(strToken);
	// On demande le token suivant.
	strToken = strtok ( NULL, separators );
	while ( strToken != NULL ) {
		res = pow(res,atoi(strToken));
		//demande du token suivant.
		strToken = strtok ( NULL, separators );
        }
	return res;
}
```

- Les logs: données envoyées/reçues, opérations et résultats

Les logs sont stockés dans un fichier au fur et à mesure de l'exécution du programme. Ils prennent en compte les données envoyées par le client et celles reçues par le serveur, ensuite le résultat de l'opération. Le fichier de log comporte également la date et l'heure de l'exécution du traitement. 

- Les informations système.

L'usage de la RAM et le temps d'exécution du programme sont récupérés par le serveur et ensuite placés dans deux pipes pour être envoyés au client à la fin du programme. Pour trouver cela il a fallu lire deux fichiers dans le dossier /proc/pid avec le pid du processus père (serveur).

RAM: se trouve dans le fichier /statm et correspond à la taille des ressources utilisées pendant le déroulement du programme. Il s'agit de la première valeur dans le fichier.

Le temps d'exécution: se trouve dans le fichier /stat  et il a fallu récupérer les valeurs qui se trouvaient de la 14ème jusqu'à la 17ème position dans le fichier. L'addition de ces valeurs représente le temps total d'exécution, en tick. Pour avoir le temps en seconde, on divise le tout par 100 qui est le nombre de tick par seconde.

### 4. Utilisation

Pour lancer le programme il est nécessaire de le compiler: l'option -lm sert à utiliser la librairie math

```
gcc -o main Main.c -lm
```

Puis pour l'exécuter, il faut lancer la commande suivante :

```
./main
```

Ensuite, il faut passer les données:

```bash
Veuillez choisir votre opération suivi des donnees;
[addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 : distance -> 5 : matriceDistance -> 6]
```

Pour les données il est  **impératif** de terminer par un entier et non une espace ni un autre 

> ✅ 1;2;3;4;5

> ❗️ 1;2;3;4;

Pour les données à passer pour l'opération 6 ou le calcul de la matrice de distance euclidienne il est nécessaire de comprendre que ça va prendre plusieurs points en paramètre, un point est représenté par 2 entiers (x;y) ainsi il faut que le nombre de données rentré soit cohérent, c'est à dire un nombre pair d'entier sans compter le premier entier qui représente l'opération. 

> ✅ 6;1;2;3;4;5;6;7;8;9;10

> ❗️ 6;1;2;3;4;5;6;7;8;9

Pour l'opération de l'exponentiation, le premier entier représente l'opération, le deuxième la base, le troisième l'exposant et si il y en a d'autres ils seront considérés comme exposant du résultat précédent.

> Exemple: 4;5;2 → 5^2 = 25 et 4;5;2;2 → (5^2)^2 → 25^2 → 625

### 5. Remarques

Il faut noter que les calculs s'exécutent rapidement, donc pour avoir un temps d'exécution qui est supérieur à 0 il faut faire plusieurs opérations consécutives.
