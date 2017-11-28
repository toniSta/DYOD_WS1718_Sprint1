#include "table_scan.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : _in(in), _column_id(column_id), _scan_type(scan_type), _search_value(search_value) {
  //  _impl = make_unique_by_column_type();
  std::cout << "TableScan.cpp constructer called" << std::endl;
}

TableScan::~TableScan() {}

ColumnID TableScan::column_id() const { return _column_id; }

ScanType TableScan::scan_type() const { return _scan_type; }

const AllTypeVariant& TableScan::search_value() const { return _search_value; }

std::shared_ptr<const Table> TableScan::_on_execute() {
  return _in->get_output();
  //  return _impl.on_execute();
}

}  // namespace opossum
