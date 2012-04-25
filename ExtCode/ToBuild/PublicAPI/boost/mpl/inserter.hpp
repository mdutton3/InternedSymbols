
#ifndef BOOST_MPL_INSERTER_HPP_INCLUDED
#define BOOST_MPL_INSERTER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: inserter.hpp 1.3 2010/04/23 08:41:02EDT Adams, Nathan (nadams9) Rel  $
// $Date: 2010/04/23 08:41:02EDT $
// $Revision: 1.3 $

namespace boost { namespace mpl {

template<
      typename Sequence
    , typename Operation
    >
struct inserter
{
    typedef Sequence    state;
    typedef Operation   operation;
};

}}

#endif // BOOST_MPL_INSERTER_HPP_INCLUDED
