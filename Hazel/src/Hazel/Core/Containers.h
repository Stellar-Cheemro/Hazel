#pragma once

#include <unordered_map>
#include <unordered_set>

namespace Hazel
{

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
using HashMap = std::unordered_map<Key, Value, Hash, KeyEqual>;

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
using HashSet = std::unordered_set<Key, Hash, KeyEqual>;
template <typename T> using Vector = std::vector<T>;
} // namespace Hazel