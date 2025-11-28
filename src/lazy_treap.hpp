#pragma once

#include "internal_lazy_treap.hpp"
#include "lazy_op.hpp"
#include "proxy.hpp"

namespace algo {
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_add_set_treap {
private:
  using lazy_op = internal::lazy_add_set_op<F>;
  using combine = internal::lazy_add_set_combine<T, F, traits>;
  using reference = internal::proxy_ref<lazy_add_set_treap, T>;

  internal::lazy_treap<T, f, base, lazy_op, combine> treap;

  lazy_add_set_treap(const internal::lazy_treap<T, f, base, lazy_op, combine> &r) : treap(r) {}

public:
  lazy_add_set_treap() : treap() {}
  lazy_add_set_treap(const std::vector<T> &a) : treap(a) {}
  lazy_add_set_treap(std::size_t n, const T &x) : treap(n, x) {}
  lazy_add_set_treap(std::size_t n) : treap(n) {}

  /// @brief Inserts `x` at the `i`-th index in the treap.
  /// @param i The index to insert at. If anything's already at this index, it's
  /// moved ahead.
  /// @param x The element to insert.
  void insert(std::size_t i, const T &x) { treap.insert(i, x); }
  void push_back(const T &x) { insert(size(), x); }
  void push_front(const T &x) { insert(0, x); }

  /// @brief Inserts all the elements in `a` at the `i`-th index in the treap.
  /// @param x The index to insert at. If anything's already at this index, it's
  /// moved ahead.
  /// @param a The elements to insert.
  void insert(std::size_t i, const std::vector<T> &a) { treap.insert(i, a); }

  /// @brief Erases the `i`-th element.
  /// @param i The index whose element needs to be erased.
  void erase(std::size_t i) { treap.erase(i); }
  void pop_back() { erase(size() - 1); }
  void pop_front() { erase(0); }

  void add(std::size_t l, std::size_t r, F x) {
    treap.apply(l, r, {x, F{}, false});
  }
  void add(std::size_t i, F x) { add(i, i, x); }

  /// @brief Applies a lazy operation over a range.
  /// @param l The starting index.
  /// @param r The ending index.
  /// @param x The lazy operation to apply to all elements in [l, r].
  void apply(std::size_t l, std::size_t r, F x) { add(l, r, x); }
  void apply(std::size_t i, F x) { apply(i, i, x); }

  void set(std::size_t l, std::size_t r, F x) {
    treap.apply(l, r, {F{}, x, true});
  }
  void set(std::size_t i, F x) { set(i, i, x); }

  /// @brief Returns the accumulated value over the range [l, r].
  /// @param l The starting (inclusive) index of the range.
  /// @param r The ending (inclusive) index of the range.
  /// @return The aggregate of all elements in the range [l, r].
  T query(std::size_t l, std::size_t r) { return treap.query(l, r); }

  /// @brief Splits the treap at index `i`, keeping the left part in this treap.
  /// @param i The split index (0-based). Elements `[0, i]` remain here;
  /// elements `(i, end]` go to the returned treap.
  /// @return A new treap containing all elements after index `i`.
  lazy_add_set_treap split(std::size_t i) { return treap.split(i); }

  /// @brief Merges `other` into this treap.
  /// @param other Another treap. After the call, it becomes empty.
  void merge(lazy_add_set_treap &r) { treap.merge(r.treap); }
  void merge(lazy_add_set_treap &&r) { merge(r); }

  /// @brief Cuts out the range [l, r] from the treap and returns it. Note that
  /// the treap this is called on is modified.
  /// @param l The starting index of the range.
  /// @param r The ending index of the range.
  /// @return A treap containing the elements in [l, r].
  lazy_add_set_treap cut(std::size_t l, std::size_t r) { return treap.cut(l, r); }

  /// @brief Reverses the order of elements in the range [l, r], modifying the
  /// treap in place.
  /// @param l The starting index.
  /// @param r The ending index.
  void reverse(std::size_t l, std::size_t r) { treap.reverse(l, r); }

  /// @brief Returns the element at the `i`-th location.
  T at(std::size_t i) { return treap.at(i); }
  reference operator[](std::size_t i) { return reference(this, i); }
  reference front() { return reference(this, 0); }
  reference back() { return reference(this, size() - 1); }

  /// @brief Returns the current size of the treap.
  std::size_t size() const { return treap.size(); }
  bool empty() const { return treap.empty(); }

  using iterator = internal::iterator<reference, lazy_add_set_treap>;

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
};

template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
using lazy_treap = std::conditional_t<has_set_trait<traits, T, F>,
                                      lazy_add_set_treap<T, f, base, F, traits>,
                                      internal::lazy_treap<T, f, base, F, traits>>;
}; // namespace algo