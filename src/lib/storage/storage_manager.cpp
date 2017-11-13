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
  auto result = _tables.insert({name, table});
  if (!result.second) throw std::runtime_error("Table already exists.");
}

void StorageManager::drop_table(const std::string& name) {
  // std::map::erase() returns 0 if nothing was erased
  if (!_tables.erase(name)) throw std::runtime_error("Table name not found.");
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const { return _tables.at(name); }

bool StorageManager::has_table(const std::string& name) const { return (_tables.find(name) != _tables.cend()); }

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> _names;
  _names.reserve(_tables.size());
  for (const auto& table : _tables) {
    _names.push_back(table.first);
  }
  return _names;
}

void StorageManager::print(std::ostream& out) const {
  for (const auto& table : _tables) {
    out << "===========================================" << std::endl;
    out << "\tTable name: " << table.first << std::endl;
    out << "\t\t#Columns: " << table.second->col_count() << std::endl;
    out << "\t\t#Rows: " << table.second->row_count() << std::endl;
    out << "\t\t#Chunks: " << table.second->chunk_count() << std::endl;
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
