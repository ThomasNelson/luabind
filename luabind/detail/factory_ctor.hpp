// Copyright Thomas Nelson 2012. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !BOOST_PP_IS_ITERATING

# ifndef LUABIND_DETAIL_FACTORY_CTOR_081018_HPP
#  define LUABIND_DETAIL_FACTORY_CTOR_081018_HPP

#  include <luabind/get_main_thread.hpp>
#  include <luabind/object.hpp>
#  include <luabind/wrapper_base.hpp>
#  include <luabind/tag_function.hpp>
#  include <luabind/back_reference_fwd.hpp>
#  include <luabind/detail/inheritance.hpp>

#  include <boost/preprocessor/iteration/iterate.hpp>
#  include <boost/preprocessor/iteration/local.hpp>
#  include <boost/preprocessor/repetition/enum_params.hpp>
#  include <boost/preprocessor/repetition/enum_binary_params.hpp>
#  include <boost/preprocessor/repetition/enum.hpp>
#  include <boost/preprocessor/repetition/enum_trailing.hpp>

namespace luabind { 
namespace detail {

//===================================================================
template <class T, class Pointer, class F>
struct factory_aux;

template < class T, class Pointer, typename F > 
struct factory_ctor:
	public factory_aux< T, Pointer, F >
{
	factory_ctor ( F factory_):
		factory_aux<T, Pointer, F>( factory_ )
	{	}
};

#define BOOST_PP_ITERATION_LIMITS ( 0, LUABIND_MAX_ARITY )
#define BOOST_PP_FILENAME_1 <luabind/detail/factory_ctor.hpp>
#include BOOST_PP_ITERATE()
#undef BOOST_PP_ITERATION_LIMITS
#undef BOOST_PP_FILENAME_1

//===================================================================
}	
}
#endif
#else // BOOST_PP_IS_ITERATING

#define N BOOST_PP_ITERATION()

#define	METHOD_PARAM(z, I, P)	P##I a##I

#define VECN( I )	BOOST_PP_CAT(mpl::vector, BOOST_PP_ADD(I, 2))

#define SIG_DEF_ARGS( I, P )	<void, adl::argument BOOST_PP_ENUM_TRAINILNG_PARAMS(I, P )>

#define SIG_DEF( I, P ) BOOST_PP_CAT( VECN( I ), SIG_DEF_ARGS(I, P ) )


template <class T, class Pointer, class R BOOST_PP_ENUM_TRAILING_PARAMS( N, typename P )>
struct factory_aux<T, Pointer, R (*)( BOOST_PP_ENUM_PARAMS( N, P ) ) >
{
	typedef pointer_holder<Pointer, T> holder_type;

	factory_aux ( R ( *factory_ )( BOOST_PP_ENUM_PARAMS( N, P ) ) ):
		m_factory(factory_)
	{	}

	void operator () (adl::argument const &self_  
			BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM( N, METHOD_PARAM, P )) const
	{
		object_rep* self = touserdata<object_rep>(self_);
		class_rep* cls = self->crep();
		typedef pointer_holder<Pointer, T> holder_type;

		R instance( m_factory( BOOST_PP_ENUM_PARAMS( N, a ) ) );

		inject_backref(self_.interpreter(), -(N + 1), instance.get(), instance.get());

		void* storage = self->allocate(sizeof(holder_type));

		self->set_instance(new (storage) holder_type(
			instance, registered_class<T>::id, instance.get(), cls));
	}

	R ( * m_factory ) ( BOOST_PP_ENUM_PARAMS( N, P ) );

//	typedef SIG_DEF( N, P ) Signature;
	typedef VECN( N ) <void, adl::argument BOOST_PP_ENUM_TRAILING_PARAMS( N, P )> Signature;
};

#undef N
#undef METHOD_PARAM
#undef VECN
#undef SIG_DEF
#undef SIG_DEF_ARGS

#endif

