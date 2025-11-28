#pragma once

#include <numeric>
#include <optional>
#include <type_traits>
#include <vector>

namespace algo {
namespace internal {
template <typename T>
struct functor_traits : functor_traits<decltype(&T::operator())> {};

template <typename C, typename _R, typename _A>
struct functor_traits<_R (C::*)(_A) const> {
  using R = _R;
  using A = std::decay_t<_A>;
};
} // namespace internal
template <typename _T, typename Comp = std::less<typename internal::functor_traits<_T>::R>>
class sparse_lichao_tree {
private:
  using int_t = typename internal::functor_traits<_T>::A;
  struct T {
    _T f;
    bool active;
    T() : active(false) {}
    T(_T f) : f(f), active(true) {}
  };

  struct node {
    int_t L, R;
    node *l, *r;
    T f;
    bool active;
    node(int_t L, int_t R) : L(L), R(R), l(nullptr), r(nullptr), active(false) {}
  };

  bool comp(const T &a, const T &b, int_t m) const {
    return a.active && (!b.active || Comp{}(a.f(m), b.f(m)));
  }

  node *t;

  void destroy(node *t) {
    if (t) {
      delete t->l;
      delete t->r;
    }
    delete t;
  }

  void add(node *t, T f, int_t L, int_t R) {
    if (t->R < L || R < t->L) {
      return;
    }
    int_t m = std::midpoint(t->L, t->R);
    if (L <= t->L && t->R <= R) {
      if (comp(f, t->f, m)) {
        std::swap(f, t->f);
      }
      if (comp(f, t->f, t->L)) {
        add(t->l = t->l ? t->l : new node(t->L, m), f, L, R);
      }
      if (comp(f, t->f, t->R)) {
        add(t->r = t->r ? t->r : new node(m + 1, t->R), f, L, R);
      }
      return;
    }
    add(t->l = t->l ? t->l : new node(t->L, m), f, L, R);
    add(t->r = t->r ? t->r : new node(m + 1, t->R), f, L, R);
  }

  T query(node *t, int_t x) const {
    if (!t || t->R < x || x < t->L) {
      return T{};
    }
    T ans = t->f;
    if (T v = query(t->l, x); comp(v, ans, x)) {
      ans = v;
    }
    if (T v = query(t->r, x); comp(v, ans, x)) {
      ans = v;
    }
    return ans;
  }

public:
  sparse_lichao_tree(int_t n) : t(new node(0, n - 1)) {}
  ~sparse_lichao_tree() { destroy(t); }

  void add(int_t l, int_t r, const _T &x) { add(t, T{x}, l, r); }
  void add(const _T &x) { add(t, T{x}, t->L, t->R); }
  std::optional<_T> query(int_t x) const {
    T ans = query(t, x);
    if (ans.active) {
      return ans.f;
    }
    return std::nullopt;
  }
};

} // namespace algo
