// Copyright (c) 2010 Thomas Nelson

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.


/*------------------------------------------------------------------------
	These converters provide direct read-only access to the STL collection 
	classes presenting them to Lua as tables. Identity is preserved using a, 
	so that weak reference so that as long as Lua has a reference to the 
	collection in question the following will always hold true:

	tab1 = myObj.Collection;
	tab2 = myObj.Collection;

	tab1 == tab2;

	The converters should not interact with collections passed as parameters,
	only return values from functions.

	The # operator:
	The # operator does not correctly check the __len metamethod for tables
	in Lua 5.1, to get the length of the table get its metatable and call the 
	__len method directly as follows:

	local mt = getmetatable(table);
	length = mt.__len(table);

	Iteration:
	In order to correctly iterate a table Lua needs to be taught to use the 
	__next and __index metamethods. And pairs needs to be expressed in terms
	of next. The following code should be somewhere in your Lua program:

	rawnext = next;
	function next(t,k)
		local m = getmetatable(t);
		local n = m and m.__next or rawnext;
		return n(t,k);
	end
	
	function pairs(t)
		return next, t, nil;
	end

	Supported collections:
		vector: keys are the index in the array.  Indexes are adjusted to be
				1 based in keeping with Lua conventions.
			In other words:
			c++ vector[0] is equivalent to Lua table[1]

		deque:	same as vector

		list:	same as vector

		set:	STL sets are inherently ordered.  The key is the set member's 
				position in the set. (1 based)

		multiset:	same as set.

		map:	key/value pairs ordered by the key

	Usage:
	c++:
		class MyObject
		{
			
			std::vector<std::string> m_MyVector;

			const std::vector<std::string> *getVector() const
			{	return &m_MyVector;
			}

			void bindScripting(lua_State *L)
			{
				using namespace luabind;
				module(pState)
				[
					class_<MyObject>("MyObject")
						.property("Vector",		&MyObject::getVector)
				];
			}
		};

	Lua:
		-- don't forget the "next" and "pairs" changes above
		obj = MyObject;
		tbl = obj.Vector;

		for k, v in pairs(tbl) do
			print( 'tbl[' .. k .. '] = ' .. v );
		end


--------------------------------------------------------------------------
*/

#ifndef LUABIND_STL_CONTAINER_CONVERTER_HPP
# define LUABIND_STL_CONTAINER_CONVERTER_HPP

# include <luabind/get_main_thread.hpp>
# include <luabind/handle.hpp>
# include <luabind/detail/policy.hpp>
# include <luabind/detail/stl_container_adapter.hpp>

namespace luabind {


#define LUABIND_STL_CONVERTER( type )										\
template <class T>															\
struct default_converter< type< T > *>										\
{																			\
    typedef boost::mpl::false_		is_native;								\
																			\
	void apply(lua_State* L, type<T> * p)									\
    {																		\
		detail::stl_adapter<type< T > >::container_to(L, p);				\
    }																		\
																			\
    template <class U>														\
    void converter_postcall(lua_State*, U const&, int)						\
    {}																		\
};

LUABIND_STL_CONVERTER(std::vector)
LUABIND_STL_CONVERTER(std::deque)
LUABIND_STL_CONVERTER(std::list)
LUABIND_STL_CONVERTER(std::set)
LUABIND_STL_CONVERTER(std::multiset)

#undef LUABIND_STL_CONVERTER

template <class K, class V>
struct default_converter< const std::map< K, V > *>
{
	typedef boost::mpl::false_	is_native;

// 	template <class U>
// 	int match(lua_State* L, U, int index)				
// 	{													
// 		return detail::stl_adapter<std::map< K, V > >::container_match(L, index);	
// 	}													
// 
// 	template <class U>									
// 	std::map< K, V > *apply(lua_State* L, U, int index)			
// 	{													
// 		return detail::stl_adapter<std::map< K, V > >::container_from(L, index);	
// 	}													

	void apply(lua_State* L, const std::map< K, V > * p)
	{													
		detail::stl_adapter<std::map< K, V > >::container_to(L, p);			
	}													

	template <class U>									
	void converter_postcall(lua_State*, U const&, int)	
	{}													
};


} // namespace luabind

#endif // LUABIND_SHARED_PTR_CONVERTER_090211_HPP
