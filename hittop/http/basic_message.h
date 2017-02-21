#ifndef HITTOP_HTTP_BASIC_MESSAGE_H
#define HITTOP_HTTP_BASIC_MESSAGE_H

namespace hittop {
namespace http {

template <
    typename Range,
    typename SubRange =
        boost::iterator_range<decltype(std::cbegin(std::declval<Range>()))>,
    typename SubRangeSequence = std::vector<
        SubRange, ::short_alloc::short_alloc<SubRange, DEFAULT_ARENA_SIZE>>,
    typename SubRangeMap = std::unordered_map<
        SubRange, SubRange, std::hash<SubRange>, std::equal_to<SubRange>,
        ::short_alloc::short_alloc<std::pair<const SubRange, SubRange>,
                                   DEFAULT_ARENA_SIZE>>,
    typename InPlaceFactoryBuilder =
        internal::ArenaFactoryBuilder<DEFAULT_ARENA_SIZE>>
    >

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_BASIC_MESSAGE_H
