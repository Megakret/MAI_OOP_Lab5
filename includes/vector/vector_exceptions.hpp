#pragma once

#include <stdexcept>

namespace vector {
class OutOfBounds : public std::range_error {
public:
  OutOfBounds()
      : std::range_error("you tried to access an element from an index outside "
                         "of the array") {}
};
} // namespace vector
