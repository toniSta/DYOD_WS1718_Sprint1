#pragma once

#include <vector>

#include "base_attribute_vector.hpp"
#include "types.hpp"

namespace opossum {

template <typename T>
class FittedAttributeVector : public BaseAttributeVector {
 public:
  explicit FittedAttributeVector(uint32_t attribute_vector_size) { _attribute_vector.reserve(attribute_vector_size); }

  ~FittedAttributeVector() override{};

  ValueID get(const size_t i) const override { return ValueID(_attribute_vector.at(i)); }

  // inserts the value_id at a given position
  void set(const size_t i, const ValueID value_id) override {
    _attribute_vector.insert(_attribute_vector.begin() + i, value_id);
  };

  // returns the number of values
  size_t size() const override { return _attribute_vector.size(); }

  // returns the width of the values in bytes
  AttributeVectorWidth width() const override { return sizeof(T); }

 protected:
  std::vector<T> _attribute_vector;
};

}  // namespace opossum
