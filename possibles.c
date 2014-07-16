#include <stdio.h>
#include <stdlib.h>

#include "backgammon.h"

#include "possibles.h"

#define VERBEUX 0


coups_possibles* public_liste_coups_possibles(const SZone plateau[28],const int die1,const int die2)
{

	coup1 *liste_possibles, *courant1;
	coups_possibles *liste_finale=NULL;
	
	/*On etablit la liste des coups dé1 puis dé 2*/
	
	liste_possibles = coups_autorises1(plateau,die1);
	
	courant1 = liste_possibles;

	while(courant1 != NULL)
	{
		/*printf("\t1er : %d to %d\n",1+courant1->premier_mouvement.src_point,1+courant1->premier_mouvement.dest_point);*/
		courant1->deuxieme_coup = coups_autorises2(courant1->premier_plateau,die2);
		courant1 = courant1->suivant;
	}
	
	liste_finale = liste_coups_possibles(liste_finale,liste_possibles);
	
	free_coup1(liste_possibles);
	
	/*Puis on étudie les coups en jouant les dés dans l'autre sens*/
	
	liste_possibles = coups_autorises1(plateau,die2);

	courant1 = liste_possibles;

	while(courant1 != NULL)
	{
		/*printf("\t2-1er : %d to %d\n",1+courant1->premier_mouvement.src_point,1+courant1->premier_mouvement.dest_point);*/
		courant1->deuxieme_coup = coups_autorises2(courant1->premier_plateau,die1);
		courant1 = courant1->suivant;
	}

	liste_finale = liste_coups_possibles(liste_finale,liste_possibles);

	free_coup1(liste_possibles);
	
	return liste_finale;
}

coups_possibles* liste_coups_possibles(coups_possibles *liste_finale,const coup1 *liste1)
{
	int i;
	coup1 *coup1_courant;
	coup2 *coup2_courant;
	coups_possibles *courant, *tail, *nouveau=NULL;

    	/*on fait un ajout en queue donc on commence par trouver le dernier élément*/
	if(liste_finale == NULL)
		tail = NULL;
	else
	{
		courant = liste_finale;
		while(courant->suivant != NULL)
		{
			courant = courant->suivant;
		}
		
		tail = courant;
	}

	coup1_courant = (coup1*) liste1;
	while(coup1_courant != NULL)
	{
		coup2_courant = coup1_courant->deuxieme_coup;
		if(coup2_courant == NULL)
		{
			if(!redondant(coup1_courant->premier_plateau,liste_finale))
			{
			 	nouveau = (coups_possibles*)malloc(sizeof(coups_possibles));
					for(i=0;i<28;i++)
				{
					nouveau->plateau[i].nb_checkers = coup1_courant->premier_plateau[i].nb_checkers;
					nouveau->plateau[i].player = coup1_courant->premier_plateau[i].player;
				}
				nouveau->premier_mouvement.dest_point = coup1_courant->premier_mouvement.dest_point;
				nouveau->premier_mouvement.src_point = coup1_courant->premier_mouvement.src_point;
				nouveau->second_mouvement.dest_point = -1 ;
				nouveau->second_mouvement.src_point = -1 ;
				
				nouveau->suivant = NULL ;
				if(tail == NULL)
				{
					tail = nouveau ;
					liste_finale = nouveau ;
				}
				else
				{
					tail->suivant = nouveau ;
					tail = nouveau ;
				}
			}
		}
		/* si il existe un deuxième coup possible */
		else while(coup2_courant != NULL)
		{
			if(!redondant(coup2_courant->plateau_final,liste_finale))
			{
				nouveau = (coups_possibles*)malloc(sizeof(coups_possibles));

				for(i=0;i<28;i++)
				{
					nouveau->plateau[i].nb_checkers = coup2_courant->plateau_final[i].nb_checkers;
					nouveau->plateau[i].player = coup2_courant->plateau_final[i].player;
				}

				nouveau->premier_mouvement.dest_point = coup1_courant->premier_mouvement.dest_point;
				nouveau->premier_mouvement.src_point = coup1_courant->premier_mouvement.src_point;

				nouveau->second_mouvement.dest_point = coup2_courant->second_mouvement.dest_point;
				nouveau->second_mouvement.src_point = coup2_courant->second_mouvement.src_point;

				nouveau->suivant = NULL ;
				if(tail == NULL)
				{
					tail = nouveau ;
					liste_finale = nouveau ;
				}
				else
				{
					tail->suivant = nouveau ;
					tail = nouveau ;
				}

			}
			coup2_courant = coup2_courant->suivant;
		}
		coup1_courant = coup1_courant->suivant;
	}

	return liste_finale;
}

coups_possibles* du_simple_au_double(coups_possibles *liste)
{
	coups_possibles *courant=liste,*temp=NULL;
	int exist_double=0,exist_alone=0;
	
	/*le seul cas problématique est le cas ou il a à la fois des simples et des doubles
	donc tant qu'on n'a pas ce cas on parcour toute la liste*/
	while(courant != NULL && !(exist_double && exist_alone))
	{
		if(courant->second_mouvement.src_point == -1) exist_alone = 1;
		else exist_double = 1;
		
		courant = courant->suivant;
	}
	
	/*on sait qu(in existe au moins deux éléments dans la liste donc pas besoin de vérifier la nullité*/
	if(exist_double && exist_alone)
	{	
		courant= liste;
		/*on supprime la séquence de premiers éléments non valides*/
		while(courant->second_mouvement.src_point == -1)
		{
			liste = liste->suivant;
			free(courant);
			courant = liste;
		}
		
		/*puis on parcour la liste en supprimant tous les coups simples*/
		while(courant->suivant != NULL)
		{
			if(courant->suivant->second_mouvement.src_point == -1)
			{
				temp = courant->suivant;
				courant->suivant = courant->suivant->suivant;
				
				free(temp);
			}
			else courant=courant->suivant;
		}
	}
	
	return liste;
}

coup2* coups_autorises2(const SZone plateau[28],const int die)
{
	int i;

	coup2 *liste=NULL;
	SMove mouvement;

	if(plateau[EPos_BarP1].nb_checkers != 0)
	{/*il y au moins un pion prisonnier*/
		if(VERBEUX) printf("\tUn pion est bloqué : ");
		if(plateau[24-die].nb_checkers == 0 || plateau[24-die].player == EPlayer1)
		{/*sortie autorisée car pas de pions ou pions alliés*/
			mouvement.src_point = EPos_BarP1;
			mouvement.dest_point = 24-die;
			liste = add_coup2(plateau,mouvement,liste,SIMPLE);
			if(VERBEUX) printf("\tsortie simple\n");
			/*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
		}
		else if(plateau[24-die].nb_checkers == 1)
		{/*il n'y a qu'un seul pion à l'adversaire*/
			mouvement.src_point = EPos_BarP1;
			mouvement.dest_point = 24-die;
			liste = add_coup2(plateau,mouvement,liste,PRISE);
			if(VERBEUX) printf("\tsortie en prenant un pion ennemi\n");
			/*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
		}
		else if(VERBEUX) printf("\tpas de sortie possible\n");
	}
	else
	{/*il n'y a pas de pion prisonier*/

		for(i=0;i<24;i++)
		{/*on parcour tout le plateau pour trouver nos pions restants*/

			if(plateau[i].player==EPlayer1 && plateau[i].nb_checkers != 0)
			{/*cas ou le joueurest le bon et a des pions*/
				if ((i-die)>=0)
				{/*le dé tombe à l'intérieur du plateau*/
					if(VERBEUX) printf("On reste à l'intérieur \n");
					if(plateau[i-die].nb_checkers == 0 || plateau[i-die].player == EPlayer1)
					{//pas de pion ou des pions alliés
						if(VERBEUX) printf("\tOn fait un déplacement classique\n");
						mouvement.src_point = i;
						mouvement.dest_point = i-die;
						liste = add_coup2(plateau,mouvement,liste,SIMPLE);
						/*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
					}
					else if(plateau[i-die].nb_checkers == 1)
					{//il n'y a qu'un seul pion à l'adversaire
						if(VERBEUX) printf("\tOn capture un pion ennemi\n");
						mouvement.src_point = i;
						mouvement.dest_point = i-die;
						liste = add_coup2(plateau,mouvement,liste,PRISE);
						/*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
					}
					//le seul autre cas possible est plusieurs pions ennemis mais on ne peut alors pas jouer
				}
				else if( (i-die)< 0 && is_bear_off(plateau))
				{//le dé fait sortir et la sortie est autorisée
					if(VERBEUX) printf("On sort du plateau\n");
					if(sortie_autorisee(plateau,i,die))
					{
						if(VERBEUX) printf("\tOn fait sortir un pion, cool!\n");
						mouvement.src_point = i;
						mouvement.dest_point = EPos_OutP1;
						liste = add_coup2(plateau,mouvement,liste,SIMPLE);
						/*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
					}
				}
			}

		}//fin de la boucle parcourant tous les triangles
	}

	return liste;
}

/*
NOM : add_coup

ROLE : Fonction qui ajoute un nouveau coup à la liste des coups jouables et retourne la nouvelle adresse de la liste

PARAMETRES :
origine est le plateau d'origine
deplacement est le décplacement a effectuer
liste est notre fameuse liste de mouvements possibles
type est le type de déplacement à effectuer

/!\ cette fonction ne fait aucune vérification il convient donc de prendre
toutes les précautions possibles avant de l'appeler.

*/
coup2* add_coup2(const SZone origine[28], const SMove deplacement, coup2* liste,const type_deplacement type)
{
	int i;

	coup2 *nouveau = (coup2*)malloc(sizeof(coup2));

	//on créé le plateau final en prenant en compte le mouvement
	for(i=0;i<28;i++)
	{
		if(i==deplacement.src_point)
		{
			nouveau->plateau_final[i].nb_checkers = origine[i].nb_checkers-1;
			nouveau->plateau_final[i].player = origine[i].player;
			
		}
		else if(i==deplacement.dest_point)
		{
			if(type == SIMPLE )
			{
				//le nombre de pions de la destination est incrémenté
				nouveau->plateau_final[i].player = EPlayer1;
				nouveau->plateau_final[i].nb_checkers = origine[i].nb_checkers + 1;
			}
			else if(type == PRISE)
			{
				//si c'est une prise il faut déplacer les pion adverse dans son BAR
				nouveau->plateau_final[EPos_BarP2].nb_checkers = origine[EPos_BarP2].nb_checkers + 1;
				nouveau->plateau_final[i].player = EPlayer1;
				nouveau->plateau_final[i].nb_checkers = 1;
			}
		}
		else
		{
			//sinon on recopie les informations
			nouveau->plateau_final[i].nb_checkers = origine[i].nb_checkers;
			nouveau->plateau_final[i].player = origine[i].player;
		}
	}

	//on enregistre le mouvement
	nouveau->second_mouvement.src_point = deplacement.src_point;
	nouveau->second_mouvement.dest_point = deplacement.dest_point;

	//on relie au reste de la chaine

	/*** L'ajout se fait en tête par simplicité mais n'a pour l'instant pas d'importance ***/
	nouveau->suivant = liste;

	return nouveau;

}

coup1* coups_autorises1(const SZone plateau[28],const int die)
{
	int i;

	coup1 *liste=NULL;
	SMove mouvement;

	if(plateau[EPos_BarP1].nb_checkers != 0)
	{//il y au moins un pion prisonnier
		if(VERBEUX) printf("\tUn pion est bloqué : ");
		if(plateau[24-die].nb_checkers == 0 || plateau[24-die].player == EPlayer1)
		{//sortie autorisée car pas de pions ou pions alliés
			mouvement.src_point = EPos_BarP1;
			mouvement.dest_point = 24-die;
			liste = add_coup1(plateau,mouvement,liste,SIMPLE);
			if(VERBEUX) printf("\tsortie simple\n");
		}
		else if(plateau[24-die].nb_checkers == 1)
		{//il n'y a qu'un seul pion à l'adversaire
			mouvement.src_point = EPos_BarP1;
			mouvement.dest_point = 24-die;
			liste = add_coup1(plateau,mouvement,liste,PRISE);
			if(VERBEUX) printf("\tsortie en prenant un pion ennemi\n");
		}
		else if(VERBEUX) printf("\tpas de sortie possible\n");
	}
	else
	{//il n'y a pas de pion prisonier

		for(i=0;i<24;i++)
		{//on parcour tout le plateau pour trouver nos pions restants

			if(plateau[i].player==EPlayer1 && plateau[i].nb_checkers != 0)
			{//cas ou le joueurest le bon et a des pions
				if ((i-die)>=0)
				{//le dé tombe à l'intérieur du plateau
					if(VERBEUX) printf("On reste à l'intérieur \n");
					if(plateau[i-die].nb_checkers == 0 || plateau[i-die].player == EPlayer1)
					{//pas de pion ou des pions alliés
						if(VERBEUX) printf("\tOn fait un déplacement classique\n");
						mouvement.src_point = i;
						mouvement.dest_point = i-die;
						liste = add_coup1(plateau,mouvement,liste,SIMPLE);
					}
					else if(plateau[i-die].nb_checkers == 1)
					{//il n'y a qu'un seul pion à l'adversaire
						if(VERBEUX) printf("\tOn capture un pion ennemi\n");
						mouvement.src_point = i;
						mouvement.dest_point = i-die;
						liste = add_coup1(plateau,mouvement,liste,PRISE);
					}
					//le seul autre cas possible est plusieurs pions ennemis mais on ne peut alors pas jouer
				}
				else if( (i-die)< 0 && is_bear_off(plateau))
				{//le dé fait sortir et la sortie est autorisée
					if(VERBEUX) printf("On sort du plateau\n");
					if(sortie_autorisee(plateau,i,die))
					{
						if(VERBEUX) printf("\tOn fait sortir un pion, cool!\n");
						mouvement.src_point = i;
						mouvement.dest_point = EPos_OutP1;
						liste = add_coup1(plateau,mouvement,liste,SIMPLE);
					}
					else if(VERBEUX) printf("pas de sortie possible\n");
				}
			}

		}//fin de la boucle parcourant tous les triangles
	}

	return liste;
}

/*
NOM : add_coup

ROLE : Fonction qui ajoute un nouveau coup à la liste des coups jouables et retourne la nouvelle adresse de la liste

PARAMETRES :
origine est le plateau d'origine
deplacement est le décplacement a effectuer
liste est notre fameuse liste de mouvements possibles
type est le type de déplacement à effectuer

/!\ cette fonction ne fait aucune vérification il convient donc de prendre
toutes les précautions possibles avant de l'appeler.

*/
coup1* add_coup1(const SZone origine[28], const SMove deplacement, coup1* liste,const type_deplacement type)
{
	int i;

	coup1 *nouveau = (coup1*)malloc(sizeof(coup1));

	//on créé le plateau final en prenant en compte le mouvement
	for(i=0;i<28;i++)
	{
		if(i==deplacement.src_point)
		{
			nouveau->premier_plateau[i].nb_checkers = origine[i].nb_checkers-1;
			nouveau->premier_plateau[i].player = origine[i].player;
			
		}
		else if(i==deplacement.dest_point)
		{
			if(type == SIMPLE )
			{
				//le nombre de pions de la destination est incrémenté
				nouveau->premier_plateau[i].player = EPlayer1;
				nouveau->premier_plateau[i].nb_checkers = origine[i].nb_checkers + 1;
			}
			else if(type == PRISE)
			{
				//si c'est une prise il faut déplacer les pion adverse dans son BAR
				nouveau->premier_plateau[EPos_BarP2].nb_checkers = origine[EPos_BarP2].nb_checkers + 1;
				nouveau->premier_plateau[i].player = EPlayer1;
				nouveau->premier_plateau[i].nb_checkers = 1;
			}
		}
		else
		{
			//sinon on recopie les informations
			nouveau->premier_plateau[i].nb_checkers = origine[i].nb_checkers;
			nouveau->premier_plateau[i].player = origine[i].player;
		}
	}

	//on enregistre le mouvement
	nouveau->premier_mouvement.src_point = deplacement.src_point;
	nouveau->premier_mouvement.dest_point = deplacement.dest_point;

	nouveau->deuxieme_coup = NULL;

	//on relie au reste de la chaine

	/*** L'ajout se fait en tête parcoups_possibles *liste simplicité mais n'a pour l'instant pas d'importance ***/
	nouveau->suivant = liste;

	return nouveau;

}

int sortie_autorisee(const SZone plateau[28],const int pos,const int die)
{
	int m;	
		
	if(pos-die == -1) return 1;
	else
	{
		if(plateau[EPos_6].nb_checkers==0 || plateau[EPos_6].player==EPlayer2)
			if(plateau[EPos_5].nb_checkers==0 || plateau[EPos_5].player==EPlayer2)
				if(plateau[EPos_4].nb_checkers==0 || plateau[EPos_4].player==EPlayer2)
					if(plateau[EPos_3].nb_checkers==0 || plateau[EPos_3].player==EPlayer2)
						if(plateau[EPos_2].nb_checkers==0 || plateau[EPos_2].player==EPlayer2)
							m=0;
						else m=1;
					else m=2;
				else m=3;
			else m=4;
		else m=6;
				
		if(pos==m) return 1;
		else return 0;
	}
}

int redondant(const SZone plateau[28],const coups_possibles *liste)
{
	coups_possibles *courant = (coups_possibles*)liste;
	int i;
	while(courant != NULL)
	{
		i=0;
		while((i<28 && courant->plateau[i].nb_checkers==plateau[i].nb_checkers) 
			&& courant->plateau[i].player==plateau[i].player)
		{
			i++;
		}
		/*si i=28 c'est qu'on a parcouru le plateau sans trouver de différence donc plateau est redondant*/
		if(i==28) return 1;
		else courant = courant->suivant;
	}
	/*si on a parcourut toute la liste plateau n'est pas redondant*/
	return 0;
}

void free_coup2(coup2 *liste)
{
	if(liste->suivant != NULL)
		free_coup2(liste->suivant);

	free(liste);
}

void free_coup1(coup1 *liste)
{
	if(liste != NULL)
	{
		if(liste->deuxieme_coup != NULL)
			free_coup2(liste->deuxieme_coup);
		
		if(liste->suivant != NULL)
			free_coup1(liste->suivant);	

		free(liste);
	}
}

void free_coups_possibles(coups_possibles *liste)
{
	if(liste != NULL)
	{
		if(liste->suivant != NULL)
			free_coups_possibles(liste->suivant);

		free(liste);
	}
}

inline int is_bear_off(const SZone plateau[28])
{
	/*On additionne le nombre de jetons par case de 1 à 6 que l'on multiplie
	par la valeur du prédicat qui vaudra 1 ssi le joueur est le bon
	et sans oublier les pions deja rentrés, le tout doit être égal à 15*/
	return (((plateau[EPos_1].nb_checkers*(plateau[EPos_1].player == EPlayer1))
		+(plateau[EPos_2].nb_checkers*(plateau[EPos_2].player == EPlayer1))
		+(plateau[EPos_3].nb_checkers*(plateau[EPos_3].player == EPlayer1))
		+(plateau[EPos_4].nb_checkers*(plateau[EPos_4].player == EPlayer1))
		+(plateau[EPos_5].nb_checkers*(plateau[EPos_5].player == EPlayer1))
		+(plateau[EPos_6].nb_checkers*(plateau[EPos_6].player == EPlayer1))
		+plateau[EPos_OutP1].nb_checkers)
		== 15) ;
}



