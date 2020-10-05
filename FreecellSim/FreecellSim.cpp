#include "stdafx.h"
#include "Table.h"
#include <iostream>

int main()
{
	int numberOfGames = 10;
	for (int n{}; n < numberOfGames; n++) {
		Timer timer(__FUNCTION__);
		TableClass T;
		TableClass::Table t;
		TableClass::CompactTable ct;
		TableClass::Stats s;

		TableClass::MoveType strategy = TableClass::MoveType::Fancy;
		TableClass::ScoringSystem scoringSystem = TableClass::ScoringSystem::finalStackIsTen;
		T.initializeStrategy(s);

		T.setupTable(t);
		T.updatePossibleMoves(t);
		//T.regularToCompact(t, ct);
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
		T.checkIfWon(t, s);
		T.printResult(s);

		T.saveStrategy(s);
	}



	//T.printFilledSpots(t);

	return 0;
}

