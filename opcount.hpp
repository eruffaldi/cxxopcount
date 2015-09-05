/**
 * Operation Counter C++ 
 * Emanuele Ruffaldi
 *
 * TODO: add operation overloads based on code 
 */

#pragma once
#include <iostream>
#include <type_traits>
#include <cstdio>

/// support class
template<bool B, class Q, Q T, Q F>
struct conditional_value { static constexpr Q value = T;  };
 
template<class Q, Q T, Q F>
struct conditional_value<false, Q, T, F> { static constexpr Q value = F; };


/// base class that provides counters and dump
template <class Base>
struct counted_base_t
{
	using base_t = Base;
	static int adds ;
	static int muls ;
	static int trig ;
	static int isqrt;
	static int comps ;
	static int cast ;

	static void dump()
	{
		char buf[256];
		sprintf(buf,"%10s adds:%3d muls:%3d trig:%d sqrt:%3d comps:%3d cast:%3d\n",
			base_t::type(),adds,muls,trig,isqrt,comps,cast);
		std::cout << buf;
	}
};

template <class Base> int counted_base_t<Base>::adds = 0;
template <class Base> int counted_base_t<Base>::muls = 0;
template <class Base> int counted_base_t<Base>::trig = 0;
template <class Base> int counted_base_t<Base>::isqrt = 0;
template <class Base> int counted_base_t<Base>::comps = 0;
template <class Base> int counted_base_t<Base>::cast = 0;


template <class T>
struct counted_t;

/// boolean specialization
template <>
struct counted_t<bool>: public counted_base_t<counted_t<bool> >
{
	counted_t() {}

	/// note not explicit
	counted_t(bool x) {}

	static const char * type(){ return "bool"; }

	counted_t<bool> operator != (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	

	counted_t<bool> operator == (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}

	counted_t<bool> operator && (const counted_t & o) const
	{
		return counted_t<bool>();
	}

	counted_t<bool> operator || (const counted_t & o) const
	{
		return counted_t<bool>();
	}	
};

template <>
struct counted_t<int>: public counted_base_t<counted_t<int> >
{
	counted_t() {}
	counted_t(int x) {}

	static const char * type() { return "int"; }

	/// cast counts only if not to same type
	template <class T>
	operator counted_t<T> ()
	{	
		 cast +=  conditional_value<std::is_same<counted_t<T>,base_t>::value, int, 0, 1>::value;
	}

	counted_t operator + (const counted_t & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator - (const counted_t & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator * (const counted_t & o) const
	{
		muls++;
		return counted_t();
	}

	counted_t operator / (const counted_t & o) const
	{
		muls++;
		return counted_t();
	}	

	counted_t<bool> operator < (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	

	counted_t<bool> operator >= (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	

	counted_t<bool> operator != (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	

	counted_t<bool> operator == (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	
};

template <>
struct counted_t<float>: public counted_base_t<counted_t<float> >
{
	static const char * type(){ return  "float";}

	template <class T>
	operator counted_t<T> ()
	{	
		 cast +=  conditional_value<std::is_same<counted_t<T>,base_t>::value, int, 0, 1>::value;
	}

	counted_t operator + (const counted_t & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator + (const float & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator - (const float & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator += (const counted_t & o) const
	{
		adds++;
		return counted_t();
	}

	// no cost
	counted_t operator - () const
	{
		return counted_t();
	}

	counted_t operator - (const counted_t & o) const
	{
		adds++;
		return counted_t();
	}

	counted_t operator * (const counted_t & o) const
	{
		muls++;
		return counted_t();
	}

	counted_t operator / (const counted_t & o) const
	{
		muls++;
		return counted_t();
	}	

	counted_t<bool> operator < (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	
	counted_t<bool> operator >= (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	
	counted_t<bool> operator != (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}	
	counted_t<bool> operator == (const counted_t & o) const
	{
		comps++;
		return counted_t<bool>();
	}		

};

inline counted_t<float> cos(const counted_t<float> & in)
{
	counted_t<float>::trig++;
	return counted_t<float>();
}

inline counted_t<float> sin(const counted_t<float> & in)
{
	counted_t<float>::trig++;
	return counted_t<float>();
}

template <class T>
counted_t<T> operator / (T x, const counted_t<T> & xx)
{
	counted_t<T>::muls++;
	return counted_t<T>();
}

template <class T>
counted_t<T> operator * (T x, const counted_t<T> & xx)
{
	counted_t<T>::muls++;
	return counted_t<T>();
}