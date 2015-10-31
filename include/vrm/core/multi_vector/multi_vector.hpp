// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <memory>
#include <tuple>
#include <vrm/core/config.hpp>
#include <vrm/core/assert.hpp>
#include <vrm/core/resizable_buffer.hpp>
#include <vrm/core/tuple_utils.hpp>

VRM_CORE_NAMESPACE
{
    namespace impl
    {
        template <typename TMultiBuffer>
        class VRM_CORE_CLASS_API multi_vector
        {
        public:
            using size_type = std::size_t;
            using multi_buffer_type = TMultiBuffer;

            static constexpr std::size_t buffer_count{
                multi_buffer_type::buffer_count};

            static constexpr size_type initial_capacity{0};

            using value_tuple = typename multi_buffer_type::value_tuple;

            using value_reference_tuple =
                typename multi_buffer_type::value_reference_tuple;

            using const_value_reference_tuple =
                typename multi_buffer_type::const_value_reference_tuple;

        private:
            multi_buffer_type _multi_buffer;
            size_type _capacity{0};
            size_type _size{0};

            void grow_if_necessary(size_type desired_size)
            {
                if(desired_size > _capacity)
                {
                    reserve((_capacity + 10) * 2);
                }
            }

            template <std::size_t TN>
            using nth_buffer_type =
                typename multi_buffer_type::template nth_buffer_type<TN>;

            template <std::size_t TN>
            using nth_buffer_value_type =
                typename multi_buffer_type::template nth_buffer_value_type<TN>;

        public:
            multi_vector() = default;

            ~multi_vector()
            {
                _multi_buffer.destroy(0, _size);
                _multi_buffer.deallocate(_capacity);
            }

            // TODO: implement
            multi_vector(const multi_vector&) = default;
            multi_vector& operator=(const multi_vector&) = default;

            multi_vector(multi_vector&&) = default;
            multi_vector& operator=(multi_vector&&) = default;

            const auto& size() const noexcept { return _size; }
            const auto& capacity() const noexcept { return _capacity; }
            auto empty() const noexcept { return _size == 0; }

            void reserve(size_type new_capacity)
            {
                VRM_CORE_ASSERT_OP(new_capacity, >, _capacity);

                _multi_buffer.grow(_capacity, new_capacity);
                _capacity = new_capacity;
            }

            void resize(size_type new_size)
            {
                VRM_CORE_ASSERT_OP(new_size, >, _size);
                grow_if_necessary(new_size);

                for(size_type i(_size); i < new_size; ++i)
                    _multi_buffer.construct_at(i);

                _size = new_size;
            }

            void clear()
            {
                _multi_buffer.destroy(0, _size);
                _size = 0;
            }

            // TODO:
            // void insert() {}
            // void emplace() {}
            // void erase() {}

            // TODO:
            template <typename... Ts>
            void unsafe_emplace_back(
                Ts&&... xs) // TODO: noexcept(noexcept(...))
            {
                VRM_CORE_STATIC_ASSERT_NM(sizeof...(xs) == buffer_count);

                VRM_CORE_ASSERT_OP(_capacity, >, _size);
                auto ref_tuple(_multi_buffer[_size]);

                for_args_data(
                    [&ref_tuple](auto data, auto&& r)
                    {
                        using type =
                            nth_buffer_value_type<decltype(data)::index>;

                        using type_ref = std::add_lvalue_reference_t<type>;

                        auto address(&(std::get<type_ref>(ref_tuple)));

                        auto placement_new([&](auto&&... args)
                            {
                                new(address) type(FWD(args)...);
                            });

                        apply(placement_new, r);
                    },
                    FWD(xs)...);

                ++_size;
            }

            template <typename... Ts>
            void unsafe_push_back(Ts&&... xs)
            {
                unsafe_emplace_back(std::make_tuple(FWD(xs))...);
            }

            template <typename... Ts>
            void emplace_back(Ts&&... xs)
            {
                grow_if_necessary(_size + 1);
                unsafe_emplace_back(FWD(xs)...);
            }

            template <typename... Ts>
            void push_back(Ts&&... xs)
            {
                emplace_back(std::make_tuple(FWD(xs))...);
            }

            void pop_back()
            {
                VRM_CORE_ASSERT_OP(_size, >, 0);
                _multi_buffer.destroy_at(_size - 1);
                --_size;
            }


            auto operator[](size_type pos) noexcept
            {
                VRM_CORE_ASSERT_OP(_size, >, pos);
                return _multi_buffer[pos];
            }

            auto operator[](size_type pos) const noexcept
            {
                VRM_CORE_ASSERT_OP(_size, >, pos);
                return _multi_buffer[pos];
            }

            auto back() noexcept { return (*this)[_size - 1]; }

            auto back() const noexcept { return (*this)[_size - 1]; }

            //template <typename T>
            //auto& get(size_type pos) noexcept
            //{
                //VRM_CORE_ASSERT_OP(_size, >, pos);
                //return _multi_buffer
            //}
        };
    }
}
VRM_CORE_NAMESPACE_END

// TODO:
// * split to inl
// * noexcept