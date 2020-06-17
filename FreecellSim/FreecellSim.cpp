#include "stdafx.h"
#include "Table.h"
#include <iostream>

int main()
{
	Timer timer(__FUNCTION__);
	TableClass T;
	TableClass::Table t;
	TableClass::CompactTable ct;
	TableClass::Stats s;

	TableClass::MoveType strategy = TableClass::MoveType::Random;
	TableClass::ScoringSystem scoringSystem = TableClass::ScoringSystem::finalStackIsTen;

	T.setupTable(t);
	T.initializeStrategy(s);
	T.updatePossibleMoves(t);
	T.regularToCompact(t, ct);
	//T.printPossibleMoves(t);

	// Game loop
	while (!s.gameIsFinished && s.turns < 200) {
		std::cout << "turn: " << s.turns << std::endl;
		T.makeMove(t, strategy, s);
		T.updateScore(t, s, scoringSystem);
		T.updatePossibleMoves(t);
		//T.printPossibleMoves(t);
		T.checkIfGameIsFinished(t, s);
		s.turns++;
		//std::cout << "-------------------------------------------------" << std::endl;
	}

	T.saveStrategy(s);

	T.checkIfWon(t, s);
	T.printResult(s);

	//T.printFilledSpots(t);

	timer.getCurrentDuration();
	std::cin.get();
	return 0;
}

