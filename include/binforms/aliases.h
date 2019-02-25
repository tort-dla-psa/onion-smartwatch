#ifndef ALIASES_H
#define ALIASES_H
#include <vector>
#include <memory>

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using vec = std::vector<T>;

template <typename T>
using vec_s = vec<sptr<T>>;

#endif // ALIASES_H
