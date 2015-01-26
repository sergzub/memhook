#ifndef MEMHOOK_MAPPING_TRAITS_HPP_INCLUDED
#define MEMHOOK_MAPPING_TRAITS_HPP_INCLUDED

#include "common.hpp"
#include "callstack.hpp"
#include "traceinfo.hpp"
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/container/string.hpp>
#include <boost/container/vector.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/cstdint.hpp>

namespace memhook {

template <typename Segment>
struct mapping_traits {
    typedef Segment                                        segment;
    typedef typename Segment::segment_manager              segment_manager;
    typedef interprocess::allocator<void, segment_manager> generic_allocator;
};

template <typename Traits>
struct mapped_traceinfo_types {
    typedef basic_traceinfo<
            interprocess::allocator<
                traceinfo_callstack_item,
                typename Traits::segment_manager
            >
        > base_type;
};

template <typename Traits>
struct mapped_traceinfo
        : basic_traceinfo<
                interprocess::allocator<traceinfo_callstack_item, typename Traits::segment_manager>
            > {
    typedef basic_traceinfo<
            interprocess::allocator<traceinfo_callstack_item, typename Traits::segment_manager>
        > base_type;
    typedef typename Traits::generic_allocator generic_allocator;

    explicit mapped_traceinfo(const generic_allocator &allocator_instance)
        : base_type(allocator_instance) {}

    mapped_traceinfo(uintptr_t address, size_t memsize, const system_clock::time_point &timestamp,
            const callstack_container &callstack, const generic_allocator &allocator_instance)
        : base_type(address, memsize, timestamp, callstack, allocator_instance) {}
};

template <typename Traits, typename CharT = char>
struct mapped_container : interprocess::interprocess_mutex {
    typedef typename Traits::segment_manager   segment_manager;
    typedef typename Traits::generic_allocator generic_allocator;
    typedef mapped_traceinfo<Traits>           traceinfo_type;

    typedef multi_index_container<
        traceinfo_type,
        multi_index::indexed_by<
            multi_index::hashed_unique<
                multi_index::const_mem_fun<traceinfo_base,
                    uintptr_t, &traceinfo_base::address
                >
            >,
            multi_index::ordered_non_unique<
                multi_index::const_mem_fun<traceinfo_base,
                    system_clock::time_point, &traceinfo_base::timestamp
                >
            >,
            multi_index::ordered_non_unique<
                multi_index::const_mem_fun<traceinfo_base,
                    std::size_t, &traceinfo_base::memsize
                >
            >
        >,
        interprocess::allocator<traceinfo_type, segment_manager>
    > indexed_container_t;
    indexed_container_t indexed_container;

    typedef container::basic_string<
            CharT,
            std::char_traits<CharT>,
            interprocess::allocator<CharT, segment_manager>
        > string_type;

    typedef std::pair<const uintptr_t, string_type> symbol_table_value_type;

    typedef unordered_map<
            uintptr_t,
            string_type,
            hash<uintptr_t>,
            std::equal_to<uintptr_t>,
            interprocess::allocator<symbol_table_value_type, segment_manager>
        > symbol_table_t;
    symbol_table_t symtab;
    symbol_table_t shltab;

    explicit mapped_container(const generic_allocator &allocator_instance)
        : indexed_container(typename indexed_container_t::ctor_args_list(), allocator_instance)
        , symtab(allocator_instance)
        , shltab(allocator_instance)
    {}
};

template <typename Traits>
struct mapping_cleaner;

} // memhook

#endif // MEMHOOK_MAPPING_TRAITS_HPP_INCLUDED
