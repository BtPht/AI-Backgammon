#include "backgammon.h"
#include "possibles.h"

coups_possibles* public_liste_coups_possibles(const sZone board[28],const int die1,const int die2)
{

        coup1 *liste_possibles, *current1;
        coups_possibles *liste_finale=NULL;

        /*On etablit la liste des coups dé1 puis dé 2*/

        liste_possibles = coups_autorises1(board,die1);

        current1 = liste_possibles;

        while(current1 != NULL)
        {
                /*printf("\t1er : %d to %d\n",1+current1->first_movement.src_point,1+current1->first_movement.dest_point);*/
                current1->deuxieme_coup = coups_autorises2(current1->first_board,die2);
                current1 = current1->next;
        }

        liste_finale = liste_coups_possibles(liste_finale,liste_possibles);

        free_coup1(liste_possibles);

        /*Puis on étudie les coups en jouant les dés dans l'autre sens*/

        liste_possibles = coups_autorises1(board,die2);

        current1 = liste_possibles;

        while(current1 != NULL)
        {
                /*printf("\t2-1er : %d to %d\n",1+current1->first_movement.src_point,1+current1->first_movement.dest_point);*/
                current1->deuxieme_coup = coups_autorises2(current1->first_board,die1);
                current1 = current1->next;
        }

        liste_finale = liste_coups_possibles(liste_finale,liste_possibles);

        free_coup1(liste_possibles);

        return liste_finale;
}

coups_possibles* liste_coups_possibles(coups_possibles *liste_finale,const coup1 *liste1)
{
        int i;
        coup1 *coup1_current;
        coup2 *coup2_current;
        coups_possibles *current, *tail, *nouveau=NULL;

        /*on fait un ajout en queue donc on commence par trouver le dernier élément*/
        if(liste_finale == NULL)
                tail = NULL;
        else
        {
                current = liste_finale;
                while(current->next != NULL)
                {
                        current = current->next;
                }

                tail = current;
        }

        coup1_current = (coup1*) liste1;
        while(coup1_current != NULL)
        {
                coup2_current = coup1_current->deuxieme_coup;
                if(coup2_current == NULL)
                {
                        if(!redondant(coup1_current->first_board,liste_finale))
                        {
                                nouveau = (coups_possibles*)malloc(sizeof(coups_possibles));
                                        for(i=0;i<28;i++)
                                {
                                        nouveau->board[i].nb_checkers = coup1_current->first_board[i].nb_checkers;
                                        nouveau->board[i].player = coup1_current->first_board[i].player;
                                }
                                nouveau->first_movement.dest_point = coup1_current->first_movement.dest_point;
                                nouveau->first_movement.src_point = coup1_current->first_movement.src_point;
                                nouveau->second_mouvement.dest_point = -1;
                                nouveau->second_mouvement.src_point = -1;

                                nouveau->next = NULL;
                                if(tail == NULL)
                                {
                                        tail = nouveau;
                                        liste_finale = nouveau;
                                }
                                else
                                {
                                        tail->next = nouveau;
                                        tail = nouveau;
                                }
                        }
                }
                /* si il existe un deuxième coup possible */
                else while(coup2_current != NULL)
                {
                        if(!redondant(coup2_current->final_board,liste_finale))
                        {
                                nouveau = (coups_possibles*)malloc(sizeof(coups_possibles));

                                for(i=0;i<28;i++)
                                {
                                        nouveau->board[i].nb_checkers = coup2_current->final_board[i].nb_checkers;
                                        nouveau->board[i].player = coup2_current->final_board[i].player;
                                }

                                nouveau->first_movement.dest_point = coup1_current->first_movement.dest_point;
                                nouveau->first_movement.src_point = coup1_current->first_movement.src_point;

                                nouveau->second_mouvement.dest_point = coup2_current->second_mouvement.dest_point;
                                nouveau->second_mouvement.src_point = coup2_current->second_mouvement.src_point;

                                nouveau->next = NULL;
                                if(tail == NULL)
                                {
                                        tail = nouveau;
                                        liste_finale = nouveau;
                                }
                                else
                                {
                                        tail->next = nouveau;
                                        tail = nouveau;
                                }

                        }
                        coup2_current = coup2_current->next;
                }
                coup1_current = coup1_current->next;
        }

        return liste_finale;
}

coups_possibles* du_simple_au_double(coups_possibles *liste)
{
        coups_possibles *current=liste,*temp=NULL;
        int exist_double=0,exist_alone=0;

        /*le seul cas problématique est le cas ou il a à la fois des simples et des doubles
        donc tant qu'on n'a pas ce cas on parcour toute la liste*/
        while(current != NULL && !(exist_double && exist_alone))
        {
                if(current->second_mouvement.src_point == -1) exist_alone = 1;
                else exist_double = 1;

                current = current->next;
        }

        /*on sait qu(in existe au moins deux éléments dans la liste donc pas besoin de vérifier la nullité*/
        if(exist_double && exist_alone)
        {
                current= liste;
                /*on supprime la séquence de premiers éléments non valides*/
                while(current->second_mouvement.src_point == -1)
                {
                        liste = liste->next;
                        free(current);
                        current = liste;
                }

                /*puis on parcour la liste en supprimant tous les coups simples*/
                while(current->next != NULL)
                {
                        if(current->next->second_mouvement.src_point == -1)
                        {
                                temp = current->next;
                                current->next = current->next->next;

                                free(temp);
                        }
                        else current=current->next;
                }
        }

        return liste;
}

coup2* coups_autorises2(const sZone board[28],const int die)
{
        int i;

        coup2 *liste=NULL;
        sMove mouvement;

        if(board[ePos_BarP1].nb_checkers != 0)
        {/*il y au moins un pion prisonnier*/
                if(VERBOSE) printf("\tUn pion est bloqué : ");
                if(board[24-die].nb_checkers == 0 || board[24-die].player == ePlayer1)
                {/*sortie autorisée car pas de pions ou pions alliés*/
                        mouvement.src_point = ePos_BarP1;
                        mouvement.dest_point = 24-die;
                        liste = add_coup2(board,mouvement,liste,SIMPLE);
                        if(VERBOSE) printf("\tsortie simple\n");
                        /*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
                }
                else if(board[24-die].nb_checkers == 1)
                {/*il n'y a qu'un seul pion à l'adversaire*/
                        mouvement.src_point = ePos_BarP1;
                        mouvement.dest_point = 24-die;
                        liste = add_coup2(board,mouvement,liste,PRISE);
                        if(VERBOSE) printf("\tsortie en prenant un pion ennemi\n");
                        /*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
                }
                else if(VERBOSE) printf("\tpas de sortie possible\n");
        }
        else
        {/*il n'y a pas de pion prisonier*/

                for(i=0;i<24;i++)
                {/*on parcour tout le board pour trouver nos pions restants*/

                        if(board[i].player == ePlayer1 && board[i].nb_checkers != 0)
                        {/*cas ou le joueurest le bon et a des pions*/
                                if ((i-die)>=0)
                                {/*le dé tombe à l'intérieur du board*/
                                        if(VERBOSE) printf("On reste à l'intérieur \n");
                                        if(board[i-die].nb_checkers == 0 || board[i-die].player == ePlayer1)
                                        {//pas de pion ou des pions alliés
                                                if(VERBOSE) printf("\tOn fait un déplacement classique\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = i-die;
                                                liste = add_coup2(board,mouvement,liste,SIMPLE);
                                                /*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
                                        }
                                        else if(board[i-die].nb_checkers == 1)
                                        {//il n'y a qu'un seul pion à l'adversaire
                                                if(VERBOSE) printf("\tOn capture un pion ennemi\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = i-die;
                                                liste = add_coup2(board,mouvement,liste,PRISE);
                                                /*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
                                        }
                                        //le seul autre cas possible est plusieurs pions ennemis mais on ne peut alors pas jouer
                                }
                                else if( (i-die)< 0 && is_bear_off(board))
                                {//le dé fait sortir et la sortie est autorisée
                                        if(VERBOSE) printf("On sort du board\n");
                                        if(sortie_autorisee(board,i,die))
                                        {
                                                if(VERBOSE) printf("\tOn fait sortir un pion, cool!\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = ePos_OutP1;
                                                liste = add_coup2(board,mouvement,liste,SIMPLE);
                                                /*printf("\t\t2eme : %d to %d\n",1+mouvement.src_point,1+mouvement.dest_point);*/
                                        }
                                }
                        }

                }//fin de la boucle parcurrent tous les triangles
        }

        return liste;
}

coup2* add_coup2(const sZone origine[28], const sMove deplacement, coup2* liste,const type_deplacement type)
{
        int i;

        coup2 *nouveau = (coup2*)malloc(sizeof(coup2));

        //on créé le board final en prenant en compte le mouvement
        for(i=0;i<28;i++)
        {
                if(i == deplacement.src_point)
                {
                        nouveau->final_board[i].nb_checkers = origine[i].nb_checkers-1;
                        nouveau->final_board[i].player = origine[i].player;

                }
                else if(i == deplacement.dest_point)
                {
                        if(type == SIMPLE )
                        {
                                //le nombre de pions de la destination est incrémenté
                                nouveau->final_board[i].player = ePlayer1;
                                nouveau->final_board[i].nb_checkers = origine[i].nb_checkers + 1;
                        }
                        else if(type == PRISE)
                        {
                                //si c'est une prise il faut déplacer les pion adverse dans son BAR
                                nouveau->final_board[ePos_BarP2].nb_checkers = origine[ePos_BarP2].nb_checkers + 1;
                                nouveau->final_board[i].player = ePlayer1;
                                nouveau->final_board[i].nb_checkers = 1;
                        }
                }
                else
                {
                        //sinon on recopie les informations
                        nouveau->final_board[i].nb_checkers = origine[i].nb_checkers;
                        nouveau->final_board[i].player = origine[i].player;
                }
        }

        //on enregistre le mouvement
        nouveau->second_mouvement.src_point = deplacement.src_point;
        nouveau->second_mouvement.dest_point = deplacement.dest_point;

        //on relie au reste de la chaine

        /*** L'ajout se fait en tête par simplicité mais n'a pour l'instant pas d'importance ***/
        nouveau->next = liste;

        return nouveau;

}

coup1* coups_autorises1(const sZone board[28],const int die)
{
        int i;

        coup1 *liste=NULL;
        sMove mouvement;

        if(board[ePos_BarP1].nb_checkers != 0)
        {//il y au moins un pion prisonnier
                if(VERBOSE) printf("\tUn pion est bloqué : ");
                if(board[24-die].nb_checkers == 0 || board[24-die].player == ePlayer1)
                {//sortie autorisée car pas de pions ou pions alliés
                        mouvement.src_point = ePos_BarP1;
                        mouvement.dest_point = 24-die;
                        liste = add_coup1(board,mouvement,liste,SIMPLE);
                        if(VERBOSE) printf("\tsortie simple\n");
                }
                else if(board[24-die].nb_checkers == 1)
                {//il n'y a qu'un seul pion à l'adversaire
                        mouvement.src_point = ePos_BarP1;
                        mouvement.dest_point = 24-die;
                        liste = add_coup1(board,mouvement,liste,PRISE);
                        if(VERBOSE) printf("\tsortie en prenant un pion ennemi\n");
                }
                else if(VERBOSE) printf("\tpas de sortie possible\n");
        }
        else
        {//il n'y a pas de pion prisonier

                for(i=0;i<24;i++)
                {//on parcour tout le board pour trouver nos pions restants

                        if(board[i].player == ePlayer1 && board[i].nb_checkers != 0)
                        {//cas ou le joueurest le bon et a des pions
                                if ((i-die)>=0)
                                {//le dé tombe à l'intérieur du board
                                        if(VERBOSE) printf("On reste à l'intérieur \n");
                                        if(board[i-die].nb_checkers == 0 || board[i-die].player == ePlayer1)
                                        {//pas de pion ou des pions alliés
                                                if(VERBOSE) printf("\tOn fait un déplacement classique\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = i-die;
                                                liste = add_coup1(board,mouvement,liste,SIMPLE);
                                        }
                                        else if(board[i-die].nb_checkers == 1)
                                        {//il n'y a qu'un seul pion à l'adversaire
                                                if(VERBOSE) printf("\tOn capture un pion ennemi\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = i-die;
                                                liste = add_coup1(board,mouvement,liste,PRISE);
                                        }
                                        //le seul autre cas possible est plusieurs pions ennemis mais on ne peut alors pas jouer
                                }
                                else if( (i-die)< 0 && is_bear_off(board))
                                {//le dé fait sortir et la sortie est autorisée
                                        if(VERBOSE) printf("On sort du board\n");
                                        if(sortie_autorisee(board,i,die))
                                        {
                                                if(VERBOSE) printf("\tOn fait sortir un pion, cool!\n");
                                                mouvement.src_point = i;
                                                mouvement.dest_point = ePos_OutP1;
                                                liste = add_coup1(board,mouvement,liste,SIMPLE);
                                        }
                                        else if(VERBOSE) printf("pas de sortie possible\n");
                                }
                        }

                }//fin de la boucle parcurrent tous les triangles
        }

        return liste;
}


coup1* add_coup1(const sZone origine[28], const sMove deplacement, coup1* liste,const type_deplacement type)
{
        int i;

        coup1 *nouveau = (coup1*)malloc(sizeof(coup1));

        //on créé le board final en prenant en compte le mouvement
        for(i=0;i<28;i++)
        {
                if(i == deplacement.src_point)
                {
                        nouveau->first_board[i].nb_checkers = origine[i].nb_checkers-1;
                        nouveau->first_board[i].player = origine[i].player;

                }
                else if(i == deplacement.dest_point)
                {
                        if(type == SIMPLE )
                        {
                                //le nombre de pions de la destination est incrémenté
                                nouveau->first_board[i].player = ePlayer1;
                                nouveau->first_board[i].nb_checkers = origine[i].nb_checkers + 1;
                        }
                        else if(type == PRISE)
                        {
                                //si c'est une prise il faut déplacer les pion adverse dans son BAR
                                nouveau->first_board[ePos_BarP2].nb_checkers = origine[ePos_BarP2].nb_checkers + 1;
                                nouveau->first_board[i].player = ePlayer1;
                                nouveau->first_board[i].nb_checkers = 1;
                        }
                }
                else
                {
                        //sinon on recopie les informations
                        nouveau->first_board[i].nb_checkers = origine[i].nb_checkers;
                        nouveau->first_board[i].player = origine[i].player;
                }
        }

        //on enregistre le mouvement
        nouveau->first_movement.src_point = deplacement.src_point;
        nouveau->first_movement.dest_point = deplacement.dest_point;

        nouveau->deuxieme_coup = NULL;

        //on relie au reste de la chaine

        /*** L'ajout se fait en tête parcoups_possibles *liste simplicité mais n'a pour l'instant pas d'importance ***/
        nouveau->next = liste;

        return nouveau;

}

int sortie_autorisee(const sZone board[28],const int pos,const int die)
{
        int m;

        if(pos-die == -1) return 1;
        else
        {
                if(board[ePos_6].nb_checkers == 0 || board[ePos_6].player == ePlayer2)
                        if(board[ePos_5].nb_checkers == 0 || board[ePos_5].player == ePlayer2)
                                if(board[ePos_4].nb_checkers == 0 || board[ePos_4].player == ePlayer2)
                                        if(board[ePos_3].nb_checkers == 0 || board[ePos_3].player == ePlayer2)
                                                if(board[ePos_2].nb_checkers == 0 || board[ePos_2].player == ePlayer2)
                                                        m=0;
                                                else m=1;
                                        else m=2;
                                else m=3;
                        else m=4;
                else m=6;

                if(pos == m) return 1;
                else return 0;
        }
}

int redondant(const sZone board[28],const coups_possibles *liste)
{
        coups_possibles *current = (coups_possibles*)liste;
        int i;
        while(current != NULL)
        {
                i=0;
                while((i<28 && current->board[i].nb_checkers == board[i].nb_checkers)
                        && current->board[i].player == board[i].player)
                {
                        i++;
                }
                /*si i=28 c'est qu'on a parcouru le board sans trouver de différence donc board est redondant*/
                if(i == 28) return 1;
                else current = current->next;
        }
        /*si on a parcourut toute la liste board n'est pas redondant*/
        return 0;
}

void free_coup2(coup2 *liste)
{
        if(liste->next != NULL)
                free_coup2(liste->next);

        free(liste);
}

void free_coup1(coup1 *liste)
{
        if(liste != NULL)
        {
                if(liste->deuxieme_coup != NULL)
                        free_coup2(liste->deuxieme_coup);

                if(liste->next != NULL)
                        free_coup1(liste->next);

                free(liste);
        }
}

void free_coups_possibles(coups_possibles *liste)
{
        if(liste != NULL)
        {
                if(liste->next != NULL)
                        free_coups_possibles(liste->next);

                free(liste);
        }
}


