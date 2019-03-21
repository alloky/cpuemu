#include "stack.h"

#include <exception>

Stack::Stack()
{
}

Stack::~Stack()
{
}

void Stack::SetStart(char * _stack_start)
{
	stack_start = _stack_start;
	stack_top = _stack_start;
}

void Stack::SetSize(int size)
{
	stack_max = stack_start + size;
}
