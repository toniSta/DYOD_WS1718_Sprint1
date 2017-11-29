#pragma once

#include <memory>
#include <string>
#include <vector>

#include "resolve_type.hpp"
#include "storage/table.hpp"
#include "storage/chunk.hpp"
#include "storage/reference_column.hpp"

#include "table_scan.hpp"
#include "types.hpp"

namespace opossum {

class TableScan;

class BaseTableScanImpl {
 public:
  explicit BaseTableScanImpl() {}

  virtual const std::shared_ptr<const Table> on_execute() = 0;

 protected:
};

template<typename T>
class TableScanImpl : public BaseTableScanImpl {
 public:
  explicit TableScanImpl(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                         const AllTypeVariant search_value)
    : _in(in), _column_id(column_id), _scan_type(scan_type), _search_value(type_cast<T>(search_value)) {}

  bool compare(const T &lhs, const T &rhs) {
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

    std::cout << _in->get_output()->chunk_count() << std::endl;
    for (auto chunk_id = ChunkID{0}; chunk_id < _in->get_output()->chunk_count(); chunk_id++) {
      const auto base_column = _in->get_output()->get_chunk(chunk_id).get_column(_column_id);

      for (auto row_id = ChunkOffset{0}; row_id < base_column->size(); row_id++) {
        if (compare(type_cast<T>((*base_column)[row_id]), _search_value)) {
          pos_list->push_back(RowID{ChunkID{chunk_id}, row_id});
        }
      }
    }

    for (auto column_id = ColumnID{0}; column_id < _in->get_output()->col_count(); column_id++) {
      chunk.add_column(std::make_shared<ReferenceColumn>(_in->get_output(), column_id, pos_list));
      table->add_column_definition(_in->get_output()->column_name(column_id), _in->get_output()->column_type(column_id));
    }

    table->emplace_chunk(std::move(chunk));

    return table;
  }

 protected:
  const std::shared_ptr<const AbstractOperator> _in;
  ColumnID _column_id;
  const ScanType _scan_type;
  const T _search_value;
};

}  // namespace opossum
