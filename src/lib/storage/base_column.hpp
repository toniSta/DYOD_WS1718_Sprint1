#pragma once

#include <memory>
#include <string>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

// BaseColumn is the abstract super class for all column types,
// e.g., ValueColumn, ReferenceColumn
class BaseColumn : private Noncopyable {
 public:
  BaseColumn() = default;
  virtual ~BaseColumn() = default;

  // we need to explicitly set the move constructor to default when
  // we overwrite the copy constructor
  BaseColumn(BaseColumn&&) = default;
  BaseColumn& operator=(BaseColumn&&) = default;

  // returns the value at a given position
  virtual const AllTypeVariant operator[](const size_t i) const = 0;

  // appends the value at the end of the column
  virtual void append(const AllTypeVariant& val) = 0;

  // returns the number of values
  virtual size_t size() const = 0;
};
}  // namespace opossum
