IA-Backgammon
=============

This code is from an educational project I did few years ago. This is a legacy
code that needs serious cleaning and refactoring. It would not be wise to use
it as it.

Three distinct parts can be identified :
        1- The interface defining how an external code should use this IA
                * backgammon.h
        2- Functions creating a list of all the movements possibles for a
        player at a given state of the game. I tried to include all the side
        cases Backgammon can lead to. A good knowledge of the rules is
        probably necessary to fully understand the code.
                * possible.h
                * possible.c
        3- A code implementing of an IA not smart at all but where I focused
        on one thing : being able to memorize and access any board played
        in previous games with the very strict objective of a constant
        complexity. The memory of the boards is represented as a tree where
        each node is actually a zone of the board, the next leave can be
        accessed depending of the number of checkers in the current zone.
        The game can be seen as symmetrical, and so is the reading of the
        tree. As the access is in constant complexity it can be read two times.
        The final leaves contain the number of victories and fails to help
        the IA making a choice. To be noted that this solution is technically
        impossible to use as the size of the tree will very quickly be larger
        than the central memory.
