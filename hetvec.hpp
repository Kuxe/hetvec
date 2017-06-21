#ifndef HETVEC_HPP
#define HETVEC_HPP

#include <vector>
#include <algorithm>
#include <utility>

/** Hetvec is a heterogenous vector (aka dynamic array)
	which is tailored for solving the common problem where
	one needs to iterate over a heterogenous collection and
	for each pair in the collection call the appropiate function
	(overloaded on the types of the elemenets of the pair).
	
	For example, in games you typically want to check collision
	between different types of objects. Ideally there would be
	some heterogenous container where you could store both
	triangles, spheres and various 3D-geometry - traverse
	the unordered cartesian product of this container
	and call the appropiate function for each pair, such as
	check_collision_triangle_sphere(Sphere s, Triangle t) if
	the pair consists of a Sphere and a Triangle.
	Implementing this kind of behaviour is non-trivial and
	it is precisely what hetvec set out to do.

	Using hetvec boils down to initializing a
	heterogenous vector parametrized by the types you need
	to deal with (again, such as Sphere and Triangle).
	Add elements to the hetvec either during construction
	or with push_back. In order to apply some function
	onto all elements of the unordered cartesian product
	you call perform(Fcns fcns) where Fcns is some class
	with a method called f(Sphere s, Triangle t) or
	f(Triangle t, Triangle t). You can add as many or as
	few methods to the Fcns as you like. In other words,
	it is the methods within Fcns you, as a user of hetvec,
	must implement in order to specialize the behaviour for
	each element in the unordered cartesian product.

	hetvec tries to mimic std::vector as much as possible
	but the heterogenous nature of hetvec disallows for
	many typical features (such as accesing an element) **/


template<typename... Ts> struct hetvec;
template<> struct hetvec<> {
protected:
	template<typename Fcns>
	void perform(const Fcns& fcns) const { }
	template<typename T, typename Fcns>
	void perform(const T& t, const Fcns& fcns) { }
	template<typename T, typename U, typename Fcns>
	void f(const T& a1, const U& a2, const Fcns& fcns) {
		fcns.f(a1, a2);
		fcns.f(a2, a1); //Client should only have to define one function and that function is assumed to be commutative
	}
	template<typename T, typename Fcns>
	void f(const T& a1, const T& a2, const Fcns& fcns) {
		fcns.f(a1, a2);
	}
	size_t size() const {
		return 0;
	}

	void clear() { }
};

#include <stdio.h>

template<typename T, typename... Ts>
struct hetvec<T, Ts...> : hetvec<Ts...> {
protected:
	std::vector<T> v;

	//Test forwarded element against my elements and forward the forwarded element
	template<typename U, typename Fcns>
	void perform(const U& u, const Fcns& fcns) {
		std::for_each(v.begin(), v.end(), [this, &u, &fcns](const T& t) { hetvec<Ts...>::f(t, u, fcns); } );
		hetvec<Ts...>::perform(u, fcns);
	}

public:
	explicit hetvec() { }
	template<typename... Us>
	explicit hetvec(const T& t, const Us& ...us) : hetvec<T, Ts...>(us...) {
		v.push_back(t);
	}
	template<typename... Us>
	explicit hetvec(const Us& ...us) : hetvec<Ts...>(us...) { }

	template<typename... Us>
	explicit hetvec(const T&& t, const Us&& ...us) : hetvec<T, Ts...>(std::move(us)...) {
		v.push_back(t);
	}
	template<typename... Us>
	explicit hetvec(const Us&& ...us) : hetvec<Ts...>(std::move(us)...) { }

	/** When adding, recurses up the parent-list until parent who correspond to T
		is found, at which non-generic add takes precedence over the generic add **/
	template<typename U>
	void push_back(const U& u) { hetvec<Ts...>::push_back(u); }
	void push_back(const T& t) { v.push_back(t); }

	template<typename U>
	void push_back(const U&& u) { hetvec<Ts...>::push_back(std::move(u)); }
	void push_back(const T&& t) { v.push_back(std::move(t)); }
	
	//Test all my elements against eachother and forward each element to parent
	template<typename Fcns>
	void perform(const Fcns& fcns = Fcns()) {
		//Test all T against all other T (same-type testing)
		for(size_t i = 0; i < v.size(); i++) {
			for(size_t j = i+1; j < v.size(); j++) {
				hetvec<Ts...>::f(v[i], v[j], fcns);
			}
		}
		//Forward all elements to parent and then tell parent to do same-type testing
		std::for_each(v.begin(), v.end(), [this, &fcns](const T& t) { hetvec<Ts...>::perform(t, fcns); } );
		hetvec<Ts...>::perform(fcns);
	}

	size_t size() const {
		return v.size() + hetvec<Ts...>::size();
	}

	void clear() {
		v.clear();
		hetvec<Ts...>::clear();
	}

	bool empty() const {
		return size() == 0;
	}
};

#endif //HETVEC_HPP
