#include "stdafx.h"
#include "Table.h"
#include <iostream>

int main()
{
	Timer timer(__FUNCTION__);
	TableClass T;
	TableClass::Table t;
	TableClass::Stats s;

	T.setupTable(t);
	T.updatePossibleMoves(t);
	T.printPossibleMoves(t);

	while (!s.gameIsFinished && s.turns < 200) {
		std::cout << "turn: " << s.turns << std::endl;
		T.makeMove(t);
		T.updatePossibleMoves(t);
		T.printPossibleMoves(t);
		T.checkIfGameIsFinished(t, s);
		s.turns++;
		std::cout << "-------------------------------------------------" << std::endl;
	}

	T.checkIfWon(t,s);
	T.printResult(s);
	
	//T.printFilledSpots(t);
	                          
	timer.getCurrentDuration();
	std::cin.get();
    return 0;
}

