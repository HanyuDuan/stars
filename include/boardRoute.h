#ifndef _BOARDROUTE_H_
#define _BOARDROUTE_H_ 1
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include "shortList.h"

using std::cout;
using std::endl;
using std::runtime_error;
using std::string;
using std::vector;

const int goodNode = -1;
const int badNode  = 0;
const int freeNode = -2;

class RouteNode {
public:
	short              data;
	bool               print;
	RouteNode*         prev;
	vector<RouteNode*> next;

	RouteNode()
		: data(0), print(true), prev(nullptr), next(vector<RouteNode*>()) {}
	RouteNode(const RouteNode& rn) { clone(rn); }

	void clone(const RouteNode& rn);

	bool listNextIs(vector<RouteNode*>& list, int data);
	bool masked(vector<RouteNode*>& list); // if all of list isn't printable
	bool hasNext();
	void maskFlag(int flag); // mask those marked if match the flag
	void resetMask();
};

typedef vector<RouteNode*>::iterator vRi;

class RouteTree {
public:
	RouteNode* head;
	RouteNode* crnt;
	static int branches;
	RouteTree() { generate(); }
	RouteTree(const RouteTree& rt);
	~RouteTree() { free(head); }

#ifdef STARS_DEBUG_INFO
	// check
	bool match() { return head == crnt; }
#endif

	// handle
	void clear();
	void generate();
	void free(RouteNode* node);
	void free(vector<RouteNode*> list);

	// move
	void       forward();
	void       forward(short data);
	void       nextNode(); // next node in the same level
	void       backward();
	RouteNode* fastBackward(RouteNode* node, int num);

	// change
	void add(short data);
	void add(ShortList& list);

	// related to show
	void showRoute();         // for boardAnalyse
	void showRoute(int flag); // for boardAnalyse
	bool showRoute(RouteNode* node, int level);
	void show(RouteNode* node, int level);
	void showAll() { show(head, 0); }
	void showCurrent() { show(crnt, 0); }

	// counter
	int  getBranches(int flag = 1);
	void branchCounter(RouteNode* node);
};

#endif // _BOARDROUTE_H_
