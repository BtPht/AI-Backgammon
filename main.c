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
	SZone plateau_test[28];
	coups_possibles *possibles_test=NULL, *courant,*best;
	decatree *lol=NULL;
	
	b_ligne ligne;
	
	FILE* fichier;
	fichier = fopen("fuuu.bg","rwb+");

	if(fichier == NULL) printf("pas de fichier trouvé\n");
	for(i=0;i<28;i++)
	{
		plateau_test[i].player = EPlayer1 ;
		plateau_test[i].nb_checkers = 0 ;
	}
	
	/*plateau_test[EPos_1].nb_checkers = 4 ;
	plateau_test[EPos_2].nb_checkers = 10 ;
	plateau_test[EPos_14].nb_checkers = 1 ;
	
	plateau_test[EPos_5].player = EPlayer2 ;
	plateau_test[EPos_6].player = EPlayer2 ;
	plateau_test[EPos_9].player = EPlayer2 ;
	plateau_test[EPos_12].player = EPlayer2 ;
	plateau_test[EPos_12].nb_checkers = 1 ;
	plateau_test[EPos_13].player = EPlayer2 ;
	plateau_test[EPos_16].player = EPlayer2 ;
	plateau_test[EPos_17].player = EPlayer2 ;
	plateau_test[EPos_19].nb_checkers = 1 ;
	plateau_test[EPos_19].player = EPlayer2 ;
	plateau_test[EPos_21].player = EPlayer2 ;
	plateau_test[EPos_22].player = EPlayer2 ;	
	plateau_test[EPos_22].nb_checkers = 8 ;
	plateau_test[EPos_23].player = EPlayer2 ;	
	plateau_test[EPos_23].nb_checkers = 5 ;
	plateau_test[26].player = EPlayer2 ;
	plateau_test[27].player = EPlayer2 ;
	
	plateau_test[EPos_BarP1].nb_checkers = 2 ;
	plateau_test[EPos_OutP1].nb_checkers = 2 ;*/
	
	plateau_test[EPos_24].player = EPlayer2 ;	
	plateau_test[EPos_24].nb_checkers = 5 ;
	
	plateau_test[EPos_BarP1].nb_checkers = 3 ;
	plateau_test[EPos_OutP2].nb_checkers = 2 ;

	add_plateau(fichier,plateau_test,EPlayer2);
	add_plateau(fichier,plateau_test,EPlayer1);
	add_plateau(fichier,plateau_test,EPlayer1);
	printf("plateau ajouté\n");
	
	lol = create_decatree(fichier);
	printf("decatree créé\n");
	
	
	if(existe2(lol,plateau_test,&note,&sens) == existe(fichier,plateau_test,&note,&sens))
		printf("--- ok ---\n");
	else printf("--- erreur ---\n");
	
	if(existe2(lol,plateau_test,&note,&sens))
		printf("existe : \t%lf\t%d\n",note,sens);
	if(existe(fichier,plateau_test,&note,&sens))
		printf("2- existe : \t%lf\t%d\n",note,sens);

	fclose(fichier);
	
	return 0;
}
