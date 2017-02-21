#ifndef HITTOP_CONTAINER_TRIE_H
#define HITTOP_CONTAINER_TRIE_H

#include <assert.h>

#include <array>
#include <cstdint>

namespace hittop {
namespace container {

template <typename T, typename Allocator> class Trie;

template <typename Allocator> class Trie<char, Allocator> {

private:
  using Bitset = std::array<std::uint64_t, 4>;

  template <typename T> static constexpr std::size_t bit_size() {
    return (sizeof(T) * 8);
  }

  template <typename T> static constexpr std::size_t upper_bound() {
    return 1 << bit_size<T>();
  }

  static std::size_t popcount(Bitset::value_type bits) {
    return __builtin_popcountll(bits);
  }

  struct Node {
    std::size_t GetIndex(std::size_t position) {
      assert(position < upper_bound<T>());
      constexpr auto mask0 = std::size_t{0x3} << 0;
      constexpr auto mask1 = std::size_t{0x3} << 4;
      constexpr auto mask2 = std::size_t{0x3} << 8;
      constexpr auto mask3 = std::size_t{0x3} << 12;

      if (position & mask3) {
        return popcount(present_[0]) + popcount(present_[1]) +
               popcount(present_[2]) +
               popcount(((1 << (position - (256 - 64 * 1))) - 1) & present_[3]);
      } else {
        if (position & mask2) {
          return popcount(present_[0]) + popcount(present_[1]) +
                 popcount(((1 << (position - (256 - 64 * 2))) - 1) &
                          present_[2]);
        } else {
          if (position & mask1) {
            return popcount(present_[0]) +
                   popcount(((1 << (position - (256 - 64 * 3))) - 1) &
                            present_[1]);
          } else {
            return popcount(((1 << (position - (256 - 64 * 4))) - 1) &
                            present_[0]);
          }
        }
      }
    }

    Bitset present_;
    std::vector<Node> children_;
  };

  Allocator alloc_;
  Node root_;
};

} // namespace container
} // namespace hittop

#endif // HITTOP_CONTAINER_TRIE_H
