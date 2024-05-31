#ifndef _INI_PARSER_H_
#define _INI_PARSER_H_

#include <map>
#include <string>

namespace let_me_see {
class IniParser {
  using StringType = std::string;
  using KeyValueType = std::map<StringType, StringType>;
  using RecordType = std::map<StringType, KeyValueType>;

 private:
  RecordType record;
  bool has_file = false;
  static const char kCommentFlag1 = '#';      // 注释开始标志1
  static const char kCommentFlag2 = ';';      // 注释开始标志2
  static const char kSectionStartFlag = '[';  // 段开始标志
  static const char kSectionEndFlag = ']';    // 段结束标志
  static const char kKeyValueFlag = '=';      // 键值对间隔标志

 public:
  int LoadFile(const char* path);

  StringType GetValue(const StringType& section, const StringType& key) const;

  void Reset() {
    record.clear();
    has_file = false;
  }

  bool HasFile() const { return has_file; }

  int SectionSize() const { return record.size(); }

  int KeyValueSize(const StringType& section) const {
    if (section.size() == 0) return 0;
    if (record.count(section) == 0) return 0;
    return record.at(section).size();
  }

 private:
  void parseFileString(StringType& in_line, StringType& current_section,
                       StringType& key, StringType& value);
};
}  // namespace let_me_see

#endif