
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "backgammon.h"
#include "possibles.h"
#include "evaluation.h"

#define VERBEUX 0

FILE *global_ressource;
plateaux_joues *global_jeu1=NULL, *global_jeu2=NULL;
decatree arbre;

void InitLibrary(char name[50])
{
	strcpy(name,"Bertrand \\0/");
	
	global_ressource = fopen("/AI_fast/fuuu.bg","rwb+");

	if(global_ressource == NULL)
	{
		printf("Pas de fichier trouvé, création d'un nouveau\n");
		global_ressource = fopen("/AI_fast/fuuu.bg","wb");
	}
	else printf("fichier de ressource correctement trouvé\n");
	
}

void StartMatch(const unsigned int target_score)
{
	/*global_ressource = fopen("AI/ressource.bg","r+");

	if(global_ressource == NULL)
	{
		printf("Pas de fichier trouvé, création d'un nouveau\n");
		global_ressource = fopen("AI/ressource.bg","w");
	}
	else printf("fichier de ressource correctement trouvé\n");
	*/
}

void StartGame()
{
}

void EndGame()
{
	plateaux_joues *courant;
	EPlayer gagnant;
	decatree *lol=NULL;

	time_t debut;
	
	float note;
	int sens;
	
	debut=time(NULL);
	
	/*Pour mémoire J1 contient le dernier plateau mémorisé
	donc soit j'ai rangé tous mes pions et l'arbitre a appelé EndGame()
	juste aprés et donc j'ai gagné, soit je n'ai pas finit et l'appel
	a été fait suite à la vistoire du mauvais d'en face.*/
	if(global_jeu1->plateau[EPos_OutP1].nb_checkers == 15) gagnant=EPlayer1;
	else gagnant=EPlayer2;
	
	lol = create_decatree(global_ressource);
	
	courant = global_jeu1;
	while(courant != NULL)
	{
		if(existe2(lol,courant->plateau,&note,&sens) != existe(global_ressource,courant->plateau,&note,&sens))
			printf("___ ERREUR __");
		
	}
	
	/*courant = global_jeu2;
	while(courant != NULL)
	{
		add_plateau(global_ressource,courant->plateau,gagnant);
		courant = courant->suivant;
	}
	*/
	
	fflush(global_ressource);
	
	free_memorise(global_jeu1);
	free_memorise(global_jeu2);
	
	global_jeu1 = NULL;
	global_jeu2 = NULL;
	
	printf("%ld secondes pour le learning\n",time(NULL)-debut);
	
}

void EndMatch()
{	
	fclose(global_ressource);
	printf("Tout est terminé\n");
	
}

int DoubleStack(const SGameState * const gameState)
{
	return(0);
}


int TakeDouble(const SGameState * const gameState)
{
	return(1);
}

void MakeDecision(const SGameState * const gameState, SMove moves[4], unsigned int lastTimeError)
{
	coups_possibles *liste=NULL , *liste_double=NULL, *best,*best2;
	int a;
	
	if(lastTimeError != 0)
	{
		printf("dés %d\t%d\n",gameState->die1,gameState->die2);
		for(a=0;a<28;a++)
		{
			printf("|");
			if(gameState->zones[a].player == EPlayer2) printf("-");
			printf("%d|",gameState->zones[a].nb_checkers);
		}
		printf("\n");
	}
	
	/*TODO dans le cas ou le J1 est le premier à jouer,
	cette instruction fait stocker le plateau initial ce qui n'a pas d'interet
	il faudrait évetuellement le vérifier quelque part 
	Autre soucit, on n'aura jamais le plateau final si J2 gagne*/
	global_jeu2 = memorise(global_jeu2,gameState->zones);
	
	liste = public_liste_coups_possibles(gameState->zones,gameState->die1,gameState->die2);
	
	/*
	courant = liste;
	while(courant != NULL && lastTimeError != 0)
	{
		printf("coup1 : %d to %d\tcoup2 : %d to %d\n",
		1+courant->premier_mouvement.src_point,1+courant->premier_mouvement.dest_point,
		1+courant->second_mouvement.src_point,1+courant->second_mouvement.dest_point);
		courant = courant->suivant;
	}
	*/
	
	if(liste != NULL)
	{/*si on a au moins un coup possible*/
	
		/*dans le cas ou on aurait une erreur parce que l'interface demande de jouer le maximum de dés
		if(lastTimeError != 0)*/
			liste = du_simple_au_double(liste);

		best = eval(global_ressource,liste);
		
		moves[0].src_point = best->premier_mouvement.src_point;
		moves[0].dest_point = best->premier_mouvement.dest_point;
		moves[1].src_point = best->second_mouvement.src_point;
		moves[1].dest_point = best->second_mouvement.dest_point;
		
		if(gameState->die1 == gameState->die2)
		{
			
			if(lastTimeError != 0)
			{
				printf("----- double ----\n");
				printf("dés %d\t%d\n",gameState->die1,gameState->die2);
				for(a=0;a<28;a++)
				{
					printf("|");
					if(best->plateau[a].player == EPlayer2) printf("-");
					printf("%d|",best->plateau[a].nb_checkers);
				}
				printf("\n");
			}
			/*on est obligé d'utiliser une autre liste en cas de double
			sinon on perdrait la première et on ne pourrait pas la libérer
			et on ne peut pas la librérer avant car best pointe sur l'un de ses éléments*/
			liste_double = public_liste_coups_possibles(best->plateau,gameState->die1,gameState->die2);
			
			/*
			courant = liste_double;
			while(courant != NULL && lastTimeError != 0)
			{
				printf("coup1 : %d to %d\tcoup2 : %d to %d\n",
				1+courant->premier_mouvement.src_point,1+courant->premier_mouvement.dest_point,
				1+courant->second_mouvement.src_point,1+courant->second_mouvement.dest_point);
				courant = courant->suivant;
			}
			*/
			
			if(liste_double != NULL)
			{
				/*if(lastTimeError != 0)*/
					liste = du_simple_au_double(liste);
				
				best2 = eval(global_ressource,liste_double);
			
				moves[2].src_point = best2->premier_mouvement.src_point;
				moves[2].dest_point = best2->premier_mouvement.dest_point;
				moves[3].src_point = best2->second_mouvement.src_point;
				moves[3].dest_point = best2->second_mouvement.dest_point;
				
				global_jeu1 = memorise(global_jeu1,best2->plateau);
			}
		}
		
		if(liste_double == NULL) global_jeu1 = memorise(global_jeu1,best->plateau);
	}
	
	if(lastTimeError != 0)
	{
		printf("\n");
		printf("\t1er :%d vers %d\n",1+moves[0].src_point,1+moves[0].dest_point);
		printf("\t2nd :%d vers %d\n",1+moves[1].src_point,1+moves[1].dest_point);
		printf("\td- 1er :%d vers %d\n",1+moves[2].src_point,1+moves[2].dest_point);
		printf("\td- 2nd :%d vers %d\n",1+moves[3].src_point,1+moves[3].dest_point);
		
		scanf("%d",&a);
	}
	
	free_coups_possibles(liste);
	free_coups_possibles(liste_double);
}


