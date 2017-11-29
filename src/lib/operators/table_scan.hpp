#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "table_scan_impl.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

class Table;

class BaseTableScanImpl;

class TableScan : public AbstractOperator {
 public:
  TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
            const AllTypeVariant search_value);

  ~TableScan();

  ColumnID column_id() const;

  ScanType scan_type() const;

  const AllTypeVariant& search_value() const;

 protected:
  std::shared_ptr<const Table> _on_execute() override;
  std::shared_ptr<BaseTableScanImpl> _impl;

  const std::shared_ptr<const AbstractOperator> _in;
  ColumnID _column_id;
  const ScanType _scan_type;
  const AllTypeVariant _search_value;
};

}  // namespace opossum
