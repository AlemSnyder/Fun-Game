#if 0
#pragma once

// Always required
#include "quill/bundled/fmt/format.h"
#include "quill/core/Codec.h"
#include "quill/core/DynamicFormatArgStore.h"
#include "quill/core/InlinedVector.h"

// To serialise the std::array member of User you need Array.h otherwise you don't need to include this
#include "quill/std/Array.h"
#include "quill/std/Vector.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utility> // for declval only required if you do the decoding manualy and use declval
#include <array>

/***/
template <int i, class T, glm::qualifier Q>
struct fmtquill::formatter<glm::vec<i, T, Q>>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(glm::vec<i, T, Q> const& v, format_context& ctx) const
  {
    return fmtquill::format_to(ctx.out(), "{}", glm::to_string(v));
  }
};


/***/
template <int i, class T, glm::qualifier Q>
struct quill::Codec<glm::vec<i, T, Q>>
{
  static size_t compute_encoded_size(detail::SizeCacheVector& conditional_arg_size_cache, glm::vec<i, T, Q> const& v) noexcept
  {
    std::array<T, i> data;
        memcpy(data.data(),glm::value_ptr(v), sizeof(data));


    // pass as arguments the class members you want to serialize
    return compute_total_encoded_size(conditional_arg_size_cache, data);
  }

  static void encode(std::byte*& buffer, detail::SizeCacheVector const& conditional_arg_size_cache,
                     uint32_t& conditional_arg_size_cache_index, glm::vec<i, T, Q> const& v) noexcept
  {
    std::array<T, i> data;
    memcpy(data.data(), glm::value_ptr(v), sizeof(data));
    // You must encode the same members and in the same order as in compute_total_encoded_size
    encode_members(buffer, conditional_arg_size_cache, conditional_arg_size_cache_index, data);
  }

  static ::glm::vec<i, T, Q> decode_arg(std::byte*& buffer)
  {
    // You must decode the same members and in the same order as in encode
    std::array<T, i> data;
    decode_members(buffer, data);


    glm::vec<i, T, Q> v;

    memcpy(glm::value_ptr(v), data.data(), sizeof(data));
    return v;

    // note:
    // If the object is not default constructible you can also do it manually without
    // decode_members helper

    // auto name = Codec<decltype(std::declval<::User>().name)>::decode_arg(buffer);
    // auto surname = Codec<decltype(std::declval<::User>().surname)>::decode_arg(buffer);
    // auto age = Codec<decltype(std::declval<::User>().age)>::decode_arg(buffer);
    // auto favorite_colors = Codec<decltype(std::declval<::User>().favorite_colors)>::decode_arg(buffer);

    // ::User user{name, surname, age, favorite_colors};

    // return user;
  }

  static void decode_and_store_arg(std::byte*& buffer, DynamicFormatArgStore* args_store)
  {
    args_store->push_back(decode_arg(buffer));
  }
};

#endif
