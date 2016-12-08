#include "list.h"
#include <random>

void main() {
	llist<int> data;
	for (unsigned int i = 0; i < 16; ++i) {
		data.add(rand());
	}

	printf("Before:\n");
	for (nodeiter<int> i = data.begin().next(); !i.done(); ++i) {
		printf("%i\n", *i);
	}

	// Insertion sort
	for (nodeiter<int> i = data.begin().next(); !i.done(); ++i) {
		for (nodeiter<int> k = i; !k.prev().prev().done(); --k) {
			if (!(*k < *k.prev())) break;
			int tmp = *k;
			*k = *k.prev();
			*k.prev() = tmp;
		}
	}

	printf("\nAfter:\n");
	for (nodeiter<int> i = data.begin().next(); !i.done(); ++i) {
		printf("%i\n", *i);
	}

	system("pause");
}