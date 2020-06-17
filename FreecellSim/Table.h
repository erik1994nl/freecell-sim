#pragma once
#include <array>
#include <vector>
#include <map>


class TableClass {
private:
	// Private member variables
	static constexpr int TOTAL_SPOTS = 156;
	static constexpr int NUMBER_OF_CARDS = 52;
	static constexpr int REGULAR_SPOTS = 148;
	static constexpr int TABLE_ROW = 8;
	static constexpr int FINAL_STACKS = 4;
	static constexpr int FIRST_FINAL_STACK_SPOT = 148;
	static constexpr int FREECELLS = 4;
	static constexpr int FIRST_FREECELL_SPOT = 152;
	static constexpr long long BLACK_CARDS = 1501199875790160; // Odd bits
	static constexpr long long RED_CARDS = 3002399751580330; // Even bits

public:
	// Public member variables
	struct Table {
		std::array<unsigned long long, TOTAL_SPOTS> spots;
		std::vector<std::pair<int, int>> possibleMoves;
		std::pair<int, int> lastMove;
		std::array<int, TABLE_ROW> bottomCards;
		Table() {
			spots = {};
			possibleMoves = {};
			lastMove = {};
			bottomCards = {};
		}
	};
	struct CompactTable {
		// Compact table, summarizes table in 25 unsigned long longs
		// Lay out all 'Table.spots' in 13x12 rectangle, every row and column is a number
		std::array<unsigned long long, 25> compactSpots;
		CompactTable() {
			compactSpots = {};
		}
	};
	struct Stats {
		bool gameIsFinished;
		bool gameIsWon;
		int turns;
		int score;
		Stats() {
			gameIsFinished = false;
			gameIsWon = false;
			turns = 0;
			score = 0;
		}
	};
	enum MoveType
	{
		Random,
		Fancy
	};
	enum ScoringSystem
	{
		finalStackIsTen
	};
private:
	// Private methods
	void getMovableCards(std::vector<int>& cards, TableClass::Table& t);
	void getPossibleMoves(TableClass::Table& t, std::vector<int>& movableCards);
	bool cardsAreCompatible(const unsigned long long& lowerCard, const unsigned long long& upperCard);
public:
	// Public methods
	void setupTable(TableClass::Table& t);
	void initializeStrategy(TableClass::Stats& s);
	void saveStrategy(TableClass::Stats& s);
	void updatePossibleMoves(TableClass::Table& t);
	void makeMove(TableClass::Table& t, const MoveType& m, TableClass::Stats& s);
	void updateScore(TableClass::Table& t, TableClass::Stats& s, const ScoringSystem& ss);
	void checkIfGameIsFinished(TableClass::Table& t, TableClass::Stats& s);
	void checkIfWon(TableClass::Table& t, TableClass::Stats& s);
	void regularToCompact(TableClass::Table& t, TableClass::CompactTable& ct);
	void compactToRegular(TableClass::Table& t, TableClass::CompactTable& ct);
	void printFilledSpots(TableClass::Table& t);
	void printPossibleMoves(TableClass::Table& t);
	void printResult(TableClass::Stats& s);
};
