#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_column.hpp"
#include "chunk.hpp"

#include "utils/assert.hpp"

namespace opossum {

void Chunk::add_column(std::shared_ptr<BaseColumn> column) {
  // Implementation goes here
}

void Chunk::append(std::vector<AllTypeVariant> values) {
  // Implementation goes here
}

std::shared_ptr<BaseColumn> Chunk::get_column(ColumnID column_id) const {
  // Implementation goes here
  return nullptr;
}

uint16_t Chunk::col_count() const {
  // Implementation goes here
  return 0;
}

uint32_t Chunk::size() const {
  // Implementation goes here
  return 0;
}

}  // namespace opossum
