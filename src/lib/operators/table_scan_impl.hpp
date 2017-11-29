#pragma once

#include <memory>
#include <string>
#include <vector>

#include "resolve_type.hpp"
#include "storage/table.hpp"

#include "table_scan.hpp"
#include "types.hpp"

namespace opossum {

class TableScan;

class BaseTableScanImpl {
 public:
  explicit BaseTableScanImpl() {}

  virtual const std::shared_ptr<const Table> execute() = 0;

 protected:
};

template <typename T>
class TableScanImpl : public BaseTableScanImpl {
 public:
  explicit TableScanImpl(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                         const AllTypeVariant search_value)
      : _in(in), _column_id{column_id}, _scan_type{scan_type}, _search_value{type_cast<T>(search_value)} {}

  const std::shared_ptr<const Table> execute() override { return _in->get_output(); }

 protected:
  const std::shared_ptr<const AbstractOperator> _in;
  ColumnID _column_id;
  const ScanType _scan_type;
  const T _search_value;
};

}  // namespace opossum
