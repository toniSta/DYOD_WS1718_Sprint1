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
    _tables.insert(std::make_pair(name, table));
  }
}

void StorageManager::drop_table(const std::string& name) {
  // std::map::erase() returns 0 if nothing was erased
  if (_tables.erase(name) == 0) {
    throw std::runtime_error("Table name not found.");
  }
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  auto search = _tables.find(name);
  if (search != _tables.end()) {
    return search->second;
  } else {
    throw std::runtime_error("Table name not found.");
  }
}

bool StorageManager::has_table(const std::string& name) const {
  return (_tables.find(name) != _tables.end()) ? true : false;
}

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> _names;
  for (auto _table : _tables) {
    _names.push_back(_table.first);
  }
  return _names;
}

void StorageManager::print(std::ostream& out) const {
  for (auto _table : _tables) {
    out << "===========================================" << std::endl;
    out << "\tTable name: " << _table.first << std::endl;
    out << "\t\t#Columns: " << _table.second->col_count() << std::endl;
    out << "\t\t#Rows: " << _table.second->row_count() << std::endl;
    out << "\t\t#Chunks: " << _table.second->chunk_count() << std::endl;
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
