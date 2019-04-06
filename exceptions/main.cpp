#include <iostream>
#include <string>
#include <setjmp.h>
#include "my_exceptions.h"

class MyClass {
	std::string s;
public:
	MyClass() : s("somewhere") {
		std::cout << "My class constructor" << std::endl;
	}

	MyClass(const std::string& _s) : s(_s) {
		std::cout << "My class constructor " << s << std::endl;
	}

	~MyClass() {
		std::cout << "My class destructor " << s << std::endl;
	}
};


int h() {
	std::cout << "h called " << std::endl;
	EXCEPTION_SAFE(MyClass, a, ("in h"));

	EXCEPTION_THROW(BAD_ALLOC);
	EXCEPTION_THROW_DEFAULT();

	EXCEPTION_RETURN();
	return 2;
}

int g() {
	std::cout << "g called " << std::endl;
	EXCEPTION_SAFE(MyClass, a, ("in g"));

	int t = h();


	EXCEPTION_RETURN();
	return 1;
}

int f() {
	std::cout << "f called " << std::endl;
	EXCEPTION_SAFE(MyClass, a, ("in f"));

	int val = 0;


	EXCEPTION_TRY(
		g();
		return val + 1;
	)
	EXCEPTION_CATCH(BAD_ALLOC, 
		std::cout << "bad alloc exception from g cautght" << std::endl;
	)
	EXCEPTION_CATCH(DEFAULT,
		std::cout << "default exception from g cautght" << std::endl;
	)
	EXCEPTION_FINALY()


	EXCEPTION_RETURN();
}


int main() {
	f();
	return 2;
}
