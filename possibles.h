#ifndef POSSIBLE
#define POSSIBLE

typedef enum type_deplacement
{
        SIMPLE,
        PRISE
}type_deplacement;

typedef struct coup2
{
        sMove second_mouvement;
        sZone plateau_final[28];
        struct coup2* suivant;
}coup2;

typedef struct coup1
{
        sMove premier_mouvement;
        sZone premier_plateau[28];
        struct coup2* deuxieme_coup;
        struct coup1* suivant;
}coup1;

typedef struct coups_possibles
{
        sMove premier_mouvement;
        sMove second_mouvement;
        sZone  plateau[28];
        struct coups_possibles* suivant;
}coups_possibles;


/*
* Fonction qui renvoir une liste de tous les coups possibles � partir
* du plateau plateau et des deux d�s die1 et die2
*
* concr�tement la fonction ne fait qu'appeler liste_co
* ups_possibles corectement
* en cr�ant une liste avec les deux ordres de d� possibles
*
* premier_mouvement et deuxieme_mouvement sont simples � comprendre
* plateau contient le plateau modifi� avec les deux mouvements ci-dessus dans l'ordre
*/
coups_possibles* public_liste_coups_possibles(const sZone plateau[28],const int die1,const int die2);


/*
* Ajoute � liste_finale tous les coups de liste sans redondance
* � la fin coups possible est �gal � lancienne liste plus les nouveaux coups
*/
coups_possibles* liste_coups_possibles(coups_possibles *liste_finale,const coup1 *liste);

/*
*        Supprime les coups ne jouant qu'un seul d� si il existe des coups
*        utilisant les deux d�s
*/
coups_possibles* du_simple_au_double(coups_possibles *liste);

coup2* coups_autorises2(const sZone plateau[28],const int die);
coup2* add_coup2(const sZone origine[28], const sMove deplacement, coup2 *liste,const type_deplacement type);

coup1* coups_autorises1(const sZone plateau[28],const int die);
coup1* add_coup1(const sZone origine[28], const sMove deplacement, coup1 *liste,const type_deplacement type);

int sortie_autorisee(const sZone plateau[28],const int pos,const int die);
int redondant(const sZone plateau[28],const coups_possibles *liste);

void free_coup2(coup2 *liste);
void free_coup1(coup1 *liste);
void free_coups_possibles(coups_possibles *liste);

/**
* On additionne le nombre de jetons par case de 1 � 6 que l'on multiplie
* par la valeur du pr�dicat qui vaudra 1 ssi le joueur est le bon
* et sans oublier les pions deja rentr�s, le tout doit �tre �gal � 15
*/
inline int is_bear_off(const sZone plateau[28])
{
        return (((plateau[ePos_1].nb_checkers*(plateau[ePos_1].player == ePlayer1))
        +(plateau[ePos_2].nb_checkers*(plateau[ePos_2].player == ePlayer1))
        +(plateau[ePos_3].nb_checkers*(plateau[ePos_3].player == ePlayer1))
        +(plateau[ePos_4].nb_checkers*(plateau[ePos_4].player == ePlayer1))
        +(plateau[ePos_5].nb_checkers*(plateau[ePos_5].player == ePlayer1))
        +(plateau[ePos_6].nb_checkers*(plateau[ePos_6].player == ePlayer1))
        +plateau[ePos_OutP1].nb_checkers)
        == 15) ;
}

#endif
