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

void Chunk::add_column(std::shared_ptr<BaseColumn> column) { chunk_columns.push_back(column); }

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  // Implementation goes here
}

std::shared_ptr<BaseColumn> Chunk::get_column(ColumnID column_id) const { return chunk_columns.at(column_id); }

uint16_t Chunk::col_count() const { return chunk_columns.size(); }

uint32_t Chunk::size() const {
  if (chunk_columns.size() == 0) {
    return 0;
  } else {
    return this->get_column(ColumnID{0})->size();
  }
}

}  // namespace opossum
