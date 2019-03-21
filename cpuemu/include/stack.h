#pragma once

class Stack {
public:
	Stack();
	~Stack();

	void SetStart(char* _stack_start);
	void SetSize(int size);

	template <class T = long long>
	void push(T value);

	template <class T = long long>
	T pop();

private:
	char* stack_start;
	char* stack_max;
	char* stack_top;
};

template<class T>
void Stack::push(T value)
{
	if (stack_top + sizeof(T) >= stack_max) {
		throw std::exception("stack oveflow");
	}
	auto ptr = reinterpret_cast<T*>(stack_top);
	*ptr = value;
	stack_top = reinterpret_cast<char*>(ptr + 1);
};

template<class T>
T Stack::pop()
{
	if (stack_top == stack_start) {
		throw std::exception("pop on empty stack");
	}
	stack_top -= sizeof(T);
	return *reinterpret_cast<T*>(stack_top);
};

