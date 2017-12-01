#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "resolve_type.hpp"
#include "storage/chunk.hpp"
#include "storage/reference_column.hpp"
#include "storage/table.hpp"

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

  bool compare(const T& lhs, const T& rhs) {
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

  const std::shared_ptr<const Table> on_execute() override {
    auto table = std::make_shared<Table>(0);
    auto pos_list = std::make_shared<PosList>(std::initializer_list<RowID>({}));
    Chunk chunk;

    for (auto chunk_id = ChunkID{0}; chunk_id < _input_table->chunk_count(); chunk_id++) {
      const auto base_column = _input_table->get_chunk(chunk_id).get_column(_column_id);

      for (auto row_id = ChunkOffset{0}; row_id < base_column->size(); row_id++) {
        if (compare(type_cast<T>((*base_column)[row_id]), _search_value)) {
          pos_list->push_back(RowID{ChunkID{chunk_id}, row_id});
        }
      }
    }

    for (auto column_id = ColumnID{0}; column_id < _input_table->col_count(); column_id++) {
      chunk.add_column(std::make_shared<ReferenceColumn>(_input_table, column_id, pos_list));
      table->add_column_definition(_input_table->column_name(column_id), _input_table->column_type(column_id));
    }

    table->emplace_chunk(std::move(chunk));

    return table;
  }

 protected:
  const std::shared_ptr<const Table> _input_table;
  const ColumnID _column_id;
  const ScanType _scan_type;
  const T _search_value;
};

}  // namespace opossum
