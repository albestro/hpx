//  Copyright (c) 2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_COMPUTE_HOST_DEFAULT_EXECUTOR_HPP
#define HPX_COMPUTE_HOST_DEFAULT_EXECUTOR_HPP

#include <hpx/config.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/traits/executor_traits.hpp>
#include <hpx/traits/is_executor.hpp>

#include <hpx/compute/host/target.hpp>

#include <type_traits>
#include <vector>

namespace hpx { namespace compute { namespace host {
    struct default_executor
    {
        default_executor(host::target& target)
        //         : target_(target)
        {
        }

        /// \cond NOINTERNAL
        bool operator==(default_executor const& rhs) const noexcept
        {
            return true;
        }

        bool operator!=(default_executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        default_executor const& context() const noexcept
        {
            return *this;
        }
        /// \endcond

        template <typename F, typename... Ts>
        void post(F&& f, Ts&&... ts)
        {
        }

        template <typename F, typename... Ts>
        hpx::future<void> async_execute(F&& f, Ts&&... ts)
        {
            return hpx::future<void>();
        }

        template <typename F, typename... Ts>
        static void sync_execute(F&& f, Ts&&... ts)
        {
        }

        template <typename F, typename Shape, typename... Ts>
        static std::vector<hpx::future<void>> bulk_async_execute(
            F&& f, Shape const& shape, Ts&&... ts)
        {
            return std::vector<hpx::future<void>>();
        }

        template <typename F, typename Shape, typename... Ts>
        static void bulk_sync_execute(F&& f, Shape const& shape, Ts&&... ts)
        {
        }

    private:
        //       host::target& target_;
    };
}}}    // namespace hpx::compute::host

namespace hpx { namespace parallel { namespace execution {
    template <>
    struct executor_execution_category<compute::host::default_executor>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <>
    struct is_one_way_executor<compute::host::default_executor> : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<compute::host::default_executor> : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<compute::host::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<compute::host::default_executor>
      : std::true_type
    {
    };
}}}    // namespace hpx::parallel::execution

#endif
