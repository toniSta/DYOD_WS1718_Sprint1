#include "value_column.hpp"

#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "type_cast.hpp"
#include "utils/assert.hpp"
#include "utils/performance_warning.hpp"

namespace opossum {

template <typename T>
const AllTypeVariant ValueColumn<T>::operator[](const size_t i) const {
  PerformanceWarning("operator[] used");

  throw std::runtime_error("Implement ValueColumn::operator[]");
}

template <typename T>
void ValueColumn<T>::append(const AllTypeVariant& val) {
  // Implementation goes here
}

template <typename T>
size_t ValueColumn<T>::size() const {
  // Implementation goes here
  return 0;
}

EXPLICITLY_INSTANTIATE_COLUMN_TYPES(ValueColumn);

}  // namespace opossum
