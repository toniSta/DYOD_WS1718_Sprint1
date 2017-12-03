#pragma once

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "resolve_type.hpp"
#include "storage/chunk.hpp"
#include "storage/dictionary_column.hpp"
#include "storage/reference_column.hpp"
#include "storage/table.hpp"
#include "storage/value_column.hpp"

#include "table_scan.hpp"
#include "types.hpp"

namespace opossum {

class BaseTableScanImpl {
 public:
  BaseTableScanImpl() {}

  virtual const std::shared_ptr<const Table> on_execute() = 0;
};

template <typename T>
class TableScanImpl : public BaseTableScanImpl {
 public:
  explicit TableScanImpl(const std::shared_ptr<const Table> input_table, ColumnID column_id, const ScanType scan_type,
                         const AllTypeVariant search_value)
      : _input_table(input_table),
        _column_id(column_id),
        _scan_type(scan_type),
        _search_value(type_cast<T>(search_value)) {}

  const std::shared_ptr<const Table> on_execute() override {
    _pos_list = std::make_shared<PosList>(std::initializer_list<RowID>({}));
    auto table = std::make_shared<Table>(0);
    auto new_referenced_table = _input_table;
    Chunk chunk;

    // Iterate over all chunks
    for (auto chunk_id = ChunkID{0}; chunk_id < _input_table->chunk_count(); chunk_id++) {
      const auto base_column = _input_table->get_chunk(chunk_id).get_column(_column_id);

      // Cast the specified column
      if (const auto column = std::dynamic_pointer_cast<ValueColumn<T>>(base_column)) {
        _scan_value_column(column, chunk_id);
      } else if (const auto column = std::dynamic_pointer_cast<DictionaryColumn<T>>(base_column)) {
        _scan_dictionary_column(column, chunk_id);
      } else if (const auto column = std::dynamic_pointer_cast<ReferenceColumn>(base_column)) {
        // use the old referenced table for the current table scan result
        new_referenced_table = column->referenced_table();
        _scan_reference_column(column, chunk_id);
      }
    }

    // Create new table for output, based on the input table's layout
    for (auto column_id = ColumnID{0}; column_id < _input_table->col_count(); column_id++) {
      chunk.add_column(std::make_shared<ReferenceColumn>(new_referenced_table, column_id, _pos_list));
      table->add_column_definition(_input_table->column_name(column_id), _input_table->column_type(column_id));
    }

    table->emplace_chunk(std::move(chunk));
    return table;
  }

 protected:
  std::shared_ptr<PosList> _pos_list;
  const std::shared_ptr<const Table> _input_table;
  const ColumnID _column_id;
  const ScanType _scan_type;
  const T _search_value;

  // Compare values with the specified operator
  bool _compare(const T& lhs, const T& rhs) {
    switch (_scan_type) {
      case ScanType::OpEquals:
        return lhs == rhs;
      case ScanType::OpNotEquals:
        return lhs != rhs;
      case ScanType::OpLessThan:
        return lhs < rhs;
      case ScanType::OpLessThanEquals:
        return lhs <= rhs;
      case ScanType::OpGreaterThan:
        return lhs > rhs;
      case ScanType::OpGreaterThanEquals:
        return lhs >= rhs;
      default:
        throw std::runtime_error("Unknown Operator");
    }
  }

  // Compare value ids of dictionary columns
  bool _compare_value_ids(const ValueID lhs, const ValueID rhs, ScanType scan_type) {
    switch (scan_type) {
      case ScanType::OpEquals:
        return lhs == rhs;
      case ScanType::OpNotEquals:
        return lhs != rhs;
      case ScanType::OpLessThan:
        return lhs < rhs;
      case ScanType::OpGreaterThanEquals:
        return lhs >= rhs;
      default:
        throw std::runtime_error("Unknown Operator");
    }
  }

  // Scan value column by iterating over its values
  void _scan_value_column(std::shared_ptr<ValueColumn<T>> value_column, ChunkID chunk_id) {
    const auto begin = value_column->values().cbegin();
    const auto end = value_column->values().cend();
    for (auto it = begin; it < end; ++it) {
      if (_compare(*it, _search_value)) {
        _pos_list->push_back(RowID{ChunkID{chunk_id}, ChunkOffset(std::distance(begin, it))});
      }
    }
  }

  // In some cases, the whole chunk has to be returned
  void _add_all_rows_to_pos_list(const std::shared_ptr<const BaseAttributeVector> attribute_vector, ChunkID chunk_id) {
    _pos_list->reserve(attribute_vector->size());
    for (auto counter = ChunkOffset(0); counter < attribute_vector->size(); counter++) {
      _pos_list->push_back(RowID{ChunkID{chunk_id}, counter});
    }
  }

  // Use dictionary column to improve scan speed
  void _scan_dictionary_column(std::shared_ptr<DictionaryColumn<T>> dictionary_column, ChunkID chunk_id) {
    ValueID bound;
    ScanType attribute_vector_scan_type;
    const auto attribute_vector = dictionary_column->attribute_vector();

    // Distinguish between the different operators
    switch (_scan_type) {
      case ScanType::OpLessThan:
        bound = dictionary_column->lower_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpLessThan;
        break;
      case ScanType::OpLessThanEquals:
        bound = dictionary_column->upper_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpLessThan;
        break;
      case ScanType::OpEquals:
        bound = dictionary_column->lower_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpEquals;
        // Add nothing to _pos_list if search value was not found in the dictionary
        if (bound == INVALID_VALUE_ID || dictionary_column->get(bound) != _search_value) {
          return;
        }
        break;
      case ScanType::OpNotEquals:
        bound = dictionary_column->lower_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpNotEquals;
        // Add all rows to _pos_list if search value was not found in the dictionary
        if (bound == INVALID_VALUE_ID || dictionary_column->get(bound) != _search_value) {
          _add_all_rows_to_pos_list(attribute_vector, chunk_id);
          return;
        }
        break;
      case ScanType::OpGreaterThan:
        bound = dictionary_column->upper_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpGreaterThanEquals;
        break;
      case ScanType::OpGreaterThanEquals:
        bound = dictionary_column->lower_bound(_search_value);
        attribute_vector_scan_type = ScanType::OpGreaterThanEquals;
        break;
      default:
        throw std::runtime_error("Unknown Operator");
    }

    if (bound == INVALID_VALUE_ID) {
      // If a value is larger the every other in the dictionary column, it will neither be found by lower_bound
      // nor by upper_bound. In that case, add the whole chunk.
      if (_scan_type == ScanType::OpLessThan || _scan_type == ScanType::OpLessThanEquals) {
        _add_all_rows_to_pos_list(attribute_vector, chunk_id);
      }
      return;
    }

    // Add every relevant value to position list
    for (auto counter = ChunkOffset(0); counter < attribute_vector->size(); counter++) {
      if (_compare_value_ids(attribute_vector->get(counter), bound, attribute_vector_scan_type)) {
        _pos_list->push_back(RowID{ChunkID{chunk_id}, counter});
      }
    }
  }

  // Scan reference column by looking up the values, since we do not want to
  // have reference columns referencing on other reference columns
  void _scan_reference_column(std::shared_ptr<ReferenceColumn> reference_column, ChunkID chunk_id) {
    bool add_to_pos_list;
    for (const auto pos : *reference_column->pos_list()) {
      // getting the referenced column
      auto base_column = reference_column->referenced_table()
                             ->get_chunk(pos.chunk_id)
                             .get_column(reference_column->referenced_column_id());
      if (auto column = std::dynamic_pointer_cast<ValueColumn<T>>(base_column)) {
        add_to_pos_list = _compare(column->values()[pos.chunk_offset], _search_value);
      } else if (auto column = std::dynamic_pointer_cast<DictionaryColumn<T>>(base_column)) {
        add_to_pos_list = _compare(column->get(pos.chunk_offset), _search_value);
      }
      if (add_to_pos_list) {
        _pos_list->push_back(RowID{pos.chunk_id, pos.chunk_offset});
      }
    }
  }
};

}  // namespace opossum
