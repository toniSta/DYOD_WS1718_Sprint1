#include "reference_column.hpp"

#include <memory>
#include <string>
#include <vector>

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

const AllTypeVariant ReferenceColumn::operator[](const size_t i) const {
  const auto row_id = _pos->at(i);
  return (*_referenced_table->get_chunk(row_id.chunk_id).get_column(_referenced_column_id))[row_id.chunk_offset];
}

void ReferenceColumn::append(const AllTypeVariant&) { throw std::logic_error("ReferenceColumn is immutable"); }

size_t ReferenceColumn::size() const { return _pos->size(); }

const std::shared_ptr<const PosList> ReferenceColumn::pos_list() const { return _pos; }

const std::shared_ptr<const Table> ReferenceColumn::referenced_table() const { return _referenced_table; }

ColumnID ReferenceColumn::referenced_column_id() const { return _referenced_column_id; }

}  // namespace opossum
