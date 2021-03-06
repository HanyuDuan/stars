#ifndef _BOARDTEST_H_
#define _BOARDTEST_H_ 1
#include "boardInterface.h"

// reload getInput to test
class BoardTest : public BoardInterface {
	friend void autoTest(int n, const vector<string>& args);

private:
	char toWinn;
	bool showCalculate;
	bool showTime;
	bool lessPrint;
	bool noPrint;

	bool askToReverseBool;
	bool askToSaveBoardBool;

	static char lastMove;

public:
	BoardTest()
		: BoardInterface(),
		  toWinn('N'),
		  showCalculate(
			  record.getDefaultSettings("inDebugMode", "showCalculate")),
		  showTime(record.getDefaultSettings("inDebugMode", "showTime")),
		  askToReverseBool(
			  record.getDefaultSettings("gameIsOver", "askToReverse")),
		  askToSaveBoardBool(
			  record.getDefaultSettings("gameIsOver", "askToSaveBoard")),
		  lessPrint(false),
		  noPrint(false) {}
	BoardTest(const vector<string>& args);

	// virtual
	virtual bool   askToReverse(bool yes) { return false; }
	virtual void   askToSaveBoard(bool yes = false);
	virtual bool   isOver(const oneMove& move);
	virtual string debugMode(const string& mode);
	virtual bool   controlMode(const string& firstMode);
	virtual short  respond();

	// tools
	string getInput(const string& mode, const short& suggestion) const;
};

void autoTest(int n, const vector<string>& args);

#endif // _BOARDTEST_H_
