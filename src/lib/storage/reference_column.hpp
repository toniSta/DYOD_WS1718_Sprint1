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

  const AllTypeVariant operator[](const size_t i) const override;

  void append(const AllTypeVariant&) override;

  size_t size() const override;

  const std::shared_ptr<const PosList> pos_list() const;

  const std::shared_ptr<const Table> referenced_table() const;

  ColumnID referenced_column_id() const;

 protected:
  const std::shared_ptr<const Table> _referenced_table;
  const ColumnID _referenced_column_id;
  const std::shared_ptr<const PosList> _pos;
};

}  // namespace opossum
