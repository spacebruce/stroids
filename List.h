#pragma once

template<typename T>
class List
{
private:
	T * start;
	T * next;
	T * end;

	const int defaultSize = 8;

public:
	List(void);
	List(int count);
	List(const List & other);
	List(List<T> && other);

	~List(void);

	inline int GetCount(void) const;
	inline int GetCapacity(void) const;

	void Add(const T & item);
	bool Remove(int index);
	void Clear(void);
	T & operator [] (int index);
	const T & operator [] (int index) const;

   	List<T> & operator=(const List<T> & other);
   	List<T> & operator=(List<T> && other);
   	
private:
	void EnsureCapacity(int min);
};

template<typename T>
List<T>::List(void) : start(nullptr), next(nullptr), end(nullptr)
{
}

template<typename T>
List<T>::List(int count)
{
	this->start = (T*)malloc(sizeof(T) * count);
	this->next = this->start;
	this->end = this->start + count;
}

template<typename T>
List<T>::List(const List & other)
{
	if(this->start != nullptr)
	{
		free(this->start);
	}
	
	this->start = (T*)malloc(sizeof(T) * other.GetCount());
	this->next = this->start + other.GetCount();
	this->end = this->start + other.GetCapacity();
}

template<typename T>
List<T>::List(List && other)
{
	this->start = other.start;
	this->next = other.next;
	this->end = other.end;
	
	other.start = nullptr;
	other.next = nullptr;
	other.end = nullptr;
}

template<typename T>
List<T>::~List(void)
{
	if (this->start != nullptr)
	{
		free(this->start);
		this->start = nullptr;
	}
}

template<typename T>
int List<T>::GetCount(void) const
{
	return (this->next - this->start);
}

template<typename T>
int List<T>::GetCapacity(void) const
{
	return (this->end - this->start);
}

template<typename T>
void List<T>::Add(const T & item)
{
	if (this->next == this->end)
	{
		this->EnsureCapacity(this->GetCapacity() * 2);
	}

	*this->next = item;
	++this->next;
}

template<typename T>
bool List<T>::Remove(int index)
{
	T * destination = this->start + index;

	if (destination > this->next)
	{
		return false;
	}

	while ( destination < this->next)
	{
		*(destination) = *(destination + 1);
		++destination;
	}

	--this->next;
	return true;
}

template<typename T>
void List<T>::Clear(void)
{
	this->next = start;
}

template<typename T>
T & List<T>::operator [] (int index)
{
	return this->start[index];
}

template<typename T>
const T & List<T>::operator [] (int index) const
{
	return this->start[index];
}

template<typename T>
List<T> & List<T>::operator=(const List<T> & other)
{
	if(this != &other)
	{
		if(this->start != nullptr)
		{
			free(this->start);
		}
		
		this->start = (T*)malloc(sizeof(T) * other.GetCount());
		this->next = this->start;
		this->end = this->start + other.GetCount();
		
		for (T * src = this->start; src != this->next;)
		{
			*(this->next++) = *(src++);
		}
	}
	return *this;
}
   	
template<typename T>
List<T> & List<T>::operator=(List<T> && other)
{
	if(this != &other)
	{
		if(this->start != nullptr)
		{
			free(this->start);
		}
		
		this->start = other.start;
		this->next = other.next;
		this->end = other.end;
		
		other.start = nullptr;
		other.next = nullptr;
		other.end = nullptr;
	}
	return *this;
}

template<typename T>
void List<T>::EnsureCapacity(int min)
{
	if (this->GetCapacity() >= min)
	{
		return;
	}

	int newCapacity = this->GetCapacity() == 0 ? defaultSize : this->GetCapacity() * 2;
	if (newCapacity < min)
	{
		newCapacity = min;
	}

	T * newStart = (T*)malloc(sizeof(T) * newCapacity);
	T * newNext = newStart;

	for (T * src = this->start; src != this->next;)
	{
		*(newNext++) = *(src++);
	}

	if (this->start != nullptr)
	{
		free(this->start);
	}

	this->start = newStart;
	this->next = newNext;
	this->end = newStart + newCapacity;
}
