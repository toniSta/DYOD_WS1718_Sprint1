#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_column.hpp"
#include "dictionary_column.hpp"
#include "table.hpp"
#include "types.hpp"
#include "utils/assert.hpp"
#include "value_column.hpp"

namespace opossum {

// ReferenceColumn is a specific column type that stores all its values as position list of a referenced column
class ReferenceColumn : public BaseColumn {
 public:
  // creates a reference column
  // the parameters specify the positions and the referenced column
  ReferenceColumn(const std::shared_ptr<const Table> referenced_table, const ColumnID referenced_column_id,
                  const std::shared_ptr<const PosList> pos)
      : _referenced_table(referenced_table), _referenced_column_id(referenced_column_id), _pos(pos) {}

  const AllTypeVariant operator[](const size_t i) const override {
    const auto row_id = _pos->at(i);
    return (*_referenced_table->get_chunk(row_id.chunk_id).get_column(_referenced_column_id))[row_id.chunk_offset];
  };

  void append(const AllTypeVariant&) override { throw std::logic_error("ReferenceColumn is immutable"); }

  size_t size() const override { return _pos->size(); }

  const std::shared_ptr<const PosList> pos_list() const { return _pos; }
  const std::shared_ptr<const Table> referenced_table() const { return _referenced_table; }

  ColumnID referenced_column_id() const;

 protected:
  const std::shared_ptr<const Table> _referenced_table;
  const ColumnID _referenced_column_id;
  const std::shared_ptr<const PosList> _pos;
};

}  // namespace opossum
