#ifndef POSSIBLE
	#define POSSIBLE

typedef enum type_deplacement
{
	SIMPLE,
	PRISE
}type_deplacement;

typedef struct coup2
{
	SMove second_mouvement;
	SZone plateau_final[28];
	struct coup2* suivant;
}coup2;

typedef struct coup1
{
	SMove premier_mouvement;
	SZone premier_plateau[28];
	struct coup2* deuxieme_coup;
	struct coup1* suivant;
}coup1;

typedef struct coups_possibles
{
	SMove premier_mouvement;
	SMove second_mouvement;
	SZone  plateau[28];
	struct coups_possibles* suivant;
}coups_possibles;


/*
    Fonction qui renvoir une liste de tous les coups possibles à partir
    du plateau plateau et des deux dés die1 et die2

    concrètement la fonction ne fait qu'appeler liste_co
    ups_possibles corectement
    en créant une liste avec les deux ordres de dé possibles

    premier_mouvement et deuxieme_mouvement sont simples à comprendre
    plateau contient le plateau modifié avec les deux mouvements ci-dessus dans l'ordre
*/
coups_possibles* public_liste_coups_possibles(const SZone plateau[28],const int die1,const int die2);


/*
    Ajoute à liste_finale tous les coups de liste sans redondance
    à la fin coups possible est égal à lancienne liste plus les nouveaux coups
*/
coups_possibles* liste_coups_possibles(coups_possibles *liste_finale,const coup1 *liste);

/*
	Supprime les coups ne jouant qu'un seul dé si il existe des coups
	utilisant les deux dés
*/
coups_possibles* du_simple_au_double(coups_possibles *liste);

coup2* coups_autorises2(const SZone plateau[28],const int die);
coup2* add_coup2(const SZone origine[28], const SMove deplacement, coup2 *liste,const type_deplacement type);

coup1* coups_autorises1(const SZone plateau[28],const int die);
coup1* add_coup1(const SZone origine[28], const SMove deplacement, coup1 *liste,const type_deplacement type);

int sortie_autorisee(const SZone plateau[28],const int pos,const int die);
int redondant(const SZone plateau[28],const coups_possibles *liste);

void free_coup2(coup2 *liste);
void free_coup1(coup1 *liste);
void free_coups_possibles(coups_possibles *liste);

inline int is_bear_off(const SZone plateau[28]);

#endif
