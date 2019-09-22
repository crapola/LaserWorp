/*******************************************************************************

	A vector that keeps track of changes.

*******************************************************************************/
#pragma once

#include <limits>
#include <vector>

template<class T,class Ta=std::allocator<T>>
class BackCache: protected std::vector<T,Ta>
{
	// =========================================================================
	// Vector
	// =========================================================================
	typedef std::vector<T,Ta> Base;

	using Base::emplace;
	using Base::emplace_back;
	using typename Base::iterator;
	using typename Base::reference;
public:
	using typename Base::const_pointer;
	using typename Base::const_reference;
	using typename Base::const_reverse_iterator;
	using typename Base::const_iterator;
	using typename Base::size_type;
	using typename Base::value_type;

	using Base::Base; // Constructors

	//    Base::assign;
	//    Base::at;
	using Base::back;
	//    Base::begin;
	using Base::capacity;
	using Base::cbegin;
	using Base::cend;
	//    Base::clear;
	using Base::crbegin;
	using Base::crend;
	//    Base::data;
	//----Base::emplace;
	//----Base::emplace_back;
	using Base::empty;
	//    Base::end;
	//    Base::erase;
	//    Base::front;
	//    Base::insert;
	using Base::max_size;
	//    Base::operator=;
	using Base::operator[]; // Using base's const version
	//    Base::pop_back;
	//    Base::push_back;
	//    Base::rbegin;
	//    Base::rend;
	using Base::reserve;
	//    Base::resize;
	using Base::shrink_to_fit;
	using Base::size;
	//    Base::swap;

	// Assign
	void assign(size_type n,const value_type& val)
	{
		Base::assign(n,val);
		SetFull();
	}

	void assign(std::initializer_list<value_type> initlist)
	{
		Base::assign(initlist);
		SetFull();
	}

	template<typename Ti>
	void assign(Ti first, Ti last)
	{
		Base::assign(first,last);
		SetFull();
	}

	// At
	const_reference at(size_type n) const
	{
		return Base::at(n);
	}

	// Begin
	const_iterator begin() const
	{
		return Base::begin();
	};

	// Clear
	void clear()
	{
		Base::clear();
		_range.Reset();
	}

	// Data
	const_pointer data() const
	{
		return Base::data();
	}

	// End
	const_iterator end() const
	{
		return Base::end();
	}

	// Erase
	// Fixes the range, always extend to last element.
	const_iterator erase(const_iterator pos)
	{
		iterator it=Base::erase(pos);
		if (empty())
			_range.Reset();
		else
		{
			_range.Add(it-begin());
			_range._to=size()-1;
		}
		return it;
	}

	const_iterator erase(const_iterator first,const_iterator last)
	{
		iterator it=Base::erase(first,last);
		if (empty())
			_range.Reset();
		else if (first!=last)
		{
			_range.Add(first-begin());
			_range._to=size()-1;
		}
		return it;
	}

	// Front
	const_reference front() const
	{
		return Base::front();
	}

	// Insert
	const_iterator insert(const_iterator pos, const T& x)
	{
		_range.Add(pos-begin());
		_range._to=size();
		return Base::insert(pos,x);
	}

	const_iterator insert(const_iterator pos,T&& x)
	{
		_range.Add(pos-begin());
		_range._to=size();
		return emplace(pos, std::move(x));
	}

	const_iterator insert(const_iterator pos,std::initializer_list<T> l)
	{
		_range.Add(pos-begin());
		_range._to=size()+l.size()-1;
		return Base::insert(pos,l);
	}

	const_iterator insert(const_iterator pos, size_type n, const T& x)
	{
		_range.Add(pos-begin(),size()+n-1);
		return Base::insert(pos,n,x);
	}

	// Non-type template parameter with an optional name.
	template<typename InputIterator,
			 typename=std::_RequireInputIter<InputIterator>>
	const_iterator insert(const_iterator pos, InputIterator first,
						  InputIterator last)
	{
		_range.Add(pos-begin(),size()+last-first-1);
		return Base::insert(pos,first,last);
	}

	// Operators
	BackCache& operator=(const BackCache& o)
	{
		Base::operator=(o);
		_range=o._range;
		return *this;
	}

	reference operator[](size_type n)
	{
		_range.Add(n);
		return Base::operator[](n);
	}

	// Push&Pop
	void pop_back()
	{
		Base::pop_back();
		_range.Cap(size()-1);
	}

	void push_back(const T& x)
	{
		Base::push_back(x);
		_range.Add(size()-1,size()-1);
	}

	void push_back(T&& x)
	{
		Base::push_back(x);
		_range.Add(size()-1,size()-1);
	}

	// Reverse iterators
	const_reverse_iterator rbegin() const
	{
		return Base::rbegin();
	}

	const_reverse_iterator rend() const
	{
		return Base::rend();
	}

	// Resize and fix the range
	void resize(size_type ns)
	{
		Base::resize(ns);
		_range.Set(_range.From(),std::min(_range.To(),size()-1));
	}

	// Swapping changes everything
	void swap(Base& x)
	{
		Base::swap(x);
		_range.Set(0,size()-1);
	}

	// =========================================================================
	// BackCache
	// =========================================================================
public:
	size_type From() const
	{
		return _range.From();
	}
	size_type RangeSize() const
	{

		return _range.Size();
	}

	bool Pending() const
	{
		return _range;
	}

	void Reset()
	{
		_range.Reset();
	}

	void SetFull()
	{
		if (!empty())
			_range.Set(0,size()-1);
		else
			_range.Reset();
	}

	size_type To() const
	{
		return _range.To();
	}

private:
	struct Range
	{
	public:
		// Add elements to current range (boolean union)
		void Add(size_type n)
		{
			if (n<_from) _from=n;
			if (n>_to) _to=n;
		}

		void Add(size_type f,size_type t)
		{
			if (f<_from) _from=f;
			if (t>_to) _to=t;
		}

		// Cap end of range to s
		void Cap(size_type s)
		{
			_to=(_to>s)?s:_to;
		}

		size_type From() const
		{
			return _from;
		}
		void Reset()
		{
			_from=MAXSIZE;
			_to=0;
		}

		// Redefine the range, ingoring previous state
		void Set(size_type f,size_type t)
		{
			_from=f;
			_to=t;
		}

		size_type Size() const
		{
			if (!Pending()) return 0;
			return _to-_from+1;
		}

		size_type To() const
		{
			return _to;
		}

		operator bool() const // True if pending, valid range
		{
			return _from<=_to;
		}
		// Alias to biggest size_t.
		static constexpr size_type MAXSIZE=std::numeric_limits<size_type>::max();
		// Range.
		size_type _from{MAXSIZE},_to{0};
	} _range{};
};

// Compare to normal vector
template<typename _Tp, typename _Alloc>
inline bool operator==(const BackCache<_Tp, _Alloc>& __x,
					   const std::vector<_Tp, _Alloc>& __y)
{
	return (__x.size() == __y.size()
			&& std::equal(__x.begin(), __x.end(), __y.begin()));
}

template<typename _Tp, typename _Alloc>
inline bool operator==(const std::vector<_Tp, _Alloc>& __x,
					   const BackCache<_Tp, _Alloc>& __y)
{
	return (__x.size() == __y.size()
			&& std::equal(__x.begin(), __x.end(), __y.begin()));
}
