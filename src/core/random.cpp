#include "chess/core/random.h"

#include <random>

namespace {

thread_local auto rng = std::mt19937_64{std::random_device{}()};

template <typename T>
inline T GenerateUniformInteger(T min, T max) {
  auto distribution = std::uniform_int_distribution<T>{min, max};
  return distribution(rng);
}

} // namespace

namespace chess {

U32 RandomU32(U32 min, U32 max) {
  return GenerateUniformInteger<U32>(min, max);
}

U64 RandomU64(U64 min, U64 max) {
  return GenerateUniformInteger<U64>(min, max);
}

} // namespace chess