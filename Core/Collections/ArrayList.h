#pragma once

template <typename T>
class ArrayList
{
private:
	T * start;
	T * next;
	T * end;

public:
	ArrayList(T array[], const int length);
	ArrayList(ArrayList<T> && other);

	~ArrayList(void);

	inline int GetCount(void) const;
	inline int GetCapacity(void) const;

	bool Add(const T & item);
	bool Remove(const T &item);
	bool RemoveAt(int index);
	int IndexOf(const T &item);
	void Clear(void);

	T & operator [] (int index);
	const T & operator [] (int index) const;

	ArrayList<T> & operator = (ArrayList<T> && other);
};

template <typename T>
ArrayList<T>::ArrayList(T array[], const int length) :
	start(array), next(array), end(array + length) {}


template <typename T>
ArrayList<T>::ArrayList(ArrayList && other) :
	start(other.start), next(other.next), end(other.end)
{
	other.start = nullptr;
	other.next = nullptr;
	other.end = nullptr;
}

template <typename T>
ArrayList<T>::~ArrayList(void)
{
	if (this->start != nullptr && this->next != nullptr)
	{
		for (T * item = this->next; item >= this->start; --item)
		{
			item->~T();
		}
		this->start = nullptr;
		this->next = nullptr;
		this->end = nullptr;
	}
}

template <typename T>
int ArrayList<T>::GetCount(void) const
{
	return (this->next - this->start);
}

template <typename T>
int ArrayList<T>::GetCapacity(void) const
{
	return (this->end - this->start);
}

template <typename T>
bool ArrayList<T>::Add(const T & item)
{
	if (this->next == this->end)
	{
		return false;
	}

	*(this->next) = item;
	++this->next;
	return true;
}

template <typename T>
bool ArrayList<T>::Remove(const T &item)
{	
	for (T * position = this->start; position < this->next; ++position)
	{
		if (*position == item)
		{
			while (position < this->next)
			{
				*(position) = *(position + 1);
				++position;
			}

			this->next->~T();
			--this->next;
			return true;
		}
	}
	return false;	
}

template <typename T>
bool ArrayList<T>::RemoveAt(int index)
{
	// Assert(index >= 0)
	// Assert(destination < this->next)

	T * destination = this->start + index;

	if (destination >= this->next)
	{
		return false;
	}

	while (destination < this->next)
	{
		*(destination) = *(destination + 1);
		++destination;
	}

	this->next->~T();
	--this->next;
	return true;
}

template <typename T>
int ArrayList<T>::IndexOf(const T &item)
{
	for (T * position = this->start; position < this->next; ++position)
	{
		if (*position == item)
		{
			return position - this->start;
		}
	}
	return -1;
}

template <typename T>
void ArrayList<T>::Clear(void)
{
	for (T * item = this->start; item < this->next; ++item)
	{
		item->~T();
	}
	this->next = this->start;
}

template <typename T>
T & ArrayList<T>::operator [] (int index)
{
	return this->start[index];
}

template <typename T>
const T & ArrayList<T>::operator [] (int index) const
{
	return this->start[index];
}

template <typename T>
ArrayList<T> & ArrayList<T>::operator = (ArrayList<T> && other)
{
	if (this != &other)
	{
		this->start = other.start;
		this->next = other.next;
		this->end = other.end;

		other.start = nullptr;
		other.next = nullptr;
		other.end = nullptr;
	}
	return *this;
}
