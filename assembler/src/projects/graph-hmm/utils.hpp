#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_set>


template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p);
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v);
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &v);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << "[ ";
    for (const auto &e : v) {
        os << e << " ";
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &v) {
    os << "{ ";
    for (const auto &e : v) {
        os << e << " ";
    }
    os << "}";
    return os;
}

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p) {
    return os << "(" << p.first << "," << p.second << ")";
}

template<typename T>
void remove_duplicates(std::vector <T> &v) {
  // Remove duplicated items
  std::sort(v.begin(), v.end());
  auto it = std::unique_copy(v.cbegin(), v.cend(), v.begin());
  v.resize(std::distance(v.begin(), it));
}

bool any_in(const auto &v, const auto &set) {
  for (const auto &e : v) {
    if (set.count(e)) {
      return true;
    }
  }
  return false;
}

template <class T>
class ObjectCounter {
 public:
  static size_t object_count_max() {
    return count_max_;
  }

  static size_t object_count_current() {
    return count_current_;
  }

  template <typename... Args>
  ObjectCounter(Args&&...) noexcept {
    ++count_current_;
    count_max_ = std::max(count_max_, count_current_);
  }

  ~ObjectCounter() {
    --count_current_;
  }
 private:
  static size_t count_max_;
  static size_t count_current_;
};

template <class T>
size_t ObjectCounter<T>::count_max_ = 0;

template <class T>
size_t ObjectCounter<T>::count_current_ = 0;