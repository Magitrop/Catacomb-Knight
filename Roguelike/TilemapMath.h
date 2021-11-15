#pragma once

double MoveTowards(double current, double target, double maxSpeed);
double MoveTowards(double current, double target, double maxSpeed, bool *goal);
bool CheckDistanceEquality(double from, double to, double maxSpeed);
void MoveTowardsVector(double *currentA, double targetA, double *currentB, double targetB, double maxSpeed);

template <class T>
class List final
{
private:
	class Node
	{
	public:
		Node *next;
		Node *prev;
		T data;

		Node(T item) : data(item)
		{
			next = prev = nullptr;
		}
	};

	Node *first;
	Node *last;
	int length;
public:
	List()
	{
		first = last = nullptr;
	}
	~List()
	{
		RemoveList();
	}

	void Add(T item)
	{
		Node *node = new Node(item);
		if (last == nullptr)
			first = last = node;
		else
		{
			node->prev = last;
			last->next = node;
			last = node;
		}
		length++;
	}
	void Remove()
	{
		if (last != nullptr)
		{
			if (last->prev != nullptr)
			{
				last = last->prev;
				delete last->next;
				last->next = nullptr;
			}
			else
			{
				delete last;
				last = first = nullptr;
			}
			length--;
		}
	}
	int Count()
	{
		return length;
	}
	void RemoveList()
	{
		while (last != nullptr)
			Remove();
	}
	bool Contains(T item)
	{
		for (Node *n = last; n != nullptr; n = n->prev)
			if (n->data == item)
				return true;
		return false;
	}
	T& operator[] (int index)
	{
		int i = 0;
		for (Node *n = last; n != nullptr; n = n->prev)
		{
			if (i == index)
				return n->data;
			i++;
		}
		throw "Index was out of range";
	}
};