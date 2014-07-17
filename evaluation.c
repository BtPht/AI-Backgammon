#include "evaluation.h"

/*
    Détails de SEUIL
    Pratiquement le seuil est la valeur de note maximale trouvée en dessous de laquelle on
    autorise à jouer un board inconnu
    De fait plus le seuil est élevé plus le nombre de nouveau coups sera important.

	ie:
    Un seuil à 1 autorise à prendre un nouveau board dés qu'il n'existe pas de coup winner à 100%.
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

	if(arbre == NULL && prof != 28)
	{
		nouveau = (decatree*)malloc(sizeof(decatree));
		for(i=0;i<13;i++) nouveau->next[i] = NULL;
		nouveau->win = 0;
		nouveau->loose = 0;
		nouveau->next[ligne->triangle[prof]+6] = add_ligne(nouveau->next[ligne->triangle[prof]+6],ligne,prof+1);

		arbre = nouveau;
	}
	else if(arbre == NULL && prof == 28)
	{
		nouveau = (decatree*)malloc(sizeof(decatree));
		for(i=0;i<13;i++) nouveau->next[i] = NULL;

		nouveau->win = ligne->win;
		nouveau->loose = ligne->loose;

		arbre = nouveau;
	}
	else if(arbre != NULL && prof != 28)
	{
		arbre->next[ligne->triangle[prof]+6] = add_ligne(arbre->next[ligne->triangle[prof]+6],ligne,prof+1);
	}
	else if(arbre != NULL && prof == 28)
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
	coups_possibles *best=NULL,*current;

	current = (coups_possibles*) liste;

	while(current != NULL)
	{
		if(existe(ressource,current->board,&note,&sens))
        	{/*
			cas ou le board a deja été joué
			pas besoin de vérifier le sens, en effet la note est donnée
			en prenant compte du sens dans lequel le board a été stocké
			*/
        		if(best != NULL)
			{
				if(note > max)
                		{
					best = current;
					max = note;
				}
			}
            		else
            		{
                		best = current;
                		max = note;
			}

		}
        	else if(max < SEUIL || current->next == NULL)
        	{
            		best = current;
            		max = SEUIL; /*de cette manière on prend le premier coups autorisé, les nexts devront être meilleurs*/
        	}

        	current = current->next;
    	}

    	return best;
}

int existe2(decatree *arbre, const sZone board[28], float *note, int *sens)
{
	int i=0;
	decatree *current = (decatree*)arbre;

	while(i<28 && current != NULL)
	{
		if(board[i].player == ePlayer1 && board[i].nb_checkers < 6)
			current = current->next[6+board[i].nb_checkers];
		else if(board[i].player == ePlayer1 && board[i].nb_checkers >= 6)
			current = current->next[12];
		else if(board[i].player == ePlayer2 && board[i].nb_checkers < 6)
			current = current->next[6-board[i].nb_checkers];
		else if(board[i].player == ePlayer2 && board[i].nb_checkers >= 6)
			current = current->next[0];

		else current = NULL;

		i++;
	}

	/*if(board[ePos_OutP1].nb_checkers < 6 && (signed int)board[ePos_OutP1].nb_checkers != ligne.triangle[i]) b1 = 0;
	if(board[ePos_BarP1].nb_checkers < 6 && (signed int)board[ePos_BarP1].nb_checkers != ligne.triangle[++i]) b1 = 0;
	if(board[ePos_OutP2].nb_checkers < 6 && (signed int)board[ePos_OutP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
	if(board[ePos_BarP2].nb_checkers < 6 && (signed int)board[ePos_BarP2].nb_checkers != ligne.triangle[++i]) b1 = 0;*/

	/*On a parcouru l'arbre en profondeur et on a retrouvé toutes les valeurs*/
	if(current != NULL)
	{
		*note = (float)current->win/((float)current->win+(float)current->loose);
		*sens = 1;
		return 1;
	}


	i=ePos_24;
	current= arbre;
	while(i>=0 && current != NULL)
	{
		if(board[i].player == ePlayer1 && board[i].nb_checkers < 6)
			current = current->next[6-board[i].nb_checkers];
		else if(board[i].player == ePlayer1 && board[i].nb_checkers >= 6)
			current = current->next[0];
		else if(board[i].player == ePlayer2 && board[i].nb_checkers < 6)
			current = current->next[board[i].nb_checkers+6];
		else if(board[i].player == ePlayer2 && board[i].nb_checkers >= 6)
			current = current->next[12];

		i--;
	}

	if(current != NULL)
	{
		if(board[ePos_OutP2].nb_checkers < 6)
			current = current->next[board[ePos_OutP2].nb_checkers+6];
		else if(board[ePos_OutP2].nb_checkers >= 6)
			current = current->next[12];
	}

	if(current != NULL)
	{
		if(board[ePos_BarP2].nb_checkers < 6)
			current = current->next[board[ePos_BarP2].nb_checkers+6];
		else if(board[ePos_BarP2].nb_checkers >= 6)
			current = current->next[12];
	}

	if(current != NULL)
	{
		if(board[ePos_OutP1].nb_checkers < 6)
			current = current->next[board[ePos_OutP1].nb_checkers+6];
		else if(board[ePos_OutP1].nb_checkers >= 6)
			current = current->next[12];
	}

	if(current != NULL)
	{
		if(board[ePos_BarP1].nb_checkers < 6)
			current = current->next[board[ePos_BarP1].nb_checkers+6];
		else if(board[ePos_BarP1].nb_checkers >= 6)
			current = current->next[12];
	}

	if(current != NULL)
	{/*soit on est dans la position d'un adversaire*/
		*note = (float)current->loose/((float)current->win+(float)current->loose);
		*sens = 2;
		return 1;
	}

	return 0;
}
/*
prend en paramètre le fichier de ressoureces
le board à évaluer, la note si le board est trouvé
*/
int existe(FILE *fichier,const sZone board[28] ,float *note,int *sens)
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
		while(i<=ePos_24 && (b1 || b2) )
		{/* on vérifie que les 24 triangles sont bien identiques */
			nb_pions = ligne.triangle[i];

			if(board[i].player == ePlayer1)
			{
				if(nb_pions < 0) b1=0;
				else if(board[i].nb_checkers < 6 && (signed int)board[i].nb_checkers != nb_pions) b1=0;
				else if (board[i].nb_checkers >= 6 && nb_pions != 6) b1=0;
			}
			else
			{
				if(nb_pions > 0) b1=0;
				else if(board[i].nb_checkers < 6 && (signed int)board[i].nb_checkers != -nb_pions) b1=0;
				else if (board[i].nb_checkers >= 6 && nb_pions != -6) b1=0;
			}


			if(board[ePos_24-i].player == ePlayer1)
			{
				if(nb_pions > 0) b2=0;
				else if(board[ePos_24-i].nb_checkers < 6 && (signed int)board[ePos_24-i].nb_checkers != -nb_pions) b2=0;
				else if (board[ePos_24-i].nb_checkers >= 6 && nb_pions != -6) b2=0;
			}
			else
			{
				if(nb_pions < 0) b2=0;
				else if(board[ePos_24-i].nb_checkers < 6 && (signed int)board[ePos_24-i].nb_checkers != nb_pions) b2=0;
				else if (board[ePos_24-i].nb_checkers >= 6 && nb_pions != 6) b2=0;
			}


			i++;
		}

		if(b1 || b2)
		{
			if(board[ePos_OutP1].nb_checkers < 6 && (signed int)board[ePos_OutP1].nb_checkers != ligne.triangle[i]) b1 = 0;
			if(board[ePos_OutP2].nb_checkers < 6 && (signed int)board[ePos_OutP2].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(board[ePos_BarP1].nb_checkers < 6 && (signed int)board[ePos_BarP1].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(board[ePos_BarP2].nb_checkers < 6 && (signed int)board[ePos_BarP2].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(board[ePos_OutP2].nb_checkers < 6 && (signed int)board[ePos_OutP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(board[ePos_OutP1].nb_checkers < 6 && (signed int)board[ePos_OutP1].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		if(b1 || b2)
		{
			if(board[ePos_BarP2].nb_checkers < 6 && (signed int)board[ePos_BarP2].nb_checkers != ligne.triangle[++i]) b1 = 0;
			if(board[ePos_BarP1].nb_checkers < 6 && (signed int)board[ePos_BarP1].nb_checkers != ligne.triangle[i]) b2 = 0;
		}

		win=ligne.win;
		loose=ligne.loose;

		if(b1)
		{/* soit on a bien trouvé un board correspondant*/
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

void add_board(FILE* ressource,const sZone board[28] ,const ePlayer winner)
{
	int i,sens,win,loose;
	float a;
	b_ligne ligne;

	if(existe(ressource,board,&a,&sens))
	{
		fseek(ressource,-sizeof(int[2]),SEEK_CUR);
		fread(&win,sizeof(win),1,ressource);
		fread(&loose,sizeof(loose),1,ressource);
		fseek(ressource,-sizeof(int[2]),SEEK_CUR);
		if(sens == 1)
		{
			win+=(winner == ePlayer1);
			loose+=(winner == ePlayer2);
			fwrite(&win,sizeof(win),1,ressource);
			fwrite(&loose,sizeof(loose),1,ressource);
		}
		else
		{
			win+=(winner == ePlayer2);
			loose+=(winner == ePlayer1);
			fwrite(&win,sizeof(win),1,ressource);
			fwrite(&loose,sizeof(loose),1,ressource);
		}
	}
	else
	{
		for(i=0;i<28;i++)
		{
			if(board[i].player == ePlayer1 && board[i].nb_checkers < 6)
				ligne.triangle[i] = board[i].nb_checkers;
			else if(board[i].player == ePlayer1 && board[i].nb_checkers >= 6)
				ligne.triangle[i] = 6;
			else if(board[i].player == ePlayer2 && board[i].nb_checkers < 6)
				ligne.triangle[i] = -board[i].nb_checkers;
			else if(board[i].player == ePlayer2 && board[i].nb_checkers >= 6)
				ligne.triangle[i] = -6;
		}
		ligne.win = (winner == ePlayer1);
		ligne.loose = (winner == ePlayer2);

		fseek(ressource, 0, SEEK_END);
		fwrite(&ligne,sizeof(b_ligne),1,ressource);
	}
}

played_board *memorise(played_board *liste, const sZone board[28])
{
	int i;
	played_board *nouveau;

	nouveau = (played_board*)malloc(sizeof(played_board));

	for(i=0;i<28;i++)
	{
		nouveau->board[i].nb_checkers = board[i].nb_checkers;
		nouveau->board[i].player = board[i].player;
	}

	nouveau->next = liste;

	return nouveau;

}

void free_memorise(played_board *liste)
{
	if(liste != NULL)
	{
		if(liste->next != NULL)
			free_memorise(liste->next);

		free(liste);
	}
}


