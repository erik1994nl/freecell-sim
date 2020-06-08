#include "stdafx.h"
#include "Table.h"
#include <iostream>
#include "stdlib.h"
#include <time.h>
#include <math.h>
#include <algorithm>

void TableClass::setupTable(TableClass::Table &t) {
	Timer timer(__FUNCTION__);
	srand(time(NULL));
	int r{};
	// Loop over every card to place
	for (int i{}; i < NUMBER_OF_CARDS; i++) {
		r = rand() % NUMBER_OF_CARDS;
		while (t.spots[r]) {
			r = rand() % NUMBER_OF_CARDS;
		}

		// Place card in random spot
		t.spots[r] = (unsigned long long)1 << i;
	}
}

void TableClass::updatePossibleMoves(TableClass::Table &t) {
	Timer timer(__FUNCTION__);
	std::vector<int> movableCards{};
	getMovableCards(movableCards, t);

	getPossibleMoves(t, movableCards);
}

void TableClass::getMovableCards(std::vector<int>& cards, TableClass::Table& t) {
	bool skipRestOfColumn[TABLE_ROW] = { 0 };
	for (int i = REGULAR_SPOTS - 1; i >= 0; i--) {
		// Spot does not contain card
		if (!t.spots[i]) {
			continue;
		}

		// Spot is not bottom card AND has incompatible lower card
		if (t.spots[i + TABLE_ROW] && !cardsAreCompatible(t.spots[i + TABLE_ROW], t.spots[i])) {
			skipRestOfColumn[i%TABLE_ROW] = true;
			continue;
		}

		// Already found incompatible card in this column, hence skip rest of column
		if (skipRestOfColumn[i%TABLE_ROW]) {
			continue;
		}

		// Check if you are allowed to move this number of cards at once
		int cardsToMove{};
		while (t.spots[i + cardsToMove * TABLE_ROW]) {
			cardsToMove++;
		}
		if (cardsToMove > (std::count(t.spots.begin() + FIRST_FREECELL_SPOT, t.spots.begin() + FIRST_FREECELL_SPOT + FREECELLS, 0) + 1)) {
			skipRestOfColumn[i%TABLE_ROW] = true;
			continue;
		}

		// Spot is bottom card
		if (!t.spots[i + TABLE_ROW]) {
			t.bottomCards[i % TABLE_ROW] = i;
		}

		// Spot is either bottom card or card with compatible lower card
		cards.push_back(i);
	}

	// Add freecell cards
	for (int i = FIRST_FREECELL_SPOT; i < FIRST_FREECELL_SPOT + FREECELLS; i++) {
		if (t.spots[i]) {
			cards.push_back(i);
		}
	}
}

void TableClass::getPossibleMoves(TableClass::Table& t, std::vector<int>& movableCards) {
	t.possibleMoves.clear();
	for (int i{}; i < movableCards.size(); i++) {

		t.spots[movableCards[i]]; // Card to consider
		for (int j{}; j < t.bottomCards.size(); j++) {
			// Card to consider is same as Bottom card
			if (t.spots[movableCards[i]] & t.spots[t.bottomCards[j]]) {
				continue;
			}

			// Card to consider and bottom card are same color
			// TODO: This check overlaps next check, but might save computation time. Verify this
			if (!((t.spots[movableCards[i]] | t.spots[t.bottomCards[j]]) & BLACK_CARDS &&
				(t.spots[movableCards[i]] | t.spots[t.bottomCards[j]]) & RED_CARDS)) {
				continue;
			}

			// Card to consider is compatible with bottom card
			if (cardsAreCompatible(t.spots[movableCards[i]], t.spots[t.bottomCards[j]])) {
				t.possibleMoves.emplace_back(std::make_pair(movableCards[i], t.bottomCards[j] + TABLE_ROW));
			}
		}

		// Card to consider can move to final stack
		for (int j{}; j < FINAL_STACKS; j++) {
			if (t.spots[movableCards[i]] <= 8) {
				if (!t.spots[FIRST_FINAL_STACK_SPOT + j] && log2(t.spots[movableCards[i]]) == j) {
					t.possibleMoves.emplace_back(std::make_pair(movableCards[i], FIRST_FINAL_STACK_SPOT + j));
					continue;
				}
			}
			else {
				if ((t.spots[FIRST_FINAL_STACK_SPOT + j] << 4) == t.spots[movableCards[i]]) {
					t.possibleMoves.emplace_back(std::make_pair(movableCards[i], FIRST_FINAL_STACK_SPOT + j));
					continue;
				}
			}
		}

		// Card to consider can move to freecell
		for (int j{}; j < FREECELLS; j++) {
			if (!t.spots[FIRST_FREECELL_SPOT + j]) {
				t.possibleMoves.emplace_back(std::make_pair(movableCards[i], FIRST_FREECELL_SPOT + j));
			}
		}
	}
}

bool::TableClass::cardsAreCompatible(const unsigned long long &lowerCard, const unsigned long long &upperCard) {
	// Lower card is 1 value lower than upper card AND a different color
	int color = log2(lowerCard);
	// Black
	if (!(color % 2)) {
		if (upperCard & (lowerCard << (5 - (color % 4))) ||
			upperCard & (lowerCard << (7 - (color % 4)))) {
			return true;
		}
	}
	// Red
	else {
		if (upperCard & (lowerCard << (4 - (color % 4))) ||
			upperCard & (lowerCard << (6 - (color % 4)))) {
			return true;
		}
	}
	return false;
}

void TableClass::makeMove(TableClass::Table& t) {
	// Always do random possible move
	int r = rand() % t.possibleMoves.size();
	std::iter_swap(t.spots.begin() + t.possibleMoves[r].first, t.spots.begin() + t.possibleMoves[r].second);
	std::cout << "Moving from " << t.possibleMoves[r].first << " to " << t.possibleMoves[r].second << "\n";
}

void TableClass::checkIfGameIsFinished(TableClass::Table& t, TableClass::Stats& s) {
	// No more possible moves
	if (!t.possibleMoves.size()) {
		s.gameIsFinished = true;
	}
}

void TableClass::checkIfWon(TableClass::Table& t, TableClass::Stats& s) {
	// Final stacks complete
	auto atLeastKing = [](unsigned long long card) {return card > (1 << 48); };
	if (std::all_of(t.spots.begin() + FIRST_FINAL_STACK_SPOT, t.spots.begin() + FIRST_FINAL_STACK_SPOT + FINAL_STACKS, atLeastKing )) {
		s.gameIsWon = true;
	}
}

void TableClass::tableToCompact(TableClass::Table& t, TableClass::CompactTable& ct) {
	// 13x12 rectangle of unsigned long longs

	// Reset compactSpots to zero
	std::fill(ct.compactSpots.begin(), ct.compactSpots.end(), 0);
	
	// Initialize first row/col
	ct.compactSpots[0] = t.spots[0];
	ct.compactSpots[13] = t.spots[0];

	int rowCounter{}, colCounter{};
	for (int i = 1; i < t.spots.size(); i++) {
		if (!(i%12)) {
			rowCounter++;
			colCounter = 0;
		}
		
		ct.compactSpots[rowCounter] = ct.compactSpots[rowCounter] | t.spots[i];
		ct.compactSpots[colCounter + 13] = ct.compactSpots[colCounter + 13] | t.spots[i];
		colCounter++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Print functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TableClass::printFilledSpots(TableClass::Table &t) {
	for (int i{}; i < TOTAL_SPOTS; i++) {
		if (t.spots[i]) {
			std::cout << "#";
		}

		if (i > 0 && !((i + 1) % TABLE_ROW)) {
			std::cout << std::endl;
		}
	}
}

void TableClass::printPossibleMoves(TableClass::Table& t) {
	std::cout << "Possible moves: " << std::endl;
	for (const auto& move : t.possibleMoves) {
		std::cout << "from: " << move.first << " to: " << move.second << std::endl;
	}
}

void TableClass::printResult(TableClass::Stats& s) {
	std::cout << "You have " << (s.gameIsWon ? "won" : "lost") << " the game." << std::endl;
	if (s.turns == 200) {
		std::cout << "The game has ended automatically after 200 turns." << std::endl;
	}
	else {
		std::cout << "The game took " << s.turns << " turns" << std::endl;
	}
}