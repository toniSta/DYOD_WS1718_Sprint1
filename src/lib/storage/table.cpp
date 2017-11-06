#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "value_column.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

Table::Table(const uint32_t chunk_size) : _max_chunk_size(chunk_size) { this->create_new_chunk(); }

void Table::add_column_definition(const std::string& name, const std::string& type) {
  _column_names_vector.push_back(name);
  _column_types_vector.push_back(type);
}

void Table::add_column(const std::string& name, const std::string& type) {
  if (_table_chunks.front().size() > 0) {
    throw std::runtime_error("Add column only works for empty tables.");
  }
  this->add_column_definition(name, type);
  _table_chunks.front().add_column(make_shared_by_column_type<BaseColumn, ValueColumn>(type));
}

void Table::append(std::vector<AllTypeVariant> values) {
  // Create a new chunk if chunk size is not unlimited and the current chunk is full.
  if (this->chunk_size() != 0 && this->chunk_size() == _table_chunks.back().size()) {
    this->create_new_chunk();
  }
  _table_chunks.back().append(values);
}

void Table::create_new_chunk() {
  Chunk _chunk;
  for (auto&& type : _column_types_vector) {
    _chunk.add_column(make_shared_by_column_type<BaseColumn, ValueColumn>(type));
  }
  _table_chunks.push_back(std::move(_chunk));
}

uint16_t Table::col_count() const { return _table_chunks.front().col_count(); }

uint64_t Table::row_count() const {
  return (_table_chunks.size() - 1) * this->chunk_size() + _table_chunks.back().size();
}

ChunkID Table::chunk_count() const { return static_cast<ChunkID>(_table_chunks.size()); }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  auto search = std::find(_column_names_vector.begin(), _column_names_vector.end(), column_name);
  if (search != _column_names_vector.end()) {
    return ColumnID{std::distance(_column_names_vector.begin(), search)};
  } else {
    throw std::runtime_error("Column name not found.");
  }
}

uint32_t Table::chunk_size() const { return _max_chunk_size; }

const std::vector<std::string>& Table::column_names() const { return _column_names_vector; }

const std::string& Table::column_name(ColumnID column_id) const { return _column_names_vector.at(column_id); }

const std::string& Table::column_type(ColumnID column_id) const { return _column_types_vector.at(column_id); }

Chunk& Table::get_chunk(ChunkID chunk_id) { return _table_chunks.at(chunk_id); }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return _table_chunks.at(chunk_id); }

}  // namespace opossum
