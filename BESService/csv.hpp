//
// Created by Otto on 2018-01-29
//
#pragma once

#include <string>
#include <vector>
#include <cassert>
#include "reflection.hpp"

namespace terra
{
    namespace Error
    {
        struct Head_Parse_Error {
            const char* err;
            Head_Parse_Error(const char* msg) { err = msg; }
        };
        struct Synax_Error {
            const char* err;
            Synax_Error(const char* msg) { err = msg; }
        };
    }

    namespace detail
    {
        struct string_ref {
            char const* str;
            size_t len;

            int length() const { return static_cast<int>(len); }

            bool operator!=(const char* data) const { return strncmp(str, data, strlen(data)); }
        };
    }
    struct column_info {
        detail::string_ref str_ref;
    };
    struct token {
        detail::string_ref str;
        enum {
            t_string,
            t_comma,
            t_line_end,
            t_end,
        } type;
        union {
            int64_t i64;
            uint64_t u64;
            double d64;
        } value;
        bool neg = false;
    };
    namespace csv
    {
        class csvreader_t
        {
        private:
            char* ptr_;
            size_t len_;
            size_t skip_line_;
            size_t cur_offset_{0};
            size_t cur_line_{0};
            size_t cur_col_{0};
            bool end_mark_{false};
            token cur_tok_;
            std::vector<column_info> cols_;

        public:
            csvreader_t(char* ptr, size_t len, size_t skip_line = 0)
                : ptr_(ptr), len_(len), skip_line_(skip_line)
            {
            }
            std::vector<column_info> const& get_column_info() const { return cols_; }
            inline token const& peek() const { return cur_tok_; }
            void next()
            {
                assert(cur_line_ >= skip_line_);
                char c = read();
                switch (c) {
                    case '"': {
                        cur_tok_.type = token::t_string;
                        parse_quote_string();
                        break;
                    }
                    case '\r':
                    case '\n':
                    case ',': {
                        cur_tok_.type = token::t_string;
                        cur_tok_.str.str = ptr_ + cur_offset_;
                        cur_tok_.str.len = 0;
                        break;
                    }
                    default: {
                        cur_tok_.type = token::t_string;
                        parse_string();
                        break;
                    }
                }
            }
            token const& read_separator()
            {
                char c = read();
                switch (c) {
                    case 0: {
                        cur_tok_.type = token::t_end;
                        cur_tok_.str.str = ptr_ + cur_offset_;
                        cur_tok_.str.len = 1;
                    } break;
                    case '\r': {
                        cur_tok_.type = token::t_line_end;
                        cur_tok_.str.str = ptr_ + cur_offset_;
                        cur_tok_.str.len = 2;
                        take();
                        take();
                    } break;
                    case '\n': {
                        cur_tok_.type = token::t_line_end;
                        cur_tok_.str.str = ptr_ + cur_offset_;
                        cur_tok_.str.len = 1;
                        take();
                    } break;
                    case ',': {
                        cur_tok_.type = token::t_comma;
                        cur_tok_.str.str = ptr_ + cur_offset_;
                        cur_tok_.str.len = 1;
                        take();
                    } break;
                    default:
                        throw Error::Synax_Error("illegal separator");
                        break;
                }
                return cur_tok_;
            }
            void skip_head_line()
            {
                assert(cur_line_ == 0);
                if (skip_line_ == 0) {
                    return;
                }
                while (cur_line_ < skip_line_) {
                    take();
                    if (end_mark_) {
                        throw Error::Head_Parse_Error("invalid skip line value");
                        // break;
                    }
                }
                take();
            }
            void add_column_info(const column_info& col) { cols_.push_back(col); }
            bool expect(char c) { return cur_tok_.str.str[0] == c; }

        private:
            void parse_quote_string()
            {
                take();
                cur_tok_.str.str = ptr_ + cur_offset_;
                char c = read();
                do {
                    switch (c) {
                        case 0:
                        case '\n': {
                            throw Error::Head_Parse_Error("not a valid quote string!");
                            break;
                        }
                        case '"': {
                            take();
                            char next_char = read();
                            if (next_char == '"') {
                                break;
                            }
                            cur_tok_.str.len = ptr_ + cur_offset_ - 1 - cur_tok_.str.str;
                            return;
                        }
                        default:
                            break;
                    }
                    take();
                    c = read();
                } while (true);
            }
            void parse_string()
            {
                cur_tok_.str.str = ptr_ + cur_offset_;
                char c = read();
                do {
                    switch (c) {
                        case 0:
                        case '\r':
                        case '\n':
                        case ',': {
                            cur_tok_.str.len = ptr_ + cur_offset_ - cur_tok_.str.str;
                            return;
                        }
                        default:
                            break;
                    }
                    take();
                    c = read();
                } while (true);
            }
            inline void take()
            {
                if (end_mark_ == false) {
                    ++cur_offset_;
                    char v = ptr_[cur_offset_];
                    if (v != '\r') ++cur_col_;
                    if (len_ > 0 && cur_offset_ >= len_) {
                        end_mark_ = true;
                    } else if (v == 0) {
                        end_mark_ = true;
                    }
                    if (v == '\n') {
                        cur_col_ = 0;
                        ++cur_line_;
                    }
                }
            }

            inline char read() const
            {
                if (end_mark_) return 0;
                return ptr_[cur_offset_];
            }
        };

        template <typename T>
        std::enable_if_t<is_emplace_back_able<T>::value> emplace_back(T& val)
        {
            val.emplace_back();
        }

        template <typename T>
        inline std::enable_if_t<is_signed_intergral_like<T>::value> read_row(csvreader_t& rd, T& val)
        {
            assert(rd.peek().type == token::t_string);
            int64_t temp = std::strtoll(rd.peek().str.str, nullptr, 10);
            val = static_cast<T>(temp);
        }

        template <typename T>
        inline std::enable_if_t<is_unsigned_intergral_like<T>::value> read_row(csvreader_t& rd, T& val)
        {
            assert(rd.peek().type == token::t_string);
            int64_t temp = std::strtoull(rd.peek().str.str, nullptr, 10);
            val = static_cast<T>(temp);
        }

        inline void read_row(csvreader_t& rd, std::string& val)
        {
            assert(rd.peek().type == token::t_string);
            val.assign(rd.peek().str.str, rd.peek().str.len);
        }
        template <typename T, typename = std::enable_if_t<is_reflection<T>::value>>
        inline void read_row(csvreader_t& rd, T& val)
        {
            do_read_row(rd, val);
        }

        template <typename K, typename V,
                  typename = std::enable_if_t<is_reflection<K>::value && is_reflection<V>::value>>
        inline void read_row(csvreader_t& rd, K& k, V& v)
        {
            do_read_row(rd, k, v);
        }

        template <typename T>
        inline std::enable_if_t<is_sequence_container<T>::value> read_row(csvreader_t& rd, T& val)
        {
            rd.next();
            while (rd.peek().str.str[0] != '\0') {
                emplace_back(val);
                read_row(rd, val.back());
            }
        }

        template <typename T>
        inline std::enable_if_t<is_associat_container<T>::value> read_row(csvreader_t& rd, T& val)
        {
            rd.next();
            while (rd.peek().str.str[0] != '\0') {
                typename T::key_type k;
                typename T::mapped_type v;
                read_row(rd, k, v);
                val[k] = v;
            }
        }

        template <typename T, typename = std::enable_if_t<is_reflection<T>::value>>
        bool from_csv(T&& t, char* buf, size_t len, size_t skip_line)
        {
            bool res = false;
            csvreader_t rd(buf, len, skip_line);
            do_parse_column_info(rd, std::forward<T>(t));
            do_read(rd, std::forward<T>(t));
            return !res;
        }

        template <typename T, typename = std::enable_if_t<is_reflection<T>::value>>
        void do_parse_column_info(csvreader_t& rd, T&& t)
        {
            rd.skip_head_line();
            using M = decltype(terra_reflect_members(std::forward<T>(t)));
            constexpr auto count = M::value();
            static_assert(count == 1, "invalid count value");
            auto tp = M::apply_impl();
            using U = std::decay_t<decltype(std::forward<T>(t).*(std::get<0>(tp)))>;
            static_assert(is_sequence_container<U>::value || is_associat_container<U>::value,
                          "invalid container type");
            do {
                rd.next();
                column_info col;
                col.str_ref.str = rd.peek().str.str;
                col.str_ref.len = rd.peek().str.length();
                rd.add_column_info(col);
                //std::cout << '[' << std::string(col.str_ref.str, col.str_ref.len) << ']' << std::endl;
                token const& tok = rd.read_separator();
                if (tok.type == token::t_line_end) {
                    break;
                }
            } while (true);
        }

        template <typename T, typename = std::enable_if_t<is_reflection<T>::value>>
        void do_read_row(csvreader_t& rd, T&& t)
        {
            using M = decltype(terra_reflect_members(std::forward<T>(t)));
            auto tp = M::apply_impl();
            constexpr auto Size = M::value();
            for (size_t i = 0; i < rd.get_column_info().size(); ++i) {
                const auto& col = rd.get_column_info().at(i);
                auto distance = get_index<T>(col.str_ref.str);
                if (distance < static_cast<decltype(distance)>(Size)) {
                    tuple_switch(distance, tp, [&t, &rd](auto& val) { read_row(rd, t.*val); },
                                 std::make_index_sequence<Size>{});
                }
                rd.read_separator();
                rd.next();
            }
        }

        template <typename K, typename V,
                  typename = std::enable_if_t<is_reflection<K>::value && is_reflection<V>::value>>
        void do_read_row(csvreader_t& rd, K& k, V& v)
        {
            using MK = decltype(terra_reflect_members(std::forward<K>(k)));
            auto tk = MK::apply_impl();
            constexpr auto SizeK = MK::value();
            using MV = decltype(terra_reflect_members(std::forward<V>(v)));
            auto tv = MV::apply_impl();
            constexpr auto SizeV = MV::value();

            for (size_t i = 0; i < rd.get_column_info().size(); ++i) {
                const auto& col = rd.get_column_info().at(i);
                auto distance_k = get_index<K>(col.str_ref.str);
                auto distance_v = get_index<V>(col.str_ref.str);
                if (distance_k < static_cast<decltype(distance_k)>(SizeK)) {
                    tuple_switch(distance_k, tk, [&k, &rd](auto& val) { read_row(rd, k.*val); },
                                 std::make_index_sequence<SizeK>{});
                }
                if (distance_v < static_cast<decltype(distance_v)>(SizeV)) {
                    tuple_switch(distance_v, tv, [&v, &rd](auto& val) { read_row(rd, v.*val); },
                                 std::make_index_sequence<SizeV>{});
                }
                rd.read_separator();
                rd.next();
            }
        }

        template <typename T, typename = std::enable_if_t<is_reflection<T>::value>>
        void do_read(csvreader_t& rd, T&& t)
        {
            for_each(std::forward<T>(t), [&t, &rd](const auto& v, auto i) {
                using M = decltype(terra_reflect_members(std::forward<T>(t)));
                constexpr auto Idx = decltype(i)::value;
                constexpr auto Count = M::value();
                static_assert(Idx < Count, "invalid Idx");
                //const char* s = get_name<T>(Idx);
                read_row(rd, t.*v);
            });
        }
    }
}