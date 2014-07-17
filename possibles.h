#ifndef POSSIBLE_H
#define POSSIBLE_H

typedef enum type_deplacement
{
        SIMPLE,
        PRISE
}type_deplacement;

typedef struct coup2
{
        sMove second_mouvement;
        sZone final_board[28];
        struct coup2* next;
}coup2;

typedef struct coup1
{
        sMove first_movement;
        sZone first_board[28];
        struct coup2* deuxieme_coup;
        struct coup1* next;
}coup1;

typedef struct coups_possibles
{
        sMove first_movement;
        sMove second_mouvement;
        sZone  board[28];
        struct coups_possibles* next;
}coups_possibles;


/**
* Fonction qui renvoir une liste de tous les coups possibles à partir
* du board board et des deux dés die1 et die2
*
* concrètement la fonction ne fait qu'appeler liste_coups_possibles corectement
* en créant une liste avec les deux ordres de dé possibles
*
* first_movement et deuxieme_mouvement sont simples à comprendre
* board contient le board modifié avec les deux mouvements ci-dessus dans l'ordre
*/
coups_possibles* public_liste_coups_possibles(const sZone board[28],const int die1,const int die2);


/**
* Ajoute à liste_finale tous les coups de liste sans redondance
* à la fin coups possible est égal à lancienne liste plus les nouveaux coups
*/
coups_possibles* liste_coups_possibles(coups_possibles *liste_finale,const coup1 *liste);

/**
*        Supprime les coups ne jouant qu'un seul dé si il existe des coups
*        utilisant les deux dés
*/
coups_possibles* du_simple_au_double(coups_possibles *liste);

coup2* coups_autorises2(const sZone board[28],const int die);

/**
* @name add_coup2
* @details Fonction qui ajoute un nouveau coup à la liste des coups jouables et retourne la nouvelle adresse de la liste
*
* @param origine est le board d'origine
* @param deplacement est le décplacement a effectuer
* @param liste est notre fameuse liste de mouvements possibles
* @param type est le type de déplacement à effectuer
*
* @note cette fonction ne fait aucune vérification il convient donc de prendre
* t outes les précautions possibles avant de l'appeler.*
*/
coup2* add_coup2(const sZone origine[28], const sMove deplacement, coup2 *liste,const type_deplacement type);

coup1* coups_autorises1(const sZone board[28],const int die);

/**
* @name add_coup1
* @details Fonction qui ajoute un nouveau coup à la liste des coups jouables et retourne la nouvelle adresse de la liste
*
* @param origine est le board d'origine
* @param deplacement est le décplacement a effectuer
* @param liste est notre fameuse liste de mouvements possibles
* @param type est le type de déplacement à effectuer
*
* @note cette fonction ne fait aucune vérification il convient donc de prendre
* toutes les précautions possibles avant de l'appeler
*/
coup1* add_coup1(const sZone origine[28], const sMove deplacement, coup1 *liste,const type_deplacement type);

int sortie_autorisee(const sZone board[28],const int pos,const int die);
int redondant(const sZone board[28],const coups_possibles *liste);

void free_coup2(coup2 *liste);
void free_coup1(coup1 *liste);
void free_coups_possibles(coups_possibles *liste);

/**
* On additionne le nombre de jetons par case de 1 à 6 que l'on multiplie
* par la valeur du prédicat qui vaudra 1 ssi le joueur est le bon
* et sans oublier les pions deja rentrés, le tout doit être égal à 15
*/
inline int is_bear_off(const sZone board[28])
{
        return (((board[ePos_1].nb_checkers*(board[ePos_1].player == ePlayer1))
        +(board[ePos_2].nb_checkers*(board[ePos_2].player == ePlayer1))
        +(board[ePos_3].nb_checkers*(board[ePos_3].player == ePlayer1))
        +(board[ePos_4].nb_checkers*(board[ePos_4].player == ePlayer1))
        +(board[ePos_5].nb_checkers*(board[ePos_5].player == ePlayer1))
        +(board[ePos_6].nb_checkers*(board[ePos_6].player == ePlayer1))
        +board[ePos_OutP1].nb_checkers)
        == 15);
}

#endif
