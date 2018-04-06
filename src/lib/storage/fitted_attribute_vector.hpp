#pragma once

#include <vector>

#include "base_attribute_vector.hpp"
#include "types.hpp"

namespace opossum {

template <typename T>
class FittedAttributeVector : public BaseAttributeVector {
 public:
  explicit FittedAttributeVector(uint32_t attribute_vector_size) { _attribute_vector.reserve(attribute_vector_size); }

  // returns ValueID from attribute vector for a certain position
  ValueID get(const size_t i) const override;

  // inserts a value_id at a given position
  void set(const size_t i, const ValueID value_id) override;

  // returns the number of values
  size_t size() const override;

  // returns the width of the values in bytes
  AttributeVectorWidth width() const override;

 protected:
  std::vector<T> _attribute_vector;
};

}  // namespace opossum
