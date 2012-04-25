
#ifndef BOOST_MPL_FIXED_C_HPP_INCLUDED
#define BOOST_MPL_FIXED_C_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: math/fixed_c.hpp 1.3 2010/04/23 08:48:46EDT Adams, Nathan (nadams9) Rel  $
// $Date: 2010/04/23 08:48:46EDT $
// $Revision: 1.3 $

#include <boost/mpl/aux_/config/static_constant.hpp>

namespace boost { namespace mpl {

template<
      long IntegerPart
    , unsigned long FractionPart
    >
struct fixed_c
{
    BOOST_STATIC_CONSTANT(long, integer_part = IntegerPart);
    BOOST_STATIC_CONSTANT(unsigned long, fraction_part = FractionPart);
    typedef fixed_c<IntegerPart, FractionPart> type;
    
    fixed_c() {}
};

}}

#endif // BOOST_MPL_FIXED_C_HPP_INCLUDED
