// MIT License
//
// Copyright (c) 2020, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

/** \file timemory/variadic/component_hybrid.hpp
 * \headerfile variadic/component_hybrid.hpp "timemory/variadic/component_hybrid.hpp"
 * This is the C++ class that bundles together components and enables
 * operation on the components as a single entity
 *
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

#include "timemory/general/source_location.hpp"
#include "timemory/variadic/component_list.hpp"
#include "timemory/variadic/component_tuple.hpp"
#include "timemory/variadic/types.hpp"

//======================================================================================//

namespace tim
{
//======================================================================================//
// variadic list of components
//
template <typename _CompTuple, typename _CompList>
class component_hybrid
{
    static_assert((_CompTuple::is_component_tuple || _CompTuple::is_auto_tuple) &&
                      (_CompList::is_component_list || _CompList::is_auto_list),
                  "Error! _CompTuple must be tim::component_tuple<...> and _CompList "
                  "must be tim::component_list<...>");

    static const std::size_t num_elements = _CompTuple::size() + _CompList::size();

    // manager is friend so can use above
    friend class manager;

    template <typename _TupleC, typename _ListC>
    friend class auto_hybrid;

public:
    using this_type       = component_hybrid<_CompTuple, _CompList>;
    using tuple_type      = typename _CompTuple::component_type;
    using list_type       = typename _CompList::component_type;
    using tuple_data_type = typename tuple_type::data_type;
    using list_data_type  = typename list_type::data_type;
    using data_type       = decltype(std::tuple_cat(std::declval<tuple_type>().data(),
                                              std::declval<list_type>().data()));
    using type_tuple      = tim::impl::tuple_concat<typename tuple_type::type_tuple,
                                               typename list_type::type_tuple>;

    using tuple_type_list = typename tuple_type::data_type;
    using list_type_list  = typename list_type::reference_type;
    using data_value_type = decltype(std::tuple_cat(std::declval<tuple_type>().get(),
                                                    std::declval<list_type>().get()));
    using data_label_type =
        decltype(std::tuple_cat(std::declval<tuple_type>().get_labeled(),
                                std::declval<list_type>().get_labeled()));

    // used by gotcha
    using component_type = component_hybrid<tuple_type, list_type>;
    using auto_type      = auto_hybrid<tuple_type, list_type>;

    // used by component hybrid and gotcha
    static constexpr bool is_component_list   = false;
    static constexpr bool is_component_tuple  = false;
    static constexpr bool is_component_hybrid = true;
    static constexpr bool is_component_type   = true;
    static constexpr bool is_auto_list        = false;
    static constexpr bool is_auto_tuple       = false;
    static constexpr bool is_auto_hybrid      = false;
    static constexpr bool is_auto_type        = false;
    static constexpr bool is_component        = false;

    // used by gotcha component to prevent recursion
    static constexpr bool contains_gotcha =
        (tuple_type::contains_gotcha || list_type::contains_gotcha);

    using size_type           = int64_t;
    using captured_location_t = source_location::captured;
    using init_func_t         = std::function<void(this_type&)>;

public:
    //----------------------------------------------------------------------------------//
    //
    static void init_storage()
    {
        tuple_type::init_storage();
        list_type::init_storage();
    }

    //----------------------------------------------------------------------------------//
    //
    static init_func_t& get_initializer()
    {
        static init_func_t _instance = [](this_type&) {};
        return _instance;
    }

public:
    explicit component_hybrid()
    : m_tuple()
    , m_list()
    {}

    template <typename _Func = init_func_t>
    explicit component_hybrid(const string_t& key, const bool& store = false,
                              const bool&  flat  = settings::flat_profile(),
                              const _Func& _func = this_type::get_initializer())
    : m_tuple(key, store, flat)
    , m_list(key, store, flat)
    {
        _func(*this);
    }

    template <typename _Func = init_func_t>
    explicit component_hybrid(const captured_location_t& loc, const bool& store = false,
                              const bool&  flat  = settings::flat_profile(),
                              const _Func& _func = this_type::get_initializer())
    : m_tuple(loc, store, flat)
    , m_list(loc, store, flat)
    {
        _func(*this);
    }

    ~component_hybrid() {}

    //------------------------------------------------------------------------//
    //      Copy construct and assignment
    //------------------------------------------------------------------------//
    component_hybrid(const tuple_type& _tuple, const list_type& _list)
    : m_tuple(_tuple)
    , m_list(_list)
    {}

    component_hybrid(const component_hybrid&) = default;
    component_hybrid(component_hybrid&&)      = default;

    component_hybrid& operator=(const component_hybrid& rhs) = default;
    component_hybrid& operator=(component_hybrid&&) = default;

    component_hybrid clone(bool store, bool flat)
    {
        return component_hybrid(m_tuple.clone(store, flat), m_list.clone(store, flat));
    }

public:
    tuple_type&       get_tuple() { return m_tuple; }
    const tuple_type& get_tuple() const { return m_tuple; }
    list_type&        get_list() { return m_list; }
    const list_type&  get_list() const { return m_list; }

    tuple_type&       get_first() { return m_tuple; }
    const tuple_type& get_first() const { return m_tuple; }
    list_type&        get_second() { return m_list; }
    const list_type&  get_second() const { return m_list; }

    tuple_type&       get_lhs() { return m_tuple; }
    const tuple_type& get_lhs() const { return m_tuple; }
    list_type&        get_rhs() { return m_list; }
    const list_type&  get_rhs() const { return m_list; }

public:
    inline int64_t  laps() const { return m_tuple.laps(); }
    inline string_t key() const { return m_tuple.key(); }
    inline uint64_t hash() const { return m_tuple.hash(); }
    inline bool     store() const { return m_tuple.store(); }
    inline void     rekey(const string_t& _key)
    {
        m_tuple.rekey(_key);
        m_list.rekey(_key);
    }

public:
    //----------------------------------------------------------------------------------//
    // get the size
    //
    static constexpr std::size_t size() { return num_elements; }

    //----------------------------------------------------------------------------------//
    // insert into graph
    inline void push()
    {
        m_tuple.push();
        m_list.push();
    }

    //----------------------------------------------------------------------------------//
    // pop out of graph
    inline void pop()
    {
        m_tuple.pop();
        m_list.pop();
    }

    //----------------------------------------------------------------------------------//
    // measure functions
    void measure()
    {
        m_tuple.measure();
        m_list.measure();
    }

    //----------------------------------------------------------------------------------//
    // start/stop functions
    void start()
    {
        m_tuple.start();
        m_list.start();
    }

    void stop()
    {
        m_tuple.stop();
        m_list.stop();
    }

    //----------------------------------------------------------------------------------//
    // construct the objects that have constructors with matching arguments
    //
    template <typename... _Args>
    void construct(_Args&&... _args)
    {
        m_tuple.construct(std::forward<_Args>(_args)...);
        m_list.construct(std::forward<_Args>(_args)...);
    }

    //----------------------------------------------------------------------------------//
    // mark a beginning position in the execution (typically used by asynchronous
    // structures)
    //
    template <typename... _Args>
    void mark_begin(_Args&&... _args)
    {
        m_tuple.mark_begin(std::forward<_Args>(_args)...);
        m_list.mark_begin(std::forward<_Args>(_args)...);
    }

    //----------------------------------------------------------------------------------//
    // mark a beginning position in the execution (typically used by asynchronous
    // structures)
    //
    template <typename... _Args>
    void mark_end(_Args&&... _args)
    {
        m_tuple.mark_end(std::forward<_Args>(_args)...);
        m_list.mark_end(std::forward<_Args>(_args)...);
    }

    //----------------------------------------------------------------------------------//
    // perform a auditd operation (typically for GOTCHA)
    //
    template <typename... _Args>
    void audit(_Args&&... _args)
    {
        m_tuple.audit(std::forward<_Args>(_args)...);
        m_list.audit(std::forward<_Args>(_args)...);
    }

    //----------------------------------------------------------------------------------//
    // recording
    //
    this_type& record()
    {
        m_tuple.record();
        m_list.record();
        return *this;
    }

    //----------------------------------------------------------------------------------//
    // reset data
    //
    void reset()
    {
        m_tuple.reset();
        m_list.reset();
    }

    //----------------------------------------------------------------------------------//
    // get data
    //
    data_value_type get() const
    {
        return std::tuple_cat(get_lhs().get(), get_rhs().get());
    }

    //----------------------------------------------------------------------------------//
    // reset data
    //
    data_label_type get_labeled() const
    {
        return std::tuple_cat(get_lhs().get_labeled(), get_rhs().get_labeled());
    }

    //----------------------------------------------------------------------------------//
    // this_type operators
    //
    this_type& operator-=(const this_type& rhs)
    {
        m_tuple -= rhs.m_tuple;
        m_list -= rhs.m_list;
        return *this;
    }

    this_type& operator-=(this_type& rhs)
    {
        m_tuple -= rhs.m_tuple;
        m_list -= rhs.m_list;
        return *this;
    }

    this_type& operator+=(const this_type& rhs)
    {
        m_tuple += rhs.m_tuple;
        m_list += rhs.m_list;
        return *this;
    }

    this_type& operator+=(this_type& rhs)
    {
        m_tuple += rhs.m_tuple;
        m_list += rhs.m_list;
        return *this;
    }

    //----------------------------------------------------------------------------------//
    // generic operators
    //
    template <typename _Op>
    this_type& operator-=(_Op&& rhs)
    {
        m_tuple -= std::forward<_Op>(rhs);
        m_list -= std::forward<_Op>(rhs);
        return *this;
    }

    template <typename _Op>
    this_type& operator+=(_Op&& rhs)
    {
        m_tuple += std::forward<_Op>(rhs);
        m_list += std::forward<_Op>(rhs);
        return *this;
    }

    template <typename _Op>
    this_type& operator*=(_Op&& rhs)
    {
        m_tuple *= std::forward<_Op>(rhs);
        m_list *= std::forward<_Op>(rhs);
        return *this;
    }

    template <typename _Op>
    this_type& operator/=(_Op&& rhs)
    {
        m_tuple /= std::forward<_Op>(rhs);
        m_list /= std::forward<_Op>(rhs);
        return *this;
    }

    //----------------------------------------------------------------------------------//
    // friend operators
    //
    friend this_type operator+(const this_type& lhs, const this_type& rhs)
    {
        this_type tmp(lhs);
        tmp.m_tuple += rhs.m_tuple;
        tmp.m_list += rhs.m_list;
        return tmp;
    }

    friend this_type operator-(const this_type& lhs, const this_type& rhs)
    {
        this_type tmp(lhs);
        tmp.m_tuple -= rhs.m_tuple;
        tmp.m_list -= rhs.m_list;
        return tmp;
    }

    template <typename _Op>
    friend this_type operator*(const this_type& lhs, _Op&& rhs)
    {
        this_type tmp(lhs);
        tmp.m_tuple *= std::forward<_Op>(rhs);
        tmp.m_list *= std::forward<_Op>(rhs);
        return tmp;
    }

    template <typename _Op>
    friend this_type operator/(const this_type& lhs, _Op&& rhs)
    {
        this_type tmp(lhs);
        tmp.m_tuple /= std::forward<_Op>(rhs);
        tmp.m_list /= std::forward<_Op>(rhs);
        return tmp;
    }

    //----------------------------------------------------------------------------------//
    friend std::ostream& operator<<(std::ostream& os, const this_type& obj)
    {
        std::stringstream tss, lss;

        obj.m_tuple.template print<true, false>(tss);
        obj.m_list.template print<false, false>(lss);

        if(tss.str().length() > 0)
            os << tss.str();
        if(tss.str().length() > 0 && lss.str().length() > 0)
            os << ", ";
        if(lss.str().length() > 0)
            os << lss.str();

        if(obj.m_tuple.laps() > 0)
            os << " [laps: " << obj.m_tuple.laps() << "]";

        return os;
    }

    //----------------------------------------------------------------------------------//
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar(cereal::make_nvp("tuple", m_tuple));
        ar(cereal::make_nvp("list", m_list));
    }

    //----------------------------------------------------------------------------------//
    static void print_storage()
    {
        tuple_type::print_storage();
        list_type::print_storage();
    }

public:
    inline data_type data() const
    {
        return std::tuple_cat(m_tuple.data(), m_list.data());
    }

public:
    //----------------------------------------------------------------------------------//
    //  get access to a type
    //
    template <typename _Tp,
              enable_if_t<(is_one_of<_Tp, tuple_type_list>::value == true), int> = 0>
    auto get() -> decltype(std::declval<tuple_type>().template get<_Tp>())
    {
        return m_tuple.template get<_Tp>();
    }

    template <typename _Tp,
              enable_if_t<(is_one_of<_Tp, list_type_list>::value == true), int> = 0>
    auto get() -> decltype(std::declval<list_type>().template get<_Tp>())
    {
        return m_list.template get<_Tp>();
    }

public:
    //----------------------------------------------------------------------------------//
    //  apply a member function to a type
    //
    template <typename _Tp, typename _Func, typename... _Args,
              enable_if_t<(is_one_of<_Tp, tuple_type_list>::value), int> = 0,
              enable_if_t<!(is_one_of<_Tp, list_type_list>::value), int> = 0>
    void type_apply(_Func&& _func, _Args&&... _args)
    {
        m_tuple.template type_apply<_Tp>(_func, std::forward<_Args>(_args)...);
    }

    template <typename _Tp, typename _Func, typename... _Args,
              enable_if_t<!(is_one_of<_Tp, tuple_type_list>::value), int> = 0,
              enable_if_t<(is_one_of<_Tp, list_type_list>::value), int>   = 0>
    void type_apply(_Func&& _func, _Args&&... _args)
    {
        m_list.template type_apply<_Tp>(_func, std::forward<_Args>(_args)...);
    }

    template <typename _Tp, typename _Func, typename... _Args,
              enable_if_t<!(is_one_of<_Tp, tuple_type_list>::value), int> = 0,
              enable_if_t<!(is_one_of<_Tp, list_type_list>::value), int>  = 0>
    void type_apply(_Func&&, _Args&&...)
    {}

protected:
    // objects
    tuple_type m_tuple = tuple_type();
    list_type  m_list  = list_type();
};

//--------------------------------------------------------------------------------------//

}  // namespace tim

//--------------------------------------------------------------------------------------//

#include "timemory/variadic/bits/component_hybrid.hpp"
