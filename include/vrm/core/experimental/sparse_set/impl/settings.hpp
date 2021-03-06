// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <vrm/core/config.hpp>
#include <vrm/core/assert.hpp>
#include <vrm/core/type_aliases.hpp>
#include <vrm/core/strong_typedef.hpp>

VRM_CORE_NAMESPACE
{
    namespace impl
    {
        template <typename T, typename TStorage>
        class sparse_set_settings
        {
        private:
            using underlying_value_type = underlying_if_strong_typedef_type<T>;

            VRM_CORE_STATIC_ASSERT_NM(
                std::is_arithmetic<underlying_value_type>{} &&
                std::is_unsigned<underlying_value_type>{});

        public:
            /// @brief Size type used for storage sizes/capacities.
            using size_type = sz_t;

            /// @brief Type of values stored in the sparse set.
            using value_type = T;

            /// @brief Type of storage.
            using storage_type = TStorage;
        };
    }
}
VRM_CORE_NAMESPACE_END
