#include <stdio.h>
#include "hetvec.hpp"

 /** Your class implementation goes here **/ 
struct Dog{};
struct Car{};
struct Foo{};
struct Bar{};

/** Specialize behaviour in "MyCustomBehaviour".
	You need to implement the parametrized f if you
	don't want to specialize behaviour for all combinations of types.
	Remark: This is pretty much all you need to do **/
struct MyCustomBehaviour {
	void f(const Dog& a, const Dog& b) const { printf("Two dogs are barking!\n"); }
	void f(const Dog& a, const Car& b) const { printf("A car hit the dog, oh no!\n"); }
	void f(const Bar& a, const Foo& b) const { printf("Foobar!\n"); }
	template<typename T, typename U>
	void f(const T& t, const U& u) const { }
};

int main(int argc, char** argv) {
	//You can insert elements into hv at construction or later with push_back
	hetvec<Dog, Car, Foo, Bar> hv {Dog(), Dog(), Car(), Bar()};
	hv.push_back(Foo());

	/** There are two dogs, one Car, one Bar and one Foo. So the following is printed:
			"Two dogs are barking!", "A car hit the dog, oh no!" x2, "Foobar!" **/
	hv.perform(MyCustomBehaviour()); //Notice that the specialized behaviour is a runtime argument

	hv.clear();
	if(hv.empty()) printf("hv is empty!\n");
	hv.perform(MyCustomBehaviour()); //hv is empty so nothing will happen
	return 0;
}
