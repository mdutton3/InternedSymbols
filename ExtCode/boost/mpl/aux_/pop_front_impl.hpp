
#ifndef BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
#define BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: aux_/pop_front_impl.hpp 1.3 2010/04/23 08:42:33EDT Adams, Nathan (nadams9) Rel  $
// $Date: 2010/04/23 08:42:33EDT $
// $Revision: 1.3 $

#include <boost/mpl/pop_front_fwd.hpp>
#include <boost/mpl/aux_/traits_lambda_spec.hpp>
#include <boost/mpl/aux_/config/workaround.hpp>
#include <boost/mpl/aux_/config/msvc.hpp>

namespace boost { namespace mpl {

// no default implementation; the definition is needed to make MSVC happy

template< typename Tag >
struct pop_front_impl
{
    template< typename Sequence > struct apply
    // conservatively placed, but maybe should go outside surrounding
    // braces.
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300) 
    {
        typedef int type;
    }
#endif
    ;
};

BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1, pop_front_impl)

}}

#endif // BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
