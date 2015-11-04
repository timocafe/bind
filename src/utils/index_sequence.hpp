#ifndef BIND_INDEX_SEQUENCE
#define BIND_INDEX_SEQUENCE

namespace bind {

    template<size_t... Indices>
    struct index_sequence {
        template<size_t N>
        using append = index_sequence<Indices..., N>;
    };

    template<size_t Size>
    struct make_index_sequence_impl {
        typedef typename make_index_sequence_impl<Size-1>::type::template append<Size-1> type;
    };

    template<>
    struct make_index_sequence_impl<0u> {
        typedef index_sequence<> type;
    };

    template<size_t Size>
    using make_index_sequence = typename make_index_sequence_impl<Size>::type;

}

#endif
