/**
 * Operation Counter C++ 
 * Emanuele Ruffaldi
 *
 * TODO: add operation overloads based on code
 * TODO: ideas for supporting if/else branching:
 *		if (expr) stmt1 else stmt2
 *		if (expr) stmt1 => trivial
 *		c.all = c.expr + max(c.stmt1,c.stmt2)
 *	Solution
 *		GLOBAL stack (all types)
 *		if (expr) =>   ifobj(expr); stmt1; elseobj; stmt2
 *		PROBLEM NOT SOLVED we need to enter both statements AND link ifobj1 to elseobj2
 *
 *
 * Cortex M4F: load (2), mover(1),add(1),mul(1),mac(1),divide(14),sqrt(14)
 */

#pragma once
#include <iostream>
#include <type_traits>
#include <cstdio>
#include <array>

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
	static int macs ;
	static int divs;
	static int trig ;
	static int isqrt;
	static int comps ;
	static int cast ;
	using serialized_t = std::array<int,8> ;

	bool lastmul = false;

	static void reset()
	{
		adds = muls = macs = trig = isqrt = comps = cast = divs = 0;
	}
	static void dump()
	{
		char buf[256];
		sprintf(buf,"%10s adds:%3d muls:%3d divs:%3d macs:%3d trig:%d sqrt:%3d comps:%3d cast:%3d\n",
			base_t::type(),adds,muls,divs,macs,trig,isqrt,comps,cast);
		std::cout << buf;
	}

	static serialized_t serialize() { return {adds,muls,divs,trig,isqrt,comps,cast}; }
	static void unserialzie(const serialized_t & x) 
	{
		adds = x[0];
		muls = x[1];
		divs = x[2];
		macs = x[3];
		trig = x[4];
		isqrt = x[5];
		comps = x[6];
		cast = x[7];
	}

	static serialized_t sum(const serialized_t & a, const serialized_t & b)
	{
		serialized_t o;
		for(int i = 0; i < a.size(); i++)
			o[i] = a[i]+b[i];
		return o;
	}

	// note: here max is the max of single ops, but actually it should be max of OVERALL
	static serialized_t max(const serialized_t & a, const serialized_t & b)
	{
		serialized_t o;
		for(int i = 0; i < a.size(); i++)
			o[i] = a[i] > b[i] ? a[i] : b[i];
		return o;
	}

};

template <class Base> int counted_base_t<Base>::adds = 0;
template <class Base> int counted_base_t<Base>::muls = 0;
template <class Base> int counted_base_t<Base>::divs = 0;
template <class Base> int counted_base_t<Base>::macs = 0;
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
		 return counted_t<T>();
	}

	counted_t operator + (const counted_t & o) const
	{
		if(lastmul || o.lastmul)
		{
			muls--;
			macs++;
			// next will be regular
		}
		else
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
		divs++;
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

	counted_t() {}
	counted_t(int x) {}
	counted_t(float x) {}

	template <class T>
	operator counted_t<T> ()
	{	
		 cast +=  conditional_value<std::is_same<counted_t<T>,base_t>::value, int, 0, 1>::value;
	}

	counted_t operator + (const counted_t & o) const
	{
		if(lastmul || o.lastmul)
		{
			macs++;
			muls--;
			// optimize one of the two muls
		}
		else
			adds++;
		return counted_t();
	}

	counted_t operator + (const float & o) const
	{
		if(lastmul)
		{
			macs++;
			muls--;
		}
		else
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
		if(lastmul || o.lastmul)
		{
			macs++;
			muls--;
			// optimize on
		}
		else
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
		if(lastmul || o.lastmul)
		{
			macs++;
			muls--;
		}
		else
			adds++;
		return counted_t();
	}

	counted_t operator * (const counted_t & o) const
	{
		muls++;
		auto r = counted_t();
		r.lastmul = true;
		return r;
	}

	counted_t operator / (const counted_t & o) const
	{
		divs++;
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

inline counted_t<float> sqrt(const counted_t<float> & in)
{
	counted_t<float>::isqrt++;
	return counted_t<float>();
}

template <class T>
counted_t<T> operator / (T x, const counted_t<T> & xx)
{
	counted_t<T>::divs++;
	return counted_t<T>();
}

template <class T>
counted_t<T> operator * (T x, const counted_t<T> & xx)
{
	counted_t<T>::muls++;
	auto r = counted_t<T>();
	r.lastmul = true;
	return r;
}

template <class T>
counted_t<T> operator * (int x, const counted_t<T> & xx)
{
	counted_t<T>::muls++;
	auto r = counted_t<T>();
	r.lastmul = true;
	return r;
}