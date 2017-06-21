# hetvec
hetvec is a header-only library which implements a heterogenous vector (aka dynamic array) which is capable of, given a set of functions whose arguments are of different type, iterating over the unordered cartesian product of the heterogenous vector and applying the corresponding function to the current pair.

This problem shows up, for example, during collision detection (and handling) where you have specialized functions for detecting collisions between various combinations of types, for instance between a sphere and a cylinder: `bool is_colliding(Sphere s, Cylinder c)`. For a large set of different geometries you'd have a large set of various functions on the form `bool is_colliding(A a, B b)`. The problem arises when you want to iterate over all geometries (of various types) and and for each pair of geometry "select" the proper collision-detection function. Since C++ does not natively support heterogenous containers or multiple dispatch, solving said problem can become tricky. It is precisely this problem hetvec solves.

## Usage
In `example.cpp` there is a short, self contained, correct (compilable, with `-std=c++11`), example:

```C++
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
```
That's it. Nothing more, nothing less. Simply copy `hetvec.hpp` into your include-directory and your good to go!

Happy coding :+1:!
