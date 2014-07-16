#include <stdio.h>
#include <stdlib.h>

#include "backgammon.h"

#include "possibles.h"
#include "evaluation.h"

/*
    Détails de SEUIL
    Pratiquement le seuil est la valeur de note maximale trouvée en dessous de laquelle on
    autorise à jouer un plateau inconnu
    De fait plus le seuil est élevé plus le nombre de nouveau coups sera important.

	ie:
    Un seuil à 1 autorise à prendre un nouveau plateau dés qu'il n'existe pas de coup gagnant à 100%.
    Un seuil à 0 fera prendre le meilleur coup connu

    La valeur du seuil n'a pas d'incidence sur les performances
*/
#define SEUIL 0.7

decatree *create_decatree(FILE* ressource)
{
	b_ligne ligne;
	decatree *arbre=NULL;

	fseek(ressource, 0, SEEK_SET);
	
	while(!feof(ressource))
	{
		fread(&ligne,sizeof(b_ligne),1,ressource);
		arbre = add_ligne(arbre,&ligne,0);
	}
	return arbre;
}

decatree *add_ligne(decatree *arbre,b_ligne *ligne,int prof)
{
	int i;
	decatree *nouveau;
	
	if(arbre == NULL && prof!=28)
	{
		nouveau = (decatree*)malloc(sizeof(decatree));
		for(i=0;i<13;i++) nouveau->suivant[i] = NULL;
		nouveau->win = 0;
		nouveau->loose = 0;
		nouveau->suivant[ligne->triangle[prof]+6] = add_ligne(nouveau->suivant[ligne->triangle[prof]+6],ligne,prof+1) ;
		
		arbre = nouveau;
	}
	else if(arbre == NULL && prof==28)
	{
		nouveau = (decatree*)malloc(sizeof(decatree));
		for(i=0;i<13;i++) nouveau->suivant[i] = NULL;
		
		nouveau->win = ligne->win;
		nouveau->loose = ligne->loose;
		
		arbre = nouveau;
	}
	else if(arbre != NULL && prof!=28)
	{
		arbre->suivant[ligne->triangle[prof]+6] = add_ligne(arbre->suivant[ligne->triangle[prof]+6],ligne,prof+1);
	}
	else if(arbre != NULL && prof==28)
	{
		arbre->win += ligne->win;
		arbre->loose += ligne->loose;
	}

	
	return arbre;
}

coups_possibles *eval(FILE* ressource,const coups_possibles *liste)
{
	int sens;
	float note,max=0;
	coups_possibles *best=NULL,*courant;

	courant = (coups_possibles*) liste;
	
	while(courant != NULL)
	{
		if(existe(ressource,courant->plateau,&note,&sens))
        	{/*
			cas ou le plateau a deja été joué
			pas besoin de vérifier le sens, en effet la note est donnée
			en prenant compte du sens dans lequel le plateau a été stocké
			*/
        		if(best != NULL)
			{
				if(note > max)
                		{
					best = courant;
					max = note;
				}
			}
            		else
            		{
                		best = courant;
                		max = note;
			}

		}
        	else if(max < SEUIL || courant->suivant == NULL)
        	{
            		best = courant;
            		max = SEUIL; /*de cette manière on prend le premier coups autorisé, les suivants devront être meilleurs*/
        	}

        	courant = courant->suivant;
    	}
    	
    	return best;
}

int existe2(decatree *arbre, const SZone plateau[28], float *note, int *sens)
{
	int i=0;
	decatree *courant = (decatree*)arbre;
	
	while(i<28 && courant != NULL)
	{
		if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers < 6)
			courant = courant->suivant[6+plateau[i].nb_checkers];
		else if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers >= 6)
			courant = courant->suivant[12];
		else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers < 6)
			courant = courant->suivant[6-plateau[i].nb_checkers];
		else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers >= 6)
			courant = courant->suivant[0];
		
		else courant = NULL;
		
		i++;
	}

	/*if(plateau[EPos_OutP1].nb_checkers < 6 && (signed int)plateau[EPos_OutP1].nb_checkers != ligne.triangle[i]) b1 = 0;
	if(plateau[EPos_BarP1].nb_checkers < 6 && (signed int)plateau[EPos_BarP1].nb_checkers != ligne.triangle[++i]) b1 = 0;
	if(plateau[EPos_OutP2].nb_checkers < 6 && (signed int)plateau[EPos_OutP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
	if(plateau[EPos_BarP2].nb_checkers < 6 && (signed int)plateau[EPos_BarP2].nb_checkers != ligne.triangle[++i]) b1 = 0;*/
	
	/*On a parcouru l'arbre en profondeur et on a retrouvé toutes les valeurs*/
	if(courant != NULL)
	{
		*note = (float)courant->win/((float)courant->win+(float)courant->loose);
		*sens = 1;
		return 1;
	}

	
	i=EPos_24;
	courant= arbre;
	while(i>=0 && courant != NULL)
	{
		if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers < 6)
			courant = courant->suivant[6-plateau[i].nb_checkers];
		else if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers >= 6)
			courant = courant->suivant[0];
		else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers < 6)
			courant = courant->suivant[plateau[i].nb_checkers+6];
		else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers >= 6)
			courant = courant->suivant[12];
		
		i--;
	}
	
	if(courant != NULL)
	{
		if(plateau[EPos_OutP2].nb_checkers < 6)
			courant = courant->suivant[plateau[EPos_OutP2].nb_checkers+6];
		else if(plateau[EPos_OutP2].nb_checkers >= 6)
			courant = courant->suivant[12];
	}
	
	if(courant != NULL)
	{
		if(plateau[EPos_BarP2].nb_checkers < 6)
			courant = courant->suivant[plateau[EPos_BarP2].nb_checkers+6];
		else if(plateau[EPos_BarP2].nb_checkers >= 6)
			courant = courant->suivant[12];
	}
	
	if(courant != NULL)
	{	
		if(plateau[EPos_OutP1].nb_checkers < 6)
			courant = courant->suivant[plateau[EPos_OutP1].nb_checkers+6];
		else if(plateau[EPos_OutP1].nb_checkers >= 6)
			courant = courant->suivant[12];
	}
	
	if(courant != NULL)
	{	
		if(plateau[EPos_BarP1].nb_checkers < 6)
			courant = courant->suivant[plateau[EPos_BarP1].nb_checkers+6];
		else if(plateau[EPos_BarP1].nb_checkers >= 6)
			courant = courant->suivant[12];
	}
	
	if(courant != NULL)
	{/*soit on est dans la position d'un adversaire*/
		*note = (float)courant->loose/((float)courant->win+(float)courant->loose);
		*sens = 2;
		return 1;
	}
	
	return 0;
}
/*
prend en paramètre le fichier de ressoureces
le plateau à évaluer, la note si le plateau est trouvé
*/
int existe(FILE *fichier,const SZone plateau[28] ,float *note,int *sens)
{
	int nb_pions,i,win,loose,b1,b2;
	b_ligne ligne;
	
	fseek(fichier, 0, SEEK_SET);
	
    	while(!feof(fichier))
	{
		b1=1;
		b2=1;
		i=0;
		fread(&ligne,sizeof(b_ligne),1,fichier);
		while(i<=EPos_24 && (b1 || b2) )
		{/* on vérifie que les 24 triangles sont bien identiques */
			nb_pions = ligne.triangle[i];

			if(plateau[i].player == EPlayer1)
			{
				if(nb_pions < 0) b1=0;
				else if(plateau[i].nb_checkers < 6 && (signed int)plateau[i].nb_checkers != nb_pions) b1=0;
				else if (plateau[i].nb_checkers >= 6 && nb_pions != 6) b1=0;
			}
			else
			{
				if(nb_pions > 0) b1=0;
				else if(plateau[i].nb_checkers < 6 && (signed int)plateau[i].nb_checkers != -nb_pions) b1=0;
				else if (plateau[i].nb_checkers >= 6 && nb_pions != -6) b1=0;
			}

			
			if(plateau[EPos_24-i].player == EPlayer1)
			{
				if(nb_pions > 0) b2=0;
				else if(plateau[EPos_24-i].nb_checkers < 6 && (signed int)plateau[EPos_24-i].nb_checkers != -nb_pions) b2=0;
				else if (plateau[EPos_24-i].nb_checkers >= 6 && nb_pions != -6) b2=0;
			}
			else
			{
				if(nb_pions < 0) b2=0;
				else if(plateau[EPos_24-i].nb_checkers < 6 && (signed int)plateau[EPos_24-i].nb_checkers != nb_pions) b2=0;
				else if (plateau[EPos_24-i].nb_checkers >= 6 && nb_pions != 6) b2=0;
			}


			i++;
		}

		if(b1 || b2)
		{
			if(plateau[EPos_OutP1].nb_checkers < 6 && (signed int)plateau[EPos_OutP1].nb_checkers != ligne.triangle[i]) b1 = 0;
			if(plateau[EPos_OutP2].nb_checkers < 6 && (signed int)plateau[EPos_OutP2].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(plateau[EPos_BarP1].nb_checkers < 6 && (signed int)plateau[EPos_BarP1].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(plateau[EPos_BarP2].nb_checkers < 6 && (signed int)plateau[EPos_BarP2].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(plateau[EPos_OutP2].nb_checkers < 6 && (signed int)plateau[EPos_OutP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(plateau[EPos_OutP1].nb_checkers < 6 && (signed int)plateau[EPos_OutP1].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(plateau[EPos_BarP2].nb_checkers < 6 && (signed int)plateau[EPos_BarP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(plateau[EPos_BarP1].nb_checkers < 6 && (signed int)plateau[EPos_BarP1].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		win=ligne.win;
		loose=ligne.loose;

		if(b1)
		{/* soit on a bien trouvé un plateau correspondant*/
			*note = (float)win/((float)win+(float)loose);
			*sens = 1;
			return 1;
		}
		else if(b2)
		{/*soit on est dans la position d'un adversaire*/
			*note = (float)loose/((float)win+(float)loose);
			*sens = 2;
			return 1;
		}
		
	}
	return 0;
}

void add_plateau(FILE* ressource,const SZone plateau[28] ,const EPlayer gagnant)
{
	int i,sens,win,loose;
	float a;
	b_ligne ligne;

	if(existe(ressource,plateau,&a,&sens))
	{
		fseek(ressource,-sizeof(int[2]),SEEK_CUR);
		fread(&win,sizeof(win),1,ressource);
		fread(&loose,sizeof(loose),1,ressource);
		fseek(ressource,-sizeof(int[2]),SEEK_CUR);
		if(sens == 1)
		{
			win+=(gagnant==EPlayer1);
			loose+=(gagnant==EPlayer2);
			fwrite(&win,sizeof(win),1,ressource);
			fwrite(&loose,sizeof(loose),1,ressource);
		}
		else
		{
			win+=(gagnant==EPlayer2);
			loose+=(gagnant==EPlayer1);
			fwrite(&win,sizeof(win),1,ressource);
			fwrite(&loose,sizeof(loose),1,ressource);
		}
	}
	else
	{
		for(i=0;i<28;i++)
		{
			if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers < 6)
				ligne.triangle[i] = plateau[i].nb_checkers;
			else if(plateau[i].player == EPlayer1 && plateau[i].nb_checkers >= 6)
				ligne.triangle[i] = 6;
			else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers < 6)
				ligne.triangle[i] = -plateau[i].nb_checkers;
			else if(plateau[i].player == EPlayer2 && plateau[i].nb_checkers >= 6)
				ligne.triangle[i] = -6;
		}
		ligne.win = (gagnant==EPlayer1);
		ligne.loose = (gagnant==EPlayer2);
		
		fseek(ressource, 0, SEEK_END);
		fwrite(&ligne,sizeof(b_ligne),1,ressource);
	}
}

plateaux_joues *memorise(plateaux_joues *liste, const SZone plateau[28])
{
	int i;
	plateaux_joues *nouveau;
	
	nouveau = (plateaux_joues*)malloc(sizeof(plateaux_joues));
	
	for(i=0;i<28;i++)
	{
		nouveau->plateau[i].nb_checkers = plateau[i].nb_checkers;
		nouveau->plateau[i].player = plateau[i].player;
	}
	
	nouveau->suivant = liste;
	
	return nouveau;
	
}

void free_memorise(plateaux_joues *liste)
{
	if(liste != NULL)
	{
		if(liste->suivant != NULL)
			free_memorise(liste->suivant);
		
		free(liste);
	}
}


