#include <stdio.h>
#include <stdlib.h>

#include "backgammon.h"

#include "possibles.h"
#include "evaluation.h"

int main(void)
{
	int i,die1=5,die2=2;
	int sens=0;
	float note=0;
	sZone board_test[28];
	 *possibles_test=NULL, *courant,*best;
	decatree *lol=NULL;

	b_ligne ligne;

	FILE* fichier;
	fichier = fopen("fuuu.bg","rwb+");

	if(fichier == NULL) printf("file not found\n");
	for(i=0;i<28;i++)
	{
		plateau_test[i].player = ePlayer1;
		plateau_test[i].nb_checkers = 0;
	}

	/*plateau_test[ePos_1].nb_checkers = 4;
	plateau_test[ePos_2].nb_checkers = 10;
	plateau_test[ePos_14].nb_checkers = 1;

	plateau_test[ePos_5].player = ePlayer2;
	plateau_test[ePos_6].player = ePlayer2;
	plateau_test[ePos_9].player = ePlayer2;
	plateau_test[ePos_12].player = ePlayer2;
	plateau_test[ePos_12].nb_checkers = 1;
	plateau_test[ePos_13].player = ePlayer2;
	plateau_test[ePos_16].player = ePlayer2;
	plateau_test[ePos_17].player = ePlayer2;
	plateau_test[ePos_19].nb_checkers = 1;
	plateau_test[ePos_19].player = ePlayer2;
	plateau_test[ePos_21].player = ePlayer2;
	plateau_test[ePos_22].player = ePlayer2;
	plateau_test[ePos_22].nb_checkers = 8;
	plateau_test[ePos_23].player = ePlayer2;
	plateau_test[ePos_23].nb_checkers = 5;
	plateau_test[26].player = ePlayer2;
	plateau_test[27].player = ePlayer2;

	plateau_test[ePos_BarP1].nb_checkers = 2;
	plateau_test[ePos_OutP1].nb_checkers = 2;*/

	plateau_test[ePos_24].player = ePlayer2;
	plateau_test[ePos_24].nb_checkers = 5;

	plateau_test[ePos_BarP1].nb_checkers = 3;
	plateau_test[ePos_OutP2].nb_checkers = 2;

	add_plateau(fichier,plateau_test,ePlayer2);
	add_plateau(fichier,plateau_test,ePlayer1);
	add_plateau(fichier,plateau_test,ePlayer1);
	printf("board added\n");

	lol = create_decatree(fichier);
	printf("decatree created\n");


	if(existe2(lol,plateau_test,&note,&sens) == existe(fichier,plateau_test,&note,&sens))
		printf("--- ok ---\n");
	else printf("--- error ---\n");

	if(existe2(lol,plateau_test,&note,&sens))
		printf("exists : \t%lf\t%d\n",note,sens);
	if(existe(fichier,plateau_test,&note,&sens))
		printf("2- exists : \t%lf\t%d\n",note,sens);

	fclose(fichier);

	return 0;
}
