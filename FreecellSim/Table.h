#pragma once
#include <array>
#include <vector>
#include <map>


class TableClass {
private:
	// Private member variables
	// Total nunber of spots on table
	static constexpr int TOTAL_SPOTS = 156;
	// Number of cards in stock
	static constexpr int NUMBER_OF_CARDS = 52;
	// Number of spot on table excluded final spots and freecells
	static constexpr int REGULAR_SPOTS = 148;
	// Cards per row
	static constexpr int TABLE_ROW = 8;
	// Number of playable rows
	static constexpr int NUM_ROWS = 19;
	// Number of spots on table for final stacks
	static constexpr int FINAL_STACKS = 4;
	// Spot number of the first final stack
	static constexpr int FIRST_FINAL_STACK_SPOT = 148;
	// Number of spots on table for freecells
	static constexpr int FREECELLS = 4;
	// Spot number of the first freecell
	static constexpr int FIRST_FREECELL_SPOT = 152;
	// Unsigned long long with all black bits equal to one
	static constexpr long long BLACK_CARDS = 1501199875790160; // Odd bits
	// Unsigned long long with all red bits equal to one
	static constexpr long long RED_CARDS = 3002399751580330; // Even bits
	// Unsigned long long with all spade bits equal to one
	static constexpr long long SPADES = 300239975158033;
	// Unsigned long long with all hearts bits equal to one
	static constexpr long long HEARTS = 600479950316066;
	// Unsigned long long with all clubs bits equal to one
	static constexpr long long CLUBS = 1200959900632130;
	// Unsigned long long with all diamonds bits equal to one
	static constexpr long long DIAMONDS = 2401919801264260;
	// Default non-normalized value for a move
	static constexpr int DEFAULT_MOVE_VALUE = 100000;


public:
	// Public member variables
	struct Table {
		std::array<unsigned long long, TOTAL_SPOTS> spots;
		std::vector<std::pair<int, int>> possibleMoves;
		std::pair<int, int> lastMove;
		std::array<int, TABLE_ROW> bottomCards;
		int tableDistance;
		Table() {
			spots = {};
			possibleMoves = {};
			lastMove = {};
			bottomCards = {};
			tableDistance = {};
		}
	};
	struct CompactTable {
		// Compact table, summarizes table in 27 unsigned long longs
		// first 8 numbers are columns of the actual table, remaing 19 are the rows from top to bottom
		std::array<unsigned long long, 27> compactSpots;
		CompactTable() {
			compactSpots = {};
		}
	};
	struct SuperCompactTable {
		// Compact table, summarizes table in 25 unsigned long longs
		// Lay out all 'Table.spots' in 13x12 rectangle, every row and column is a number
		std::array<unsigned long long, 25> superCompactSpots;
		SuperCompactTable() {
			superCompactSpots = {};
		}
	};
	struct Stats {
		bool gameIsFinished;
		bool gameIsWon;
		int turns;
		int score;
		std::map<std::pair<int, int>, int> moveValues;
		Stats() {
			gameIsFinished = false;
			gameIsWon = false;
			turns = 0;
			score = 0;
			moveValues = {};
		}
	};
	enum class CardSuit {
		Spades = 0,
		Hearts,
		Clubs,
		Diamonds
	};
	enum class MoveType
	{
		// Pick a randomly selected move from all available moves
		Random,

		// Pick a move where all possible moves have pre-calculated possibilities
		Fancy
	};
	enum class ScoringSystem
	{
		finalStackIsTen
	};
private:
	// Private methods
	int getNumberOfCardsInColumn(TableClass::Table& t, int& col);
	unsigned long long getCardCompact(TableClass::CompactTable& ct, int& col, int& row);
	void getMovableCards(std::vector<int>& cards, TableClass::Table& t);
	void getPossibleMoves(TableClass::Table& t, std::vector<int>& movableCards);
	bool cardsAreCompatible(const unsigned long long& lowerCard, const unsigned long long& upperCard);
	void makeFancyMove(TableClass::Table& t, TableClass::Stats& s, double r);
	unsigned long long getCardOneSpotLower(TableClass::Table& t, int& spot);
	unsigned long long getCardOneSpotLower(TableClass::Table& t, unsigned long long& card);
	unsigned long long getCardOneSpotLower(TableClass::Table& t, int& col, int& row);
	unsigned long long getOneCardValueLower(unsigned long long& card);
	unsigned long long getOneCardValueHigher(unsigned long long& card);
	int getPenalty(unsigned long long& card, TableClass::Table& t);
	void printWin();
	void printLoss();
public:
	// Public methods
	void setupTable(TableClass::Table& t);
	void initializeStrategy(TableClass::Stats& s);
	void saveStrategy(TableClass::Stats& s);
	void getTableDistance(TableClass::Table& t, TableClass::CompactTable& ct);
	void updatePossibleMoves(TableClass::Table& t);
	void makeMove(TableClass::Table& t, const MoveType& m, TableClass::Stats& s);
	void updateScore(TableClass::Table& t, TableClass::Stats& s, const ScoringSystem& ss);
	void checkIfGameIsFinished(TableClass::Table& t, TableClass::Stats& s);
	void checkIfWon(TableClass::Table& t, TableClass::Stats& s);
	void regularToCompact(TableClass::Table& t, TableClass::CompactTable& ct);
	void regularToSuperCompact(TableClass::Table& t, TableClass::SuperCompactTable& sct);
	void compactToRegular(TableClass::Table& t, TableClass::CompactTable& ct);
	CardSuit getSuit(unsigned long long& card);
	void printFilledSpots(TableClass::Table& t);
	void printPossibleMoves(TableClass::Table& t);
	void printResult(TableClass::Stats& s);
};
