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

void Chunk::append(std::vector<AllTypeVariant> values) {
  DebugAssert(values.size() == this->col_count(), "Wrong number of values.");

  for (std::vector<AllTypeVariant>::iterator it = values.begin() ; it != values.end(); ++it) {
  	this->get_column(ColumnID{std::distance(values.begin(), it)})->append(*it);
  }
}

std::shared_ptr<BaseColumn> Chunk::get_column(ColumnID column_id) const { return _chunk_columns.at(column_id); }

uint16_t Chunk::col_count() const { return _chunk_columns.size(); }

uint32_t Chunk::size() const { return _chunk_columns.empty() ? 0 : this->get_column(ColumnID{0})->size(); }

}  // namespace opossum
