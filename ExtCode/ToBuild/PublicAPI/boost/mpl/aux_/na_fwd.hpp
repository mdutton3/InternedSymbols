
#ifndef BOOST_MPL_AUX_NA_FWD_HPP_INCLUDED
#define BOOST_MPL_AUX_NA_FWD_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: aux_/na_fwd.hpp 1.3 2010/04/23 08:42:28EDT Adams, Nathan (nadams9) Rel  $
// $Date: 2010/04/23 08:42:28EDT $
// $Revision: 1.3 $

#include <boost/mpl/aux_/adl_barrier.hpp>

BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

// n.a. == not available
struct na
{
    typedef na type;
    enum { value = 0 };
};

BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE
BOOST_MPL_AUX_ADL_BARRIER_DECL(na)

#endif // BOOST_MPL_AUX_NA_FWD_HPP_INCLUDED
