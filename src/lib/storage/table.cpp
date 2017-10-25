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

void Table::add_column_definition(const std::string& name, const std::string& type) {
  column_names_vector.push_back(name);
  column_types_vector.push_back(type);
}

void Table::add_column(const std::string& name, const std::string& type) {
  this->add_column_definition(name, type);

  if (this->chunk_size() > 0 && this->chunk_count() == 0) {
    this->create_new_chunk();
  } else {
    for (auto& chunk : table_chunks) {
      chunk.add_column(make_shared_by_column_type<BaseColumn, ValueColumn>(type));
    }
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  if (this->chunk_size() == table_chunks.back().size()) {
    this->create_new_chunk();
  }
  table_chunks.back().append(values);
}

void Table::create_new_chunk() {
  table_chunks.push_back(Chunk{});

  for (auto& type : column_types_vector) {
    table_chunks.back().add_column(make_shared_by_column_type<BaseColumn, ValueColumn>(type));
  }
}

uint16_t Table::col_count() const { return column_names_vector.size(); }

uint64_t Table::row_count() const {
  if (table_chunks.empty()) {
    return 0;
  } else {
    return (table_chunks.size() - 1) * this->chunk_count() + table_chunks.back().size();
  }
}

ChunkID Table::chunk_count() const { return ChunkID{table_chunks.size()}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  for (uint16_t column_id = 0; column_id < column_names_vector.size(); ++column_id) {
    if (column_names_vector.at(column_id) == column_name) {
      return ColumnID{column_id};
    }
  }
  throw std::runtime_error("Column name not found.");
}

uint32_t Table::chunk_size() const { return max_chunk_size; }

const std::vector<std::string>& Table::column_names() const { return column_names_vector; }

const std::string& Table::column_name(ColumnID column_id) const { return column_names_vector.at(column_id); }

const std::string& Table::column_type(ColumnID column_id) const { return column_types_vector.at(column_id); }

Chunk& Table::get_chunk(ChunkID chunk_id) { return table_chunks.at(chunk_id); }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return table_chunks.at(chunk_id); }

}  // namespace opossum
