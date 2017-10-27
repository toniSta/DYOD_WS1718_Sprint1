#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  static StorageManager instance;
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  if (has_table(name)) {
    throw std::runtime_error("Table already exists.");
  } else {
    _table_names.push_back(name);
    _table_pointers.push_back(table);
  }
}

void StorageManager::drop_table(const std::string& name) {
  for (uint16_t _table_index = 0; _table_index < _table_names.size(); ++_table_index) {
    if (_table_names.at(_table_index) == name) {
      _table_names.erase(_table_names.begin() + _table_index);
      _table_pointers.erase(_table_pointers.begin() + _table_index);
      return;
    }
  }
  throw std::runtime_error("Table name not found.");
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  for (uint16_t _table_index = 0; _table_index < _table_names.size(); ++_table_index) {
    if (_table_names.at(_table_index) == name) {
      return _table_pointers.at(_table_index);
    }
  }
  throw std::runtime_error("Table name not found.");
}

bool StorageManager::has_table(const std::string& name) const {
  return std::find(_table_names.begin(), _table_names.end(), name) != _table_names.end() ? true : false;
}

std::vector<std::string> StorageManager::table_names() const { return _table_names; }

void StorageManager::print(std::ostream& out) const {
  for (uint16_t _table_index = 0; _table_index < _table_names.size(); ++_table_index) {
    out << "===========================================" << std::endl;
    out << "\tTable name: " << _table_names.at(_table_index) << std::endl;
    out << "\t\t#Columns: " << _table_pointers.at(_table_index)->col_count() << std::endl;
    out << "\t\t#Rows: " << _table_pointers.at(_table_index)->row_count() << std::endl;
    out << "\t\t#Chunks: " << _table_pointers.at(_table_index)->chunk_count() << std::endl;
    out << "===========================================" << std::endl;
    out << std::endl;
  }
}

void StorageManager::reset() {
  // Got the idea from here: https://stackoverflow.com/questions/38796566/c-singleton-and-new-operator/38796665
  // General idea behind that: in order to override old sm, reuse previously allocated memory
  // (we get the address by calling get())
  get() = StorageManager();
}

}  // namespace opossum
