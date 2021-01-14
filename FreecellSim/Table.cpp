#include "stdafx.h"
#include "Table.h"
#include <iostream>
#include "stdlib.h"
#include <time.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>

void TableClass::setupTable(TableClass::Table& t) {
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

void TableClass::initializeStrategy(TableClass::Stats& s) {
	char filePathIncFile[MAX_PATH];
	GetModuleFileNameA(NULL, filePathIncFile, MAX_PATH);
	std::string::size_type pos = std::string(filePathIncFile).find_last_of("\\/");
	std::string filePath = std::string(filePathIncFile).substr(0, pos);
	
	//std::string inFilePath = filePath + "\\data.txt";

	//std::string line;
	//std::ifstream infile(inFilePath, std::ifstream::binary);
	//infile.seekg(0, infile.end);
	//long long size = infile.tellg();

	//if (size == -1) {
	//	// File does not exist, do not initialize
	//	return;
	//}
	//infile.seekg(0);
	//char* buffer = new char[size+1];
	//infile.read(reinterpret_cast<char*>(&buffer), size);

	//s.score = reinterpret_cast<int>(std::move(buffer));






	std::string inFilePath = filePath + "\\strat.bin";
	FILE* pStratFile;
	pStratFile = fopen(inFilePath.c_str(), "rb");

	if (pStratFile == NULL) {
		return;
	}

	// Read number of moves
	int numberOfMoves{};
	fread(&numberOfMoves, sizeof(int), 1, pStratFile);
	// Read moves and values
	std::vector<std::pair<int, int>> moves{};
	std::pair<int, int> m{};
	int v{};
	for (int i{}; i < numberOfMoves; i++) {
		fread(&m, sizeof(int) * 2, 1, pStratFile);
		fread(&v, sizeof(int), 1, pStratFile);
		s.moveValues[m] = v;
	}

	fclose(pStratFile);
}

void TableClass::saveStrategy(TableClass::Stats& s) {
	char filePathIncFile[MAX_PATH];
	GetModuleFileNameA(NULL, filePathIncFile, MAX_PATH);
	std::string::size_type pos = std::string(filePathIncFile).find_last_of("\\/");
	std::string filePath = std::string(filePathIncFile).substr(0, pos);

	std::string fileToWrite = filePath + "\\strat.bin";
	FILE* pStratFile;
	pStratFile = fopen(fileToWrite.c_str(), "wb");
	// Write number of moves
	auto numberOfMoves = s.moveValues.size();
	fwrite(&numberOfMoves, sizeof(size_t), 1, pStratFile);
	// Write moves and values
	for (auto const& [move, value] : s.moveValues) {
		fwrite(&move, sizeof(move), 1, pStratFile);
		fwrite(&value, sizeof(int), 1, pStratFile);
	}
	fclose(pStratFile);
}

void TableClass::getTableDistance(TableClass::Table& t, TableClass::CompactTable& ct) {
	std::array<int, 8> cardsInColumn{};
	for (int col{}; col < TABLE_ROW; col++) {
		// Get number of cards in column
		cardsInColumn[col] = ((t.bottomCards[col] - col) / TABLE_ROW) + 1;

		for (int row{}; row < cardsInColumn[col]; row++) {
			// Table distance is sum of distance of every card
			// card distance is steps to endpile * card penalty

			unsigned long long card = ct.compactSpots[col] & ct.compactSpots[row + TABLE_ROW];
			int penalty{};

			auto suit = getSuit(card);
			
			
			//while(card > t.spots[FIRST_FINAL_STACK_SPOT + //color])
		}

	}
	auto wait = 5;
}

void TableClass::updatePossibleMoves(TableClass::Table& t) {
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
			skipRestOfColumn[i % TABLE_ROW] = true;
			continue;
		}

		// Already found incompatible card in this column, hence skip rest of column
		if (skipRestOfColumn[i % TABLE_ROW]) {
			continue;
		}

		// Check if you are allowed to move this number of cards at once
		int cardsToMove{};
		while (t.spots[i + cardsToMove * TABLE_ROW]) {
			cardsToMove++;
		}
		if (cardsToMove > (std::count(t.spots.begin() + FIRST_FREECELL_SPOT, t.spots.begin() + FIRST_FREECELL_SPOT + FREECELLS, 0) + 1)) {
			skipRestOfColumn[i % TABLE_ROW] = true;
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

unsigned long long TableClass::getOneCardValueLower(unsigned long long& card) {
	return card >> 4;
}

unsigned long long TableClass::getOneCardValueHigher(unsigned long long& card) {
	return card << 4;
}

void TableClass::getPossibleMoves(TableClass::Table& t, std::vector<int>& movableCards) {
	t.possibleMoves.clear();
	for (int i{}; i < movableCards.size(); i++) {

		unsigned long long cardToConsider = t.spots[movableCards[i]];
		for (int j{}; j < t.bottomCards.size(); j++) {
			// Card to consider is same as Bottom card
			if (cardToConsider & t.spots[t.bottomCards[j]]) {
				continue;
			}

			// Card to consider and bottom card are same color
			// TODO: This check overlaps next check, but might save computation time. Verify this
			if (!((cardToConsider | t.spots[t.bottomCards[j]]) & BLACK_CARDS &&
				(cardToConsider | t.spots[t.bottomCards[j]]) & RED_CARDS)) {
				continue;
			}

			// Card to consider is compatible with bottom card
			if (cardsAreCompatible(cardToConsider, t.spots[t.bottomCards[j]])) {
				t.possibleMoves.emplace_back(std::make_pair(movableCards[i], t.bottomCards[j] + TABLE_ROW));
			}
		}

		// Card to consider can move to final stack
		for (int j{}; j < FINAL_STACKS; j++) {
			// Ace
			if (cardToConsider <= 8) {
				if (!t.spots[FIRST_FINAL_STACK_SPOT + j] && log2(cardToConsider) == j) {
					t.possibleMoves.emplace_back(std::make_pair(movableCards[i], FIRST_FINAL_STACK_SPOT + j));
					continue;
				}
			}
			// Non-ace
			else {
				if (t.spots[FIRST_FINAL_STACK_SPOT + j] == getOneCardValueLower(cardToConsider)) {
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

bool::TableClass::cardsAreCompatible(const unsigned long long& lowerCard, const unsigned long long& upperCard) {
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

void::TableClass::makeFancyMove(TableClass::Table& t, TableClass::Stats& s, double r) {
	int totalValue{};
	for (auto& move : t.possibleMoves) {
		// Add default moveValue to map if it doesn't exist
		if (s.moveValues.find(move) == s.moveValues.end()) {
			s.moveValues[move] = DEFAULT_MOVE_VALUE;
		}
		totalValue += s.moveValues[move];
	}


	std::vector<double> moveProbabilities{};

	for (auto& move : t.possibleMoves) {
		moveProbabilities.emplace_back((double)s.moveValues.at(move) / (double)totalValue);
	}

	double cumProbabilities[500];
	std::partial_sum(moveProbabilities.begin(), moveProbabilities.end(), cumProbabilities);
	std::pair<int, int> moveToMake{};
	for (int i{}; i < moveProbabilities.size(); i++) {
		if (r < cumProbabilities[i]) {
			moveToMake = t.possibleMoves[i];
			break;
		}
	}

	auto moveIter = std::find(t.possibleMoves.begin(), t.possibleMoves.end(), moveToMake);
	std::iter_swap(t.spots.begin() + (*moveIter).first,  t.spots.begin() + (*moveIter).second);
	std::cout << "Moving from " << (*moveIter).first << " to " << (*moveIter).second << "\n";
	t.lastMove = (*moveIter);
}

void TableClass::makeMove(TableClass::Table& t, const TableClass::MoveType& m, TableClass::Stats& s) {
	float r{};
	//r = rand() % t.possibleMoves.size();
	r = ((double)rand() / (RAND_MAX));
	switch (m)
	{
	case TableClass::MoveType::Random:
	{
		// Random move
		std::iter_swap(t.spots.begin() + t.possibleMoves[r].first, t.spots.begin() + t.possibleMoves[r].second);
		std::cout << "Moving from " << t.possibleMoves[r].first << " to " << t.possibleMoves[r].second << "\n";
		t.lastMove = t.possibleMoves[r];
		break;
	}
	case TableClass::MoveType::Fancy:
	{
		makeFancyMove(t, s, r);
		break;
	}
	default:
		break;
	}
}

void TableClass::updateScore(TableClass::Table& t, TableClass::Stats& s, const TableClass::ScoringSystem& ss) {
	switch (ss)
	{
	case TableClass::ScoringSystem::finalStackIsTen:
	{
		// Add ten points when card is movd to final stack
		if (t.lastMove.second >= FIRST_FINAL_STACK_SPOT && t.lastMove.second < FIRST_FINAL_STACK_SPOT + FINAL_STACKS) {
			std::cout << "Score up! Current score: " << s.score << std::endl;
			s.score = s.score + 10;
			s.moveValues.at(t.lastMove) += 1;
		}
		else {
			s.moveValues.at(t.lastMove) -= 1;
		}
		break;
	}
	default:
		break;
	}
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
	if (std::all_of(t.spots.begin() + FIRST_FINAL_STACK_SPOT, t.spots.begin() + FIRST_FINAL_STACK_SPOT + FINAL_STACKS, atLeastKing)) {
		s.gameIsWon = true;
	}
}

void TableClass::regularToCompact(TableClass::Table& t, TableClass::CompactTable& ct) {
	// 19x8 rectangle of unsigned long longs

	// Reset compactSpots to zero
	std::fill(ct.compactSpots.begin(), ct.compactSpots.end(), 0);

	for (int col{}; col < TABLE_ROW; col++) {
		for (int row{}; row < NUM_ROWS; row++) {
			ct.compactSpots[col] = ct.compactSpots[col] | t.spots[col + row * TABLE_ROW];
		}
	}

	for (int row{}; row < NUM_ROWS; row++) {
		for (int col{}; col < TABLE_ROW; col++) {
			ct.compactSpots[row + TABLE_ROW] = ct.compactSpots[row + TABLE_ROW] | t.spots[col + row * TABLE_ROW];
		}
	}
}

void TableClass::regularToSuperCompact(TableClass::Table& t, TableClass::SuperCompactTable& sct) {
	// 13x12 rectangle of unsigned long longs

	// Reset compactSpots to zero
	std::fill(sct.superCompactSpots.begin(), sct.superCompactSpots.end(), 0);

	// Initialize first row/col
	sct.superCompactSpots[0] = t.spots[0];
	sct.superCompactSpots[13] = t.spots[0];

	int rowCounter{}, colCounter = 1;
	for (int i = 1; i < t.spots.size(); i++) {
		// Next row and first column
		if (!(i % 12)) {
			rowCounter++;
			colCounter = 0;
		}

		sct.superCompactSpots[rowCounter] = sct.superCompactSpots[rowCounter] | t.spots[i];
		sct.superCompactSpots[colCounter + 13] = sct.superCompactSpots[colCounter + 13] | t.spots[i];
		colCounter++;
	}
}

void TableClass::compactToRegular(TableClass::Table& t, TableClass::CompactTable& ct) {
	// TODO: implement function body
}

TableClass::CardSuit TableClass::getSuit(unsigned long long& card) {
	if (card & SPADES) {
		return CardSuit::Spades;
	}
	else if (card & HEARTS) {
		return CardSuit::Hearts;
	}
	else if (card & CLUBS) {
		return CardSuit::Clubs;
	}
	else {
		return CardSuit::Diamonds;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Print functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TableClass::printFilledSpots(TableClass::Table& t) {
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

	std::cout << "The final score is: " << s.score << std::endl;

	s.gameIsWon ? printWin() : printLoss();

	if (s.gameIsWon) {
		auto wait = 1;
	}
}

void TableClass::printWin() {
	std::cout << "\n";
	std::cout << "----------------------------\n";
	std::cout << "------------------------||--\n";
	std::cout << "-----------------------||---\n";
	std::cout << "----------------------||----\n";
	std::cout << "---------------------||-----\n";
	std::cout << "--------------------||------\n";
	std::cout << "--||---------------||-------\n";
	std::cout << "----||------------||--------\n";
	std::cout << "------|||--------||---------\n";
	std::cout << "--------|||-----||----------\n";
	std::cout << "----------||---||-----------\n";
	std::cout << "-----------||-||------------\n";
	std::cout << "------------|||-------------\n";
	std::cout << "----------------------------\n";
	std::cout << "\n";
}

void TableClass::printLoss() {
	std::cout << "\n";
	std::cout << "-----------------------------\n";
	std::cout << "--|||-------------------|||--\n";
	std::cout << "----|||---------------|||----\n";
	std::cout << "------|||-----------|||------\n";
	std::cout << "--------|||-------|||--------\n";
	std::cout << "----------|||---|||----------\n";
	std::cout << "------------||||||-----------\n";
	std::cout << "------------||||||-----------\n";
	std::cout << "----------|||---|||----------\n";
	std::cout << "--------|||-------|||--------\n";
	std::cout << "------|||-----------|||------\n";
	std::cout << "----|||---------------|||----\n";
	std::cout << "--|||-------------------|||--\n";
	std::cout << "-----------------------------\n";
	std::cout << "\n";
}