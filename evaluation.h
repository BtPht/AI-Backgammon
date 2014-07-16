#ifndef EVALUATION_H_INCLUDED
	#define EVALUATION_H_INCLUDED

typedef struct plateaux_joues
{
	SZone  plateau[28];
	struct plateaux_joues* suivant;
}plateaux_joues;

typedef struct
{
	signed char triangle[28];
	unsigned int win;
	unsigned int loose;
}b_ligne;

typedef struct decatree
{
	struct decatree *suivant[13];
	unsigned int win;
	unsigned int loose;
}decatree;

decatree *create_decatree(FILE* ressource);
decatree *add_ligne(decatree *arbre,b_ligne *ligne,int prof);

coups_possibles *eval(FILE* ressource, const coups_possibles *liste);

int existe2(decatree *arbre, const SZone plateau[28], float *note, int *sens);
int existe(FILE *ressource, const SZone plateau[28], float *note,int *sens);

void add_plateau(FILE* ressource, const SZone plateau[28], const EPlayer gagnant);
plateaux_joues *memorise(plateaux_joues *liste, const SZone plateau[28]);

void free_memorise(plateaux_joues *liste);

/* TODO

	commenter les .h

*/

#endif
