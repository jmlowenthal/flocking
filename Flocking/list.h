#pragma once

template<typename T> class llist;

template<typename T>
class node {
	node* next = nullptr;
	node* prev = nullptr;
	T data;

public:
	void remove();
	node<T>(T d) : data(d) {};

	template<typename T> friend class llist;
	template<typename T> friend class nodeiter;
};

template<typename T>
class nodeiter {
	llist<T>& parent;

	node<T>* _prev = nullptr;
	node<T>* _next = nullptr;
	node<T>* current = nullptr;

public:
	nodeiter<T>& operator++();
	nodeiter<T>& operator--();
	T& operator*();
	T* operator->();
	bool done();
	bool rdone();

	nodeiter<T> next();
	nodeiter<T> prev();

	void remove();

	nodeiter<T>(node<T>* c, llist<T>& p);

	template<typename T> friend class llist;
};

#pragma region Class function definitions

template<typename T>
inline nodeiter<T>::nodeiter(node<T>* c, llist<T>& p) : current(c), parent(p)
{
	if (c != nullptr) {
		_prev = c->prev; _next = c->next;
	}
}

template<typename T>
inline nodeiter<T>& nodeiter<T>::operator++()
{
	current = _next;
	if (current != nullptr) {
		_next = current->next;
		_prev = current->prev;
	}
	else {
		// maintain backward link (_prev)
		_next = nullptr;
	}
	return *this;
}

template<typename T>
inline nodeiter<T>& nodeiter<T>::operator--()
{
	current = _prev;
	if (current != nullptr) {
		_next = current->next;
		_prev = current->prev;
	}
	else {
		// maintain forward link (_next)
		_prev = nullptr;
	}
	return *this;
}

template<typename T>
inline T & nodeiter<T>::operator*()
{
	return current->data;
}

template<typename T>
inline T * nodeiter<T>::operator->()
{
	return &(current->data);
}

template<typename T>
inline bool nodeiter<T>::done()
{
	return current == nullptr && _next == nullptr;
}

template<typename T>
inline bool nodeiter<T>::rdone()
{
	return current == nullptr && _prev == nullptr;
}

template<typename T>
inline nodeiter<T> nodeiter<T>::next()
{
	return nodeiter<T>(_next, parent);
}

template<typename T>
inline nodeiter<T> nodeiter<T>::prev()
{
	return nodeiter<T>(_prev, parent);
}


template<typename T>
inline void nodeiter<T>::remove() {
	if (current != nullptr) {
		if (_next != nullptr)
			_next->prev = _prev;
		else // This is the last item
			parent._end = current->prev;
		if (_prev != nullptr)
			_prev->next = _next;
		else // This is the first item
			parent._start = _next;

		node<T>* tmp = current;
		current = nullptr;
		delete tmp;
	}
}

#pragma endregion

template<typename T>
class llist {
	node<T>* _start = nullptr;
	node<T>* _end = nullptr;

public:
	nodeiter<T> begin();
	nodeiter<T> end();
	T& front();
	T& back();

	void add(T data);
	void clear();

	template<typename T> friend class nodeiter;
};

#pragma region Class function definitions

template<typename T>
inline nodeiter<T> llist<T>::begin()
{
	return nodeiter<T>(_start, *this);
}

template<typename T>
inline nodeiter<T> llist<T>::end()
{
	return nodeiter<T>(_end, *this);
}

template<typename T>
inline T & llist<T>::front()
{
	return _start->data;
}

template<typename T>
inline T & llist<T>::back()
{
	return _end->data;
}

template<typename T>
inline void llist<T>::add(T data)
{
	if (_start == nullptr) {
		_start = new node<T>(data);
		_end = _start;
	}
	else {
		node<T>* tmp = _end;
		_end = new node<T>(data);
		tmp->next = _end;
		_end->prev = tmp;
	}
}

template<typename T>
inline void llist<T>::clear()
{
	for (nodeiter<T> i = begin(); !i.done(); ++i) {
		i.remove();
	}
}

#pragma endregion