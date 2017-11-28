#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

class TableScanImpl {
 public:
  TableScanImpl();

  //  const std::shared_ptr<const PosList> on_execute();
};

}  // namespace opossum
