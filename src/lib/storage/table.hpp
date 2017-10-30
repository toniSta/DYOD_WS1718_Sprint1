#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_column.hpp"
#include "chunk.hpp"

#include "type_cast.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

class TableStatistics;

// A table is partitioned horizontally into a number of chunks
class Table : private Noncopyable {
 public:
  // creates a table
  // the parameter specifies the maximum chunk size, i.e., partition size
  // default (0) is an unlimited size. A table holds always at least one chunk
  explicit Table(const uint32_t chunk_size = 0) { _max_chunk_size = chunk_size; }
  // TODO(marcel): refactor

  // we need to explicitly set the move constructor to default when
  // we overwrite the copy constructor
  Table(Table&&) = default;
  Table& operator=(Table&&) = default;

  // returns the number of columns (cannot exceed ColumnID (uint16_t))
  uint16_t col_count() const;

  // Returns the number of rows.
  // This number includes invalidated (deleted) rows.
  // Use approx_valid_row_count() for an approximate count of valid rows instead.
  uint64_t row_count() const;

  // returns the number of chunks (cannot exceed ChunkID (uint32_t))
  ChunkID chunk_count() const;

  // returns the chunk with the given id
  Chunk& get_chunk(ChunkID chunk_id);
  const Chunk& get_chunk(ChunkID chunk_id) const;

  // Returns a list of all column names.
  const std::vector<std::string>& column_names() const;

  // returns the column name of the nth column
  const std::string& column_name(ColumnID column_id) const;

  // returns the column type of the nth column
  const std::string& column_type(ColumnID column_id) const;

  // Returns the column with the given name.
  // This method is intended for debugging purposes only.
  // It does not verify whether a column name is unambiguous.
  ColumnID column_id_by_name(const std::string& column_name) const;

  // return the maximum chunk size (cannot exceed ChunkOffset (uint32_t))
  uint32_t chunk_size() const;

  // adds column definition without creating the actual columns
  // this is helpful when, e.g., an operator first creates the structure of the table
  // and then adds chunk by chunk
  void add_column_definition(const std::string& name, const std::string& type);

  // adds a column to the end, i.e., right, of the table
  // the added column should have the same length as existing columns (if any)
  void add_column(const std::string& name, const std::string& type);

  // inserts a row at the end of the table
  // note this is slow and not thread-safe and should be used for testing purposes only
  void append(std::vector<AllTypeVariant> values);

  // creates a new chunk and appends it
  void create_new_chunk();

 protected:
  std::vector<std::string> _column_names_vector;
  std::vector<std::string> _column_types_vector;
  std::vector<Chunk> _table_chunks;
  uint32_t _max_chunk_size;
};
}  // namespace opossum
