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

void Chunk::add_column(std::shared_ptr<BaseColumn> column) { _chunk_columns.push_back(column); }

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == col_count(), "Wrong number of values.");

  for (auto column = ColumnID(0); column < col_count(); column++) {
    get_column(column)->append(values[column]);
  }
}

std::shared_ptr<BaseColumn> Chunk::get_column(ColumnID column_id) const { return _chunk_columns.at(column_id); }

uint16_t Chunk::col_count() const { return _chunk_columns.size(); }

uint32_t Chunk::size() const { return _chunk_columns.empty() ? 0 : get_column(ColumnID(0))->size(); }

}  // namespace opossum
