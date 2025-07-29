#pragma once

namespace algo {
template <typename Tv, typename Tw, typename Serializer_t>
class serialized_array {
  std::vector<Tw> data;
  Serializer_t serializer;

public:
  serialized_array(std::size_t n, Serializer_t s)
      : data(n), serializer(std::move(s)) {}

  Tw &operator[](const Tv &key) { return data[serializer(key)]; }

  const Tw &at(const Tv &key) const { return data.at(serializer(key)); }

  void fill(const Tw &val) { std::fill(data.begin(), data.end(), val); }

  void resize(std::size_t n) { data.resize(n); }

  std::size_t size() const { return data.size(); }

  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
};

} // namespace algo