#pragma once

#include "list.h"

struct Agent {
	enum Type {
		PREDATOR,
		NONE
	};

	Type type = NONE;
	Vector2D vel;
	Vector2D tmpvel;
	Vector2D pos;

	Agent() {};
	Agent(Vector2D p) : pos(p) {};
	Agent(Vector2D p, Vector2D v) : pos(p), vel(v) {};
};

class Database {
	llist<Agent> data;
	llist<Agent*> xsort;

public:
	nodeiter<Agent> begin();
	nodeiter<Agent*> xbegin();

	void add(Agent& a);
	void clear();
	void sort();
};

inline nodeiter<Agent> Database::begin() {
	return data.begin();
}

inline nodeiter<Agent*> Database::xbegin() {
	return xsort.begin();
}
inline void Database::add(Agent& a) {
	data.add(a);
	xsort.add(&data.back());
}

inline void Database::clear() {
	data.clear();
	xsort.clear();
}

inline void Database::sort() {
	// Insertion sort (xsort)
	for (nodeiter<Agent*> i = xsort.begin().next(); !i.done(); ++i) {
		for (nodeiter<Agent*> k = i; !k.prev().prev().rdone(); --k) {
			if (!((*k)->pos[0][0] < (*k.prev())->pos[0][0])) break;
			Agent* tmp = *k;
			*k = *k.prev();
			*k.prev() = tmp;
		}
	}
}