#include "table_scan_impl.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "resolve_type.hpp"
#include "storage/table.hpp"

#include "table_scan.hpp"
#include "types.hpp"

namespace opossum {

//template <typename T>
//TableScanImpl::TableScanImpl(const TableScan& table_scan)
//  : BaseTableScanImpl{table_scan},
//    _search_value{type_cast<T>(table_scan.search_value())},
//    _scan_type{table_scan.scan_type()} {}

//template <typename T>
//const std::shared_ptr<const Table> TableScanImpl<T>::execute() {
//  //  return _in->get_output();
//  return nullptr;
//}

}  // namespace opossum
