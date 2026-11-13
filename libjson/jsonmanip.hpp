#pragma once

#include <ostream>
#include <string_view>
#include <iterator>
#include <charconv>

namespace json {

  struct json_string {
    std::string_view str;
  };

  inline std::ostream& operator <<(std::ostream& out, json_string q) {
    static constexpr char hex[] = "0123456789abcdef";
    out << '"';
    for (unsigned char ch : q.str) {
      switch (ch) {
      case '"':  out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      case '\b': out << "\\b"; break;
      case '\f': out << "\\f"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default:
        if (ch < 0x20) {
          out << "\\u00" << hex[(ch >> 4) & 0xF] << hex[ch & 0xF];
        }
        else {
          out << ch;
        }
      }
    }
    return out << '"';
  }



  struct json_number {
    double num;
  };

  inline std::ostream& operator <<(std::ostream& out, json_number n) {
      char buf[32];
      auto [end, ec] = std::to_chars(buf, std::end(buf), n.num);
      return out.write(buf, end - buf);
  }

}