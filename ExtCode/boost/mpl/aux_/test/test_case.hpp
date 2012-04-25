
#ifndef BOOST_MPL_AUX_TEST_TEST_CASE_HPP_INCLUDED
#define BOOST_MPL_AUX_TEST_TEST_CASE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: aux_/test/test_case.hpp 1.3 2010/04/23 08:48:02EDT Adams, Nathan (nadams9) Rel  $
// $Date: 2010/04/23 08:48:02EDT $
// $Revision: 1.3 $

#include <boost/preprocessor/cat.hpp>

#define MPL_TEST_CASE() void BOOST_PP_CAT(test,__LINE__)()

#endif // BOOST_MPL_AUX_TEST_TEST_CASE_HPP_INCLUDED
