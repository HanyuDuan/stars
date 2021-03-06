#ifndef _BOARDHANDLE_H_
#define _BOARDHANDLE_H_ 1

#include <stdio.h>
#include <time.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "json/json.h"

using std::logic_error;
using std::runtime_error;
using std::string;
using std::vector;
typedef vector<int> shortv;

class BoardState;
class BoardAnalyse;
class BoardInterface;
class BoardTest;

class oneMove {
	friend class BoardState;
	friend class BoardAnalyse;
	friend class BoardInterface;
	friend class BoardTest;
	friend void autoTest(int n, const vector<string>& args);

private:
	/*
	 * respond can give oneMove player, word, list, time(by computer),
	 * mode function can give oneMove mode, move, time(by a person),
	 * byComputer, suggestion, player(reverse & add), then push back to
	 * boardRecord debug has them all normal doesn't have suggestion add
	 * mode just have mode, move and player reverse mode just have mode and
	 * move
	 * */
	string  mode;
	string  word;
	shortv  list;
	int64_t time = 0; // time taken for the computer to respond
	// the move that is been taken after the computer analyse
	int  move       = 0;
	int  suggestion = 0; // suggested move by the computer
	char player     = '0';
	bool byComputer = true; // is this move taken by the computer
	bool hintOn     = true;

public:
	oneMove() {}
	oneMove(const Json::Value& root);
						 operator Json::Value();
	friend std::ostream& operator<<(std::ostream& os, oneMove& move);
};

inline char rPlayer(const char plr) {
	return plr ^ 0x68;
}

class BoardState {
	// handles the memory allocate and ways to change the board
	friend class BoardAnalyse;
	friend class BoardInterface;

private:
	vector<string> board;
	vector<int>    top;
	vector<int>    starArea;
	int            rows;
	int            cols;
	int            winn;
	static int     removeNumber;
	static int     addNumber;

public:
	BoardState() : BoardState(8, 8, 4) {}
	BoardState(const Json::Value& root);
	BoardState(const int r, const int c, const int w)
		: board(vector<string>(c, string(r, ' '))),
		  top(vector<int>(c, 0)),
		  starArea(vector<int>(c, r)),
		  rows(r),
		  cols(c),
		  winn(w) {}

	// operator
	operator Json::Value() const {
		Json::Value root;
		root["board"]  = std::move(boardToJson());
		root["top"]    = std::move(topToJson());
		root["row"]    = rows;
		root["column"] = cols;
		root["winn"]   = winn;
		return root;
	}
	Json::Value boardToJson() const;
	Json::Value topToJson() const;

	// show
	void show() const;
	void printHead() const;

	// check
	bool colCanAdd(const int col) const;
	bool colCanRemove(const int col) const;

	// getter
	int                   getWinn() const { return winn; }
	char                  getTopPiece(const int col) const;
	const vector<string>& getBoard() const { return board; }
	const shortv&         getTop() const { return top; }

	// is function
	bool colIsFull(const int col) const { return top.at(col - 1) == rows; }
	bool colIsEmpty(const int col) const { return top.at(col - 1) == 0; }
	bool boardIsFull() const;
	bool winPieceNearBy(const int col, const int ro) const;
	bool winPieceButOne(const int col, const int ro, const int win) const;
	char isOver() const;

	// tools
	void   nonFullColumn(shortv& nonFull) const;
	shortv nonFullColumn() const;
	void   sweepFullColumn(shortv& nonFull, const int col) const;
	int    pieceCount() const;

	// change board
	void add(const char plr, const int col) {
#ifdef STARS_DEBUG_INFO
		if (col < 1 || col > cols)
			throw logic_error("trying to add in a wrong place");
		if (top[col - 1] == rows)
			throw logic_error("trying to add to a full column");
		if (plr != 'X' && plr != '0')
			throw logic_error("wrong player!");
		++addNumber;
#endif
		board[col - 1][top[col - 1]++] = plr;
	}
	void remove() {}
	template <typename T, typename... Args>
	void remove(const T col, Args... args) {
#ifdef STARS_DEBUG_INFO
		if (col < 1 || col > cols)
			throw logic_error("trying to remove in a wrong place");
		if (top[col - 1] == 0)
			throw logic_error("trying to remove from an empty column");
		++removeNumber;
#endif
		board[col - 1][(top[col - 1]--) - 1] = ' ';
		remove(args...);
	}

	// debug
	bool match() const { return removeNumber == addNumber; }
	bool valid() const;

	// custom
	void customBoard(const int cl, const int ro, const int wi);

	// performance
	void   areaTopTransform();
	void   areaTopRestore();
	void   starShow();
	void   setATopWithTop(const int i, const int t);
	void   setATopWithNumber(const int i, const int n);
	shortv aTopFullColumn();
	int    starNumber() const;
	int    threeRowCount(const char plr, shortv& safeList);
	shortv makeThreeCols(const char plr, shortv& safeList);
	bool   specialPiece(const int col, const int ro) const;

	// input a new board
	void refreshTop() noexcept;

	// history move
	void retInit(const vector<oneMove>& history);
};

#endif // _BOARDHANDLE_H_
