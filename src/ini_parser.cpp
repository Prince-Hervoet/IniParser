#include "ini_parser.hpp"

#include <fstream>
#include <iostream>

namespace let_me_see {
int IniParser::LoadFile(const char* path) {
  record.clear();
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "unable to open this file." << std::endl;
    return 0;
  }
  bool is_ok = true;
  StringType in_line;
  StringType current_section;
  StringType key;
  StringType value;
  while (std::getline(input_file, in_line)) {
    if (in_line.size() == 0) continue;
    if (in_line[0] == kCommentFlag1 || in_line[0] == kCommentFlag2) continue;
    key.clear();
    value.clear();
    parseFileString(in_line, current_section, key, value);
    if (key.size() > 0 && value.size() == 0) {
      std::cerr << "unknown key and value" << std::endl;
      is_ok = false;
      break;
    }
    if (key.size() > 0 && value.size() > 0) {
      if (current_section.size() == 0) {
        std::cerr << "unknown section" << std::endl;
        is_ok = false;
        break;
      }
      if (record.count(current_section) == 0) {
        std::map<StringType, StringType> temp;
        temp.insert({key, value});
        record.insert({current_section, std::move(temp)});
      } else {
        record[current_section].insert({key, value});
      }
    }
  }
  input_file.close();
  if (is_ok) {
    has_file = true;
    return 1;
  }
  record.clear();
  return 0;
}

void IniParser::parseFileString(StringType& in_line,
                                StringType& current_section, StringType& key,
                                StringType& value) {
  if (in_line[0] == kSectionStartFlag &&
      in_line[in_line.size() - 1] == kSectionEndFlag) {
    current_section = in_line.substr(1, in_line.size() - 2);
    return;
  }
  bool is_key_str = true;
  for (int i = 0; i < in_line.size(); ++i) {
    if (in_line[i] == kKeyValueFlag) {
      is_key_str = false;
      continue;
    }
    if (is_key_str)
      key.append(1, in_line[i]);
    else
      value.append(1, in_line[i]);
  }
}

std::string IniParser::GetValue(const StringType& section,
                                const StringType& key) {
  if (section.size() == 0 || key.size() == 0) return "";
  if (record.count(section) == 0) return "";
  if (record[section].count(key) == 0) return "";
  return record.at(section).at(key);
}

}  // namespace let_me_see