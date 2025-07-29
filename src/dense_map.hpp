#pragma once

#include <optional>
#include <utility>
#include <vector>

namespace algo {
template <typename Tv, typename Tw, typename Serializer_t> class dense_map {
private:
  std::vector<Tv> keys;
  std::vector<std::optional<Tw>> data;
  Serializer_t serializer;

public:
  dense_map(std::size_t n, Serializer_t s)
      : data(n), serializer(std::move(s)) {}

  Tw &operator[](const Tv &key) {
    int i = serializer(key);
    if (!data[i].has_value()) {
      keys.push_back(key);
      data[i] = Tw{};
    }
    return *data[i];
  }

  const Tw &at(const Tv &key) const { return *data.at(serializer(key)); }

  bool contains(const Tv &key) const {
    int i = serializer(key);
    return 0 <= i && i < data.size() && data[i].has_value();
  }

  void fill(const Tw &val) {
    for (auto &opt : data) {
      opt = val;
    }
  }

  struct kv_ref {
    const Tv &key;
    Tw &value;
  };

  struct iterator {
    using it_v = typename std::vector<Tv>::iterator;
    it_v it;
    dense_map *par;

    iterator(it_v it, dense_map *par) : it(it), par(par) {}
    iterator &operator++() {
      ++it;
      return *this;
    }
    bool operator!=(const iterator &other) const { return it != other.it; }
    kv_ref operator*() const {
      Tv &key = *it;
      return {key, (*par)[key]};
    }
  };

  iterator begin() { return {keys.begin(), this}; }
  iterator end() { return {keys.end(), this}; }
};
} // namespace algo