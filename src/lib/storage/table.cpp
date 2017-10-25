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

  if (this->chunk_count() == 0) {
    this->create_new_chunk();
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  if (this->chunk_size() > table_chunks.back().size()) {
    std::cout << "1231231";
  }
  // Implementation goes here
}

void Table::create_new_chunk() {
  // Implementation goes here
}

uint16_t Table::col_count() const {
  // Implementation goes here
  return 0;
}

uint64_t Table::row_count() const {
  // Implementation goes here
  return 0;
}

ChunkID Table::chunk_count() const {
  return ChunkID{table_chunks.size()};
}

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  // Implementation goes here
  return ColumnID{0};
}

uint32_t Table::chunk_size() const {
  return max_chunk_size;
}

const std::vector<std::string>& Table::column_names() const {
  throw std::runtime_error("Implement Table::column_names()");
}

const std::string& Table::column_name(ColumnID column_id) const {
  throw std::runtime_error("Implement Table::column_name");
}

const std::string& Table::column_type(ColumnID column_id) const {
  throw std::runtime_error("Implement Table::column_type");
}

Chunk& Table::get_chunk(ChunkID chunk_id) { throw std::runtime_error("Implement Table::get_chunk"); }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { throw std::runtime_error("Implement Table::get_chunk"); }

}  // namespace opossum
