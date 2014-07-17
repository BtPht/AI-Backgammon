#include "backgammon.h"
#include "backgammon_private.h"

#define VERBOSE 0

/**
 * Global variables used by the IA
 */
FILE *global_ressource;
played_board *global_jeu1=NULL, *global_jeu2=NULL;

void initLibrary(char name[50])
{
        strcpy(name,"Bertrand");

        global_ressource = fopen("ressource.bg","rwb+");

        if(global_ressource == NULL)
        {
                printf("File not found, creating a new one\n");
                global_ressource = fopen("ressource.bg","wb");
        }
        else printf("Ressources file correctly located\n");

}

void startMatch(const unsigned int target_score)
{
}

void startGame()
{
}

void endGame()
{
        played_board *current;
        ePlayer winner;
        decatree *tree=NULL;

        time_t debut;

        float note;
        int sens;

        debut=time(NULL);

        /*Pour mémoire J1 contient le dernier board mémorisé
         *        donc soit j'ai rangé tous mes pions et l'arbitre a appelé EndGame()
         *        juste aprés et donc j'ai gagné, soit je n'ai pas finit et l'appel
         *        a été fait suite à la vistoire du mauvais d'en face.
         */
        if(global_jeu1->board[ePos_OutP1].nb_checkers == 15) winner=ePlayer1;
        else winner=ePlayer2;

        tree = create_decatree(global_ressource);

        current = global_jeu1;
        while(current != NULL)
        {
                if(existe2(tree,current->board,&note,&sens) != existe(global_ressource,current->board,&note,&sens))
                        printf("___ ERROR __");

        }

        /*current = global_jeu2;
         *while(current != NULL)
         *{
         *      add_board(global_ressource,current->board,winner);
         *      current = current->next;
         *}
        */

        fflush(global_ressource);

        free_memorise(global_jeu1);
        free_memorise(global_jeu2);

        global_jeu1 = NULL;
        global_jeu2 = NULL;

        printf("%ld secondes pour le learning\n",time(NULL)-debut);

}

void endMatch()
{
        fclose(global_ressource);
        printf("Tout est terminé\n");

}

int doubleStack(const sGameState * const gameState)
{
        return(0);
}


int takeDouble(const sGameState * const gameState)
{
        return(1);
}

void makeDecision(const sGameState * const gameState, sMove moves[4], unsigned int lastTimeError)
{
        coups_possibles *list=NULL , *list_double=NULL, *best,*best2;

        if(lastTimeError != 0)
        {
                printf("dés %d\t%d\n",gameState->die1,gameState->die2);
                for(unsigned int i=0;i<28;i++)
                {
                        printf("|");
                        if(gameState->zones[i].player == ePlayer2) printf("-");
                        printf("%d|",gameState->zones[i].nb_checkers);
                }
                printf("\n");
        }

        /*TODO dans le cas ou le J1 est le premier à jouer,
         *        cette instruction fait stocker le board initial ce qui n'a pas d'interet
         *        il faudrait évetuellement le vérifier quelque part
         *        Autre soucit, on n'aura jamais le board final si J2 gagne*/
        global_jeu2 = memorise(global_jeu2,gameState->zones);

        list = public_list_coups_possibles(gameState->zones,gameState->die1,gameState->die2);

        /*
         *        current = list;
         *        while(current != NULL && lastTimeError != 0)
         *        {
         *                printf("coup1 : %d to %d\tcoup2 : %d to %d\n",
         *                1+current->first_movement.src_point,1+current->first_movement.dest_point,
         *                1+current->second_mouvement.src_point,1+current->second_mouvement.dest_point);
         *                current = current->next;
}
*/

        if(list != NULL)
        {/*si on a au moins un coup possible*/

                /*dans le cas ou on aurait une erreur parce que l'interface demande de jouer le maximum de dés
                 *                if(lastTimeError != 0)*/
                list = du_simple_au_double(list);

                best = eval(global_ressource,list);

                moves[0].src_point = best->first_movement.src_point;
                moves[0].dest_point = best->first_movement.dest_point;
                moves[1].src_point = best->second_mouvement.src_point;
                moves[1].dest_point = best->second_mouvement.dest_point;

                if(gameState->die1 == gameState->die2)
                {

                        if(lastTimeError != 0)
                        {
                                printf("----- double ----\n");
                                printf("dés %d\t%d\n",gameState->die1,gameState->die2);
                                for(unsigned int i=0;i<28;i++)
                                {
                                        printf("|");
                                        if(best->board[i].player == ePlayer2) printf("-");
                                        printf("%d|",best->board[i].nb_checkers);
                                }
                                printf("\n");
                        }
                        /*on est obligé d'utiliser une autre list en cas de double
                         *                        sinon on perdrait la première et on ne pourrait pas la libérer
                         *                        et on ne peut pas la librérer avant car best pointe sur l'un de ses éléments*/
                        list_double = public_list_coups_possibles(best->board,gameState->die1,gameState->die2);

                        /*
                         *                        current = list_double;
                         *                        while(current != NULL && lastTimeError != 0)
                         *                        {
                         *                                printf("coup1 : %d to %d\tcoup2 : %d to %d\n",
                         *                                1+current->first_movement.src_point,1+current->first_movement.dest_point,
                         *                                1+current->second_mouvement.src_point,1+current->second_mouvement.dest_point);
                         *                                current = current->next;
                }
                */

                        if(list_double != NULL)
                        {
                                /*if(lastTimeError != 0)*/
                                list = du_simple_au_double(list);

                                best2 = eval(global_ressource,list_double);

                                moves[2].src_point = best2->first_movement.src_point;
                                moves[2].dest_point = best2->first_movement.dest_point;
                                moves[3].src_point = best2->second_mouvement.src_point;
                                moves[3].dest_point = best2->second_mouvement.dest_point;

                                global_jeu1 = memorise(global_jeu1,best2->board);
                        }
                }

                if(list_double == NULL) global_jeu1 = memorise(global_jeu1,best->board);
        }

        if(lastTimeError != 0)
        {
                printf("\n");
                printf("\t1er :%d toward %d\n",1+moves[0].src_point,1+moves[0].dest_point);
                printf("\t2nd :%d toward %d\n",1+moves[1].src_point,1+moves[1].dest_point);
                printf("\td- 1er :%d toward %d\n",1+moves[2].src_point,1+moves[2].dest_point);
                printf("\td- 2nd :%d toward %d\n",1+moves[3].src_point,1+moves[3].dest_point);
        }

        free_coups_possibles(list);
        free_coups_possibles(list_double);
}
