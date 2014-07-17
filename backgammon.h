#ifndef BACKGAMMON_H_INCLUDED
#define BACKGAMMON_H_INCLUDED

#include <stdbool.h>

/**
* @name ePlayer
* @typedef enum
* @details Enumerate the players
*/
typedef enum
{
        ePlayer1,	// Player 1 -> The current player
        ePlayer2	// Player 2 -> The opponent
} ePlayer;

/**
* @name sZone
* @typedef struct
* @details Strcut representing the zones of the game
*/
typedef struct
{
        ePlayer player;			// Owner of the checkers
        unsigned int nb_checkers;	// Number of checkers
} sZone;

/**
* @name ePosition
* @typedef enum
* @details Enumerate the zones on the board
*/
typedef enum
{
        ePos_1=0,
        ePos_2,
        ePos_3,
        ePos_4,
        ePos_5,
        ePos_6,
        ePos_7,
        ePos_8,
        ePos_9,
        ePos_10,
        ePos_11,
        ePos_12,
        ePos_13,
        ePos_14,
        ePos_15,
        ePos_16,
        ePos_17,
        ePos_18,
        ePos_19,
        ePos_20,
        ePos_21,
        ePos_22,
        ePos_23,
        ePos_24,
        ePos_OutP1,
        ePos_BarP1,
        ePos_OutP2,
        ePos_BarP2
} ePosition;

/**
* @name sGameState
* @typedef struct
* @details Struct representing the current state of the game
*/
typedef struct
{
        sZone zones[28];                // Array Containing the zones where checkers can be
        unsigned int die1,die2;
        unsigned int scoreP1,scoreP2;	// Player's scores
        unsigned int bet;
} sGameState;

/**
* @name sMove
* @typedef struct
* @details Struct representing a move
*/
typedef struct
{
        ePosition src_point;
        ePosition dest_point;
} sMove;

/**
* @note Fonctions visible outside the library
*
* @note These functions must be implemented by the creator of the IA
*
* @note These functions must be called by the user of the library
*
* @warning Any error in the call order will lead to undefined behavior
*
* @note The signatures commented are the ones the IA must implement
* the typedefs are only used by the caller of the library
*/

/**
* @name initLibrary
* @details Initialize the library
* @param char name[50]
*	name associated to the library
*/
//void initLibrary(char name[50]);
typedef void (*fpInitLibrary)(char[50]);

/**
* @name startMatch
* @details Initialize the IA for a full match
* @param const unsigned int target_score
*	number of rounds to win the match
*/
//void startMatch(const unsigned int target_score);
typedef void (*fpStartMatch)(const unsigned int);

/**
* @name startGame
* @details Initialize the IA for a round
*/
//void startGame();
typedef void (*fpStartGame)();

/**
* @name endGame
* @details Notify the end of a round
* @remarks The signature of this function doesn't include the winner of the round
* the IA can still know who won by remembering its last move
*
* @note The signature may however change in a later update
*
*/
//void endGame();
typedef void (*fpEndGame)();

/**
* @name endMatch
* @details Notify the end of a match
* @remarks The signature of this function doesn't include the winner of the match
* but the IA can still know who won by remembering its last move and score
*
* @note The signature may however change in a later update
*
*/
//void endMatch();
typedef void (*fpEndMatch)();

/**
* @name doubleStack
* @details Ask if the IA wants to double the bet
* @param const sGameState * const gameState
*	current state of the game
* @return bool
*	true if the player wants to double the bet
*/
//bool doubleBet(const sGameState * const gameState);
typedef bool (*fpDoubleStack)(const sGameState * const);

/**
* @name tkeDouble
* @details Accept or refuse the new bet
* @param const sGameState * const gameState
*	current state of the game
* @return bool
*	True if the IA accept the new bet
*/
//bool takeDouble(const sGameState * const gameState);
typedef bool (*fpTakeDouble)(const sGameState * const);

/**
* @name makeDecision
* @details Ask the IA its moving choices
* @param const sGameState * const gameSMtate
*	Current state of the game
* @param sMove moves[4]
*	Array of the moves
* @param unsigned int lastTimeError
*	True if the last move triggered an error (i.e illegal move)
* @warning The moves parameter gives the IA access to a part of the memomry
* which belongs to the library users. This is too dangerous and the
* signature MUST be changed
*/
//void makeDecision(const sGameState * const gameState, sMove moves[4], unsigned int lastTimeError);
typedef void (*fpMakeDecision)(const sGameState * const, sMove[4], unsigned int);

#endif

