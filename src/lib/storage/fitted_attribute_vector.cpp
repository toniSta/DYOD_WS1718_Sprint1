#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "fitted_attribute_vector.hpp"

namespace opossum {

template <typename T>
ValueID FittedAttributeVector<T>::get(const size_t i) const {
  return ValueID(_attribute_vector.at(i));
}

template <typename T>
void FittedAttributeVector<T>::set(const size_t i, const ValueID value_id) {
  _attribute_vector.insert(_attribute_vector.begin() + i, value_id);
}

template <typename T>
size_t FittedAttributeVector<T>::size() const {
  return _attribute_vector.size();
}

template <typename T>
AttributeVectorWidth FittedAttributeVector<T>::width() const {
  return sizeof(T);
}

// Need this for instantiation
// Problem described here: https://stackoverflow.com/a/8752879
template class FittedAttributeVector<uint8_t>;
template class FittedAttributeVector<uint16_t>;
template class FittedAttributeVector<uint32_t>;

}  // namespace opossum
