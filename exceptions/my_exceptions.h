#pragma once

#include <csetjmp>
#include <functional>
#include <list>

enum MyExceptionType {
	BAD_FILE = 1,
	OUT_OF_MEM,
	BAD_ALLOC,
	DEFAULT,
	NO_EXCEPTION
};


class MyExceptionDestructorNode {
public:
	enum Type {
		NORMAL,
		FRAME_END
	};
	Type type;
	std::function<void()> destructor;
};

std::list<MyExceptionDestructorNode*> MY_GLOBAL_EXCEPTION_DESTR_LIST;
std::jmp_buf MY_GLOBAL_EXCEPTION;
MyExceptionType MY_GLOBAL_EXCEPTION_TYPE;



#define EXCEPTION_SAFE(__type, __variable, __args) \
	volatile __type __variable __args; \
	if (true) { \
		MyExceptionDestructorNode* new_node = new MyExceptionDestructorNode(); \
		new_node->destructor = [&__variable]() { __variable.~__type(); }; \
		new_node->type = MyExceptionDestructorNode::Type::NORMAL;  \
		MY_GLOBAL_EXCEPTION_DESTR_LIST.insert(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin(), new_node); \
	}

void EXCEPTION_FREE_FRAME() {
	while (!MY_GLOBAL_EXCEPTION_DESTR_LIST.empty() 
	   && (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin())->type != MyExceptionDestructorNode::Type::FRAME_END)
	{ 
		MyExceptionDestructorNode* cur = (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin()); 
		MY_GLOBAL_EXCEPTION_DESTR_LIST.erase(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin()); 
		cur->destructor();
		delete cur;
	}
	if (!MY_GLOBAL_EXCEPTION_DESTR_LIST.empty()) {
		MyExceptionDestructorNode* cur = (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		MY_GLOBAL_EXCEPTION_DESTR_LIST.erase(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		delete cur;
	}
}

void EXCEPTION_FREE_FRAME_NO_DESTRUCT() {
	while (!MY_GLOBAL_EXCEPTION_DESTR_LIST.empty()
		&& (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin())->type != MyExceptionDestructorNode::Type::FRAME_END)
	{
		MyExceptionDestructorNode* cur = (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		MY_GLOBAL_EXCEPTION_DESTR_LIST.erase(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		delete cur;
	}
	if (!MY_GLOBAL_EXCEPTION_DESTR_LIST.empty()) {
		MyExceptionDestructorNode* cur = (*MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		MY_GLOBAL_EXCEPTION_DESTR_LIST.erase(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin());
		delete cur;
	}
}

void INSERT_FRAME_END() {
	MyExceptionDestructorNode* __dest_list_cur_frame_end = new MyExceptionDestructorNode(); 
	__dest_list_cur_frame_end->type = MyExceptionDestructorNode::Type::FRAME_END; 
	MY_GLOBAL_EXCEPTION_DESTR_LIST.insert(MY_GLOBAL_EXCEPTION_DESTR_LIST.begin(), __dest_list_cur_frame_end);
}

#define EXCEPTION_TRY(code) \
	if (true) { \
		INSERT_FRAME_END(); \
	} \
	volatile int __my_exception = std::setjmp(MY_GLOBAL_EXCEPTION); \
	if(__my_exception != 0){ \
		goto __my_catch; \
	} \
	code \
	goto __myexception_finaly; \
	__my_catch:

#define EXCEPTION_CATCH(exception_type, code) \
	if(exception_type == MY_GLOBAL_EXCEPTION_TYPE) \
	{\
	code \
	goto __myexception_finaly; \
	}

#define EXCEPTION_FINALY(code) __myexception_finaly: \
	code

#define EXCEPTION_RETURN() \
	EXCEPTION_FREE_FRAME_NO_DESTRUCT();

#define EXCEPTION_THROW_DEFAULT() \
	EXCEPTION_FREE_FRAME(); \
	MY_GLOBAL_EXCEPTION_TYPE = MyExceptionType::DEFAULT; \
	std::longjmp(MY_GLOBAL_EXCEPTION, 1)

#define EXCEPTION_THROW(exception_type) \
	EXCEPTION_FREE_FRAME(); \
	MY_GLOBAL_EXCEPTION_TYPE = MyExceptionType:: ## exception_type; \
	std::longjmp(MY_GLOBAL_EXCEPTION, 1)