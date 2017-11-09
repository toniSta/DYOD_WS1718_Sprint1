#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "all_type_variant.hpp"
#include "fitted_attribute_vector.hpp"
#include "type_cast.hpp"
#include "types.hpp"

namespace opossum {

class BaseAttributeVector;
class BaseColumn;

// Even though ValueIDs do not have to use the full width of ValueID (uint32_t), this will also work for smaller ValueID
// types (uint8_t, uint16_t) since after a down-cast INVALID_VALUE_ID will look like their numeric_limit::max()
constexpr ValueID INVALID_VALUE_ID{std::numeric_limits<ValueID::base_type>::max()};

// Dictionary is a specific column type that stores all its values in a vector
template <typename T>
class DictionaryColumn : public BaseColumn {
 public:
  /**
   * Creates a Dictionary column from a given value column.
   */
  explicit DictionaryColumn(const std::shared_ptr<BaseColumn>& base_column) { this->_compress(base_column); }

  // return the value at a certain position. If you want to write efficient operators, back off!
  const AllTypeVariant operator[](const size_t i) const override { return 42; }

  // return the value at a certain position.
  const T get(const size_t i) { return 1; }

  // dictionary columns are immutable
  void append(const AllTypeVariant&) override { throw std::runtime_error("Dictionary columns are immutable."); }

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() { return _dictionary; }

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() { return _attribute_vector; }

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) { return _dictionary->at(value_id); }

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) {
    auto dict_iterator = std::lower_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (dict_iterator == _dictionary->cend())
      return INVALID_VALUE_ID;
    else
      return ValueID(std::distance(_dictionary->cbegin(), dict_iterator));
  }

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) { return ValueID(lower_bound(type_cast<T>(value))); }

  // returns the first value ID that refers to a value > the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) {
    auto dict_iterator = std::upper_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (dict_iterator == _dictionary->cend())
      return INVALID_VALUE_ID;
    else
      return ValueID(std::distance(_dictionary->cbegin(), dict_iterator));
  }

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) { return ValueID(upper_bound(type_cast<T>(value))); }

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const { return _dictionary->size(); }

  // return the number of entries
  size_t size() const override { return _attribute_vector->size(); }

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  std::shared_ptr<BaseAttributeVector> _attribute_vector;

  void _compress(const std::shared_ptr<BaseColumn>& base_column) {
    const auto value_column = std::dynamic_pointer_cast<ValueColumn<T>>(base_column);
    const auto& values = value_column->values();

    _dictionary = std::make_shared<std::vector<T>>(values);

    // sort and make unique
    std::sort(_dictionary->begin(), _dictionary->end());
    _dictionary->erase(std::unique(_dictionary->begin(), _dictionary->end()), _dictionary->end());

    this->_create_attribute_vector(value_column->size());
    for (size_t value = 0; value < values.size(); ++value) {
      _attribute_vector->set(value, this->lower_bound(values.at(value)));
    }
  }

  void _create_attribute_vector(const size_t column_size) {
    const uint8_t required_bits = std::ceil(std::log2(this->unique_values_count()));
    if (required_bits <= 8) {
      _attribute_vector = std::make_shared<FittedAttributeVector<uint8_t>>(column_size);
    } else if (required_bits <= 16) {
      _attribute_vector = std::make_shared<FittedAttributeVector<uint16_t>>(column_size);
    } else if (required_bits <= 32) {
      _attribute_vector = std::make_shared<FittedAttributeVector<uint32_t>>(column_size);
    }
  }
};

}  // namespace opossum
