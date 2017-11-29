#include "table_scan.hpp"

#include <memory>
#include <resolve_type.hpp>
#include <string>
#include <vector>

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "storage/table.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : _in(in), _column_id(column_id), _scan_type(scan_type), _search_value(search_value) {}

TableScan::~TableScan() {}

ColumnID TableScan::column_id() const { return _column_id; }

ScanType TableScan::scan_type() const { return _scan_type; }

const AllTypeVariant& TableScan::search_value() const { return _search_value; }

std::shared_ptr<const Table> TableScan::_on_execute() {
  auto input_table = _in->get_output();
  DebugAssert(input_table, "No input table given");
  const auto& column_type = input_table->column_type(_column_id);
  _impl = make_unique_by_column_type<BaseTableScanImpl, TableScanImpl>(column_type, _in, _column_id, _scan_type,
                                                                       _search_value);

  return _impl->execute();
}

}  // namespace opossum
