//  MIT License
//
//  Copyright (c) 2020, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

/** \file extern/auto_user_bundle.hpp
 * \headerfile extern/auto_user_bundle.hpp "timemory/extern/auto_user_bundle.hpp"
 * Extern template declarations
 *
 */

#pragma once

//--------------------------------------------------------------------------------------//
// auto_user_bundle
//
#if defined(TIMEMORY_EXTERN_TEMPLATES) && !defined(TIMEMORY_BUILD_EXTERN_TEMPLATE)

#    include "timemory/components.hpp"
#    include "timemory/utility/macros.hpp"
#    include "timemory/variadic/auto_hybrid.hpp"
#    include "timemory/variadic/auto_list.hpp"
#    include "timemory/variadic/auto_tuple.hpp"

// clang-format off

TIMEMORY_DECLARE_EXTERN_TUPLE(auto_user_bundle_t, ::tim::component::user_tuple_bundle)
TIMEMORY_DECLARE_EXTERN_LIST(auto_user_bundle_t, ::tim::component::user_list_bundle)
TIMEMORY_DECLARE_EXTERN_HYBRID(auto_user_bundle_t)

// clang-format on

#endif

//======================================================================================//
