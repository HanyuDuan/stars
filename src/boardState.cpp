#include "boardState.h"

int BoardState::removeNumber;
int BoardState::addNumber;

BoardState::BoardState(const Json::Value& root)
	: rows(root["row"].asInt()), cols(root["column"].asInt()), winn(root["winn"].asInt()) {
	starsOn = false;
	generate();
	for (short i = 0; i < cols; ++i) {
		strcpy(board[i], root["board"][i].asCString());
		top[i] = root["top"][i].asInt();
	}
}

void BoardState::generate() {
	// TODO - is quite stupid, change this when have the chance
	// it's best to use allocate?
	board	 = new char*[cols];
	top		 = new short[cols];
	starArea = new short[cols];
	for (short i = 0; i < cols; ++i) {
		top[i]		= 0;
		starArea[i] = rows;
		board[i]	= new char[rows + 1];
		for (short j = 0; j < rows; ++j)
			board[i][j] = ' ';
	}
}

void BoardState::generate(char** b, const short* t) {
	board	= new char*[cols];
	top		= new short[cols];
	starArea = new short[cols];
	for (short i = 0; i < cols; ++i) {
		board[i]   = new char[rows + 1];
		top[i]	   = t[i];
		starArea[i] = rows;
	}
	refreshBoard(b);
}

void BoardState::free() {
	for (short i = 0; i < cols; ++i)
		delete[] board[i];
	delete[] board;
	delete[] top;
	delete[] starArea;
}

BoardState& BoardState::operator=(const BoardState& bh) {
	if (this == &bh)
		return *this;
	free();
	cols	= bh.cols;
	rows		= bh.rows;
	winn	= bh.winn;
	starsOn = bh.starsOn;
	generate(bh.board, bh.top);
	return *this;
}

Json::Value BoardState::boardToJson() {
	Json::Value root;
	for (short i = 0; i < cols; ++i) {
		board[i][rows] = '\0';
		root.append(board[i]);
	}
	return root;
}

Json::Value BoardState::topToJson() { return arraryToJson(top, cols); }

template <typename T>
Json::Value BoardState::arraryToJson(T a[], int n) {
	Json::Value root;
	for (int i = 0; i < n; ++i)
		root.append(a[i]);
	return root;
}

void BoardState::show() {
	short i = 1;
	for (; i <= cols && i <= 10; ++i)
		printf(" %d", i);
	for (; i <= cols; ++i)
		printf("%d", i);
	printf("\n");
	for (short i = rows - 1; i > -1; --i) {
		for (short j = 0; j < cols; ++j)
			printf("|%c", board[j][i]);
		printf("|\n");
	}
}

bool BoardState::boardIsFull() {
	// static int timer = 0;
	// ++timer;
	// if (timer > 38815)
	//     printf("isOver %d ", timer);
	for (short i = 1; i <= cols; ++i)
		if (!colIsFull(i))
			return false;
	return true;
}

char BoardState::isOver() {
	// static int timer = 0;
	// ++timer;
	// if (timer > 5000)
	//     printf("isOver %d ", timer);
	// if (boardIsFull())
	//     return 'F';
	// check if there is winn in line
	for (short i = 0; i < cols; ++i)
		for (short j = 0; j < top[i]; ++j)
			// assume that there is only one winn in line in the board
			if (winPieceNearBy(i, j)) {
				// printf("debug - column=%d, row=%d\n", i, j);
				return board[i][j];
			}
	return 'N';
}

void BoardState::nonFullColumn(shortv& nonFull) {
	nonFull.clear();  // see if delete this will help?
	for (short i = 0; i < cols; ++i)
		if (top[i] != starArea[i])
			nonFull.push_back(i + 1);
	// static int timer = 0;
	// printf("nonfull %d ", ++timer);
}

char BoardState::rPlayer(const char plr) {
	if (plr == 'X')
		return '0';
	if (plr == '0')
		return 'X';
	throw runtime_error("No such player exist!\n");
}

short BoardState::randomMove() {
	shortv list;
	nonFullColumn(list);
	if (list.empty())
		throw runtime_error("trying randomMove() in an empty list\n");
	return randomMove(list);
}

short BoardState::randomMove(shortv& list) {
	if (list.empty())
		throw runtime_error("trying randomMove(shortv& list) in an empty list\n");
	srand((unsigned)time(NULL));
	return list[rand() % list.size()];
}

short BoardState::randomSuggestion(const char plr, shortv& list, const string& mode) {
	shortv opp2, opp1, plr2, plr1;
	char   opp = rPlayer(plr);
	srand((unsigned)time(NULL));
	for (vIter col = list.begin(); col != list.end(); ++col) {
		if (colIsFull(*col) || colIsEmpty(*col))
			;
		else if (board[*col - 1][top[*col - 1] - 1] == plr) {
			if (top[*col - 1] - 1 > 0 && board[*col - 1][top[*col - 1] - 2] == plr)
				plr2.push_back(*col);
			else
				plr1.push_back(*col);
		} else {
			if (top[*col - 1] - 1 > 0 && board[*col - 1][top[*col - 1] - 2] == opp)
				opp2.push_back(*col);
			else
				opp1.push_back(*col);
		}
	}
	short ran = rand() % 100;
	if (mode == "progressive") {
		if (!plr2.empty() && ran < 72)
			return randomMove(plr2);
		ran = rand() % 100;
		if (!opp2.empty() && ran < 95)
			return randomMove(opp2);
		ran = rand() % 100;
		if (!plr1.empty() && ran < 95)
			return randomMove(plr1);
		ran = rand() % 100;
		if (!opp1.empty() && ran < 57)
			return randomMove(opp1);
	} else if (mode == "defensive") {
		if (!opp2.empty() && ran < 95)
			return randomMove(opp2);
		ran = rand() % 100;
		if (!plr2.empty() && ran < 72)
			return randomMove(plr2);
		ran = rand() % 100;
		if (!opp1.empty() && ran < 77)
			return randomMove(opp1);
		ran = rand() % 100;
		if (!plr1.empty() && ran < 65)
			return randomMove(plr1);
	} else
		throw runtime_error("no such mode.\n");
	if (list.empty())
		throw runtime_error("call randomSuggestion with empty list");
	return randomMove(list);
}

short BoardState::randomSuggestion(const char plr, shortv& list, shortv oppList, const string& mode) {
	// preference No.2: take the opponent's safe list
	shortv intersectionList;
	MyShortList::shortIntersection(intersectionList, list, oppList);
	if (intersectionList.empty()) {
		if (list.empty())
			throw runtime_error("call randomSuggestion(4 args) with empty list");
		return randomSuggestion(plr, list, mode);
	}
	return randomSuggestion(plr, intersectionList, mode);
	// preference No.1: take what can bring me winn-1 in a row, and what can
	// interrupt opponent's three in a row
}

bool BoardState::winPieceNearBy(const short col, const short ro) {
	// grow up, right, upright, downright
	short i		  = 1;
	char  present = board[col][ro];
	bool  canUp = ro <= top[col] - winn, canRight = col <= cols - winn, canDown = ro >= winn - 1;
	if (canRight) {
		// right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro] != present)
				break;
		if (i == winn)
			return true;
		// up & right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro + i] != present)
				break;
		if (i == winn)
			return true;
		if (canDown) {
			// down & right
			for (i = 1; i < winn; ++i)
				if (board[col + i][ro - i] != present)
					break;
			if (i == winn)
				return true;
		}
	}
	if (canUp) {
		// up
		for (i = 1; i < winn; ++i)
			if (board[col][ro + i] != present)
				return false;
		return true;
	}
	return false;
}

void BoardState::refreshBoard(char** hb) {
	for (short i = 0; i < cols; ++i)
		for (short j = 0; j < rows; ++j)
			board[i][j] = hb[i][j];
}

void BoardState::refreshTop() {
	for (short i = 0; i < cols; ++i) {
		short j = 0;
		for (; j < rows && board[i][j] != ' '; ++j)
			;
		top[i] = j;
	}
}

void BoardState::customBoard(const short cl, const short ro, const short wi) {
	free();
	cols = cl;
	rows	   = ro;
	winn   = wi;
	generate();
}

void BoardState::areaTopTransform() {
	for (short i = 0; i < cols; ++i)
		starArea[i] = 0;
	for (short i = 0; i < cols; ++i)
		setATopWithTop(i, top[i]);

	// check for bulks of piece
	for (short i = 0; i < cols - 1;++i){
		if (top[i] && top[i+1]) {
			setATopWithNumber(i + 3, 1);
			setATopWithNumber(i - 2, 1);
		}
	}

	// then check for overflow
	for (short i = 0; i < cols; ++i)
		if (starArea[i] > rows)
			starArea[i] = rows;
	
	// check if stars are even
	int stars = starNumber();
	if(stars % 2 == 0)
		return;
	// randomly chose a non-full column and add it
	shortv starNotZero, starNotFull, inter;
	for (short i = 0; i < cols; ++i) {
		if (starArea[i])
			starNotFull.push_back(i + 1); // not full of stars(maybe doesn't have stars)
		if (rows != starArea[i])
			starNotZero.push_back(i + 1); // have stars but may be full of stars
	}
	MyShortList::shortIntersection(inter, starNotFull, starNotZero);
	if (inter.empty())
		++starArea[randomMove(starNotZero) - 1];
	else
		++starArea[randomMove(inter) - 1];
}

void BoardState::areaTopRestore() {
	for (short i = 0; i < cols; ++i)
		starArea[i] = rows;
}

void BoardState::starShow() {
	areaTopTransform();
	short i = 1;
	for (; i <= cols && i <= 10; ++i)
		printf(" %d", i);
	for (; i <= cols; ++i)
		printf("%d", i);
	printf("\n");
	for (short i = rows - 1; i > -1; --i) {
		for (short j = 0; j < cols; ++j) {
			if (i < starArea[j])
				printf("|%c", board[j][i]);
			else
				printf("|+");
		}
		printf("|\n");
	}
	areaTopRestore();
}

void BoardState::setATopWithTop(short i, short t) {
	// important numbers here!
	if (t == 0)
		return;
	setATopWithNumber(i - 2, t / 2);
	setATopWithNumber(i - 1, t + 1);
	setATopWithNumber(i, t + 2);
	setATopWithNumber(i + 1, t + 1);
	setATopWithNumber(i + 2, t / 2);
}

shortv BoardState::aTopFullColumn() {
	areaTopTransform();
	shortv list;
	for (short i = 0; i < cols;++i)
		if (starArea[i]==0)
			list.push_back(i + 1);
	return list;
}

int BoardState::starNumber() {
	int sum = 0;
	for (int i = 0; i < cols;++i)
		sum += rows - starArea[i];
	return sum;
}

void BoardState::retInit(vector<oneMove>& his) {
	// return the state to the 'initial state' accroding to the history move
	for (vector<oneMove>::reverse_iterator riter = his.rbegin(); riter != his.rend();++riter) {
		if (riter->mode=="debug"||riter->mode=="add"||riter->mode=="normal")
			remove(riter->move);
		else if (riter->mode=="reverse")
			add(riter->player, riter->move);
		else
			throw runtime_error("unexpected, unhandled mode in retInit in boardState.h\n");
	}
}
