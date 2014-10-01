#ifndef POSSIBLE_H_INCLUDED
#define POSSIBLE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

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

typedef struct possible_movements
{
        sMove first_movement;
        sMove second_mouvement;
        sZone  board[28];
        struct possible_movements* next;
}possible_movements;


/**
 * @name
 * Fonction qui renvoit une liste de tous les coups possibles à partir
 * du board board et des deux dés die1 et die2
 *
 * concrètement la fonction ne fait qu'appeler liste_possible_movements corectement
 * en créant une liste avec les deux ordres de dé possibles
 *
 * first_movement et deuxieme_mouvement sont simples à comprendre
 * board contient le board modifié avec les deux mouvements ci-dessus dans l'ordre
 */
possible_movements* list_possible_moves_public(const sZone board[28], const int die1, const int die2);


/**
 * Ajoute à liste_finale tous les coups de liste sans redondance
 * à la fin coups possible est égal à lancienne liste plus les nouveaux coups
 */
possible_movements* liste_possible_movements(possible_movements *liste_finale, const coup1 *liste);

/**
 *        Supprime les coups ne jouant qu'un seul dé si il existe des coups
 *        utilisant les deux dés
 */
possible_movements* du_simple_au_double(possible_movements *liste);

coup2* coups_autorises2(const sZone board[28],const int die);

/**
 * @name add_coup2
 * @details Function adding a new possibility to the list obtained after the first choice
 *
 * @param origine est le board d'origine
 * @param deplacement est le décplacement a effectuer
 * @param liste est notre fameuse liste de mouvements possibles
 * @param type est le type de déplacement à effectuer
 *
 * @returns The list of all the possibile moves
 *
 * @note cette fonction ne fait aucune vérification il convient donc de prendre
 * t outes les précautions possibles avant de l'appeler.*
 */
coup2* add_coup2(const sZone origine[28], const sMove deplacement, coup2 *liste, const type_deplacement type);

coup1* coups_autorises1(const sZone board[28], const int die);

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
coup1* add_coup1(const sZone origine[28], const sMove deplacement, coup1 *liste, const type_deplacement type);

bool sortie_autorisee(const sZone board[28], const int pos, const int die);
bool redondant(const sZone board[28], const possible_movements *liste);

void free_coup2(coup2 *liste);
void free_coup1(coup1 *liste);
void free_possible_movements(possible_movements *liste);

/**
 * On additionne le nombre de jetons par case de 1 à 6 que l'on multiplie
 * par la valeur du prédicat qui vaudra 1 ssi le joueur est le bon
 * et sans oublier les pions deja rentrés, le tout doit être égal à 15
 */
inline bool is_bear_off(const sZone board[28])
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
