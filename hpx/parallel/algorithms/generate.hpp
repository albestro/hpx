//  Copyright (c) 2014 Grant Mercer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/generate.hpp

#if !defined(HPX_PARALLEL_DETAIL_GENERATE_JULY_15_2014_0224PM)
#define HPX_PARALLEL_DETAIL_GENERATE_JULY_15_2014_0224PM

#include <hpx/config.hpp>
#include <hpx/traits/concepts.hpp>
#include <hpx/traits/is_iterator.hpp>

#include <hpx/parallel/execution_policy.hpp>
#include <hpx/parallel/algorithms/detail/dispatch.hpp>
#include <hpx/parallel/algorithms/detail/is_negative.hpp>
#include <hpx/parallel/algorithms/for_each.hpp>
#include <hpx/parallel/util/detail/algorithm_result.hpp>

#include <algorithm>
#include <iterator>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace hpx { namespace parallel { HPX_INLINE_NAMESPACE(v1)
{
    ///////////////////////////////////////////////////////////////////////////
    // generate
    namespace detail
    {
        /// \cond NOINTERNAL
        template <typename Iter>
        struct generate : public detail::algorithm<generate<Iter>, Iter>
        {
            generate()
              : generate::algorithm("generate")
            {}

            template <typename ExPolicy, typename FwdIter, typename F>
            static FwdIter
            sequential(ExPolicy, FwdIter first, FwdIter last, F && f)
            {
                std::generate(first, last, std::forward<F>(f));
                return last;
            }

            template <typename ExPolicy, typename FwdIter, typename F>
            static typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
            parallel(ExPolicy policy, FwdIter first, FwdIter last, F && f)
            {
                typedef typename std::iterator_traits<FwdIter>::value_type type;

                return for_each_n<FwdIter>().call(
                        policy, boost::mpl::false_(),
                        first, std::distance(first, last),
                        [f](type& v)
                        {
                            v = f();
                        });
            }
        };

        ///////////////////////////////////////////////////////////////////////
        // non-segmented implementation
        template <typename ExPolicy, typename FwdIter, typename F>
        inline typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
        generate_(ExPolicy && policy, FwdIter first, FwdIter last, F && f,
            std::false_type)
        {
            typedef typename parallel::is_sequential_execution_policy<
                    ExPolicy
                >::type is_seq;

            return detail::generate<FwdIter>().call(
                std::forward<ExPolicy>(policy), is_seq(),
                first, last, std::forward<F>(f));
        }

        ///////////////////////////////////////////////////////////////////////
        // segmented implementation
        template <typename ExPolicy, typename FwdIter, typename F>
        inline typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
        generate_(ExPolicy && policy, FwdIter first, FwdIter last, F && f,
            std::true_type);

        /// \endcond
    }

    /// Assign each element in range [first, last) a value generated by the
    /// given function object f
    ///
    /// \note   Complexity: Exactly \a distance(first, last)
    ///                     invocations of \a f and assignments.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a equal requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param f            generator function that will be called. signature of
    ///                     function should be equivalent to the following:
    ///                     \code
    ///                     Ret fun();
    ///                     \endcode \n
    ///                     The type \a Ret must be such that an object of type
    ///                     \a FwdIter can be dereferenced and assigned a value
    ///                     of type \a Ret.
    ///
    /// The assignments in the parallel \a generate algorithm invoked with an
    /// execution policy object of type \a sequential_execution_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a generate algorithm invoked with
    /// an execution policy object of type \a parallel_execution_policy or
    /// \a parallel_task_execution_policy are permitted to execute in an unordered
    /// fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a replace_if algorithm returns a \a hpx::future<FwdIter>
    ///           if the execution policy is of type
    ///           \a sequential_task_execution_policy or
    ///           \a parallel_task_execution_policy
    ///           and returns \a FwdIter otherwise.
    ///           It returns \a last.
    ///
    template <typename ExPolicy, typename FwdIter, typename F,
    HPX_CONCEPT_REQUIRES_(
        is_execution_policy<ExPolicy>::value &&
        hpx::traits::is_iterator<FwdIter>::value)>
    typename util::detail::algorithm_result<ExPolicy, FwdIter>::type
    generate(ExPolicy && policy, FwdIter first, FwdIter last, F && f)
    {
        typedef typename std::iterator_traits<FwdIter>::iterator_category
            iterator_category;

        static_assert(
            (boost::is_base_of<
                std::forward_iterator_tag, iterator_category>::value),
            "Required at least forward iterator.");

        typedef hpx::traits::segmented_iterator_traits<FwdIter> iterator_traits;
        typedef typename iterator_traits::is_segmented_iterator is_segmented;

        return detail::generate_(
            std::forward<ExPolicy>(policy), first, last,
            std::forward<F>(f), is_segmented());
    }

    ///////////////////////////////////////////////////////////////////////////
    // generate_n
    namespace detail
    {
        /// \cond NOINTERNAL
        template <typename OutIter>
        struct generate_n: public detail::algorithm<generate_n<OutIter>, OutIter>
        {
            generate_n()
                : generate_n::algorithm("generate_n")
            {}

            template <typename ExPolicy, typename F>
            static OutIter
            sequential(ExPolicy, OutIter first, std::size_t count, F && f)
            {
                return std::generate_n(first, count, f);
            }

            template <typename ExPolicy, typename F>
            static typename util::detail::algorithm_result<
                ExPolicy, OutIter
            >::type
            parallel(ExPolicy policy, OutIter first, std::size_t count,
                F && f)
            {
                typedef typename std::iterator_traits<OutIter>::value_type type;

                return
                    for_each_n<OutIter>().call(
                        policy, boost::mpl::false_(), first, count,
                        [f](type& v)
                        {
                            v = f();
                        });
            }
        };
        /// \endcond
    }

    /// Assigns each element in range [first, first+count) a value generated by
    /// the given function object g.
    ///
    /// \note   Complexity: Exactly \a count invocations of \a f and
    ///         assignments, for count > 0.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam OutIter     The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     output iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a equal requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements in the sequence the
    ///                     algorithm will be applied to.
    /// \param f            Refers to the generator function object that will be
    ///                     called. The signature of the function should be
    ///                     equivalent to
    ///                     \code
    ///                     Ret fun();
    ///                     \endcode \n
    ///                     The type \a Ret must be such that an object of type
    ///                     \a OutputIt can be dereferenced and assigned a value
    ///                     of type \a Ret.
    ///
    /// The assignments in the parallel \a generate_n algorithm invoked with an
    /// execution policy object of type \a sequential_execution_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a generate_n algorithm invoked with
    /// an execution policy object of type \a parallel_execution_policy or
    /// \a parallel_task_execution_policy are permitted to execute in an unordered
    /// fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a replace_if algorithm returns a \a hpx::future<OutIter>
    ///           if the execution policy is of type
    ///           \a sequential_task_execution_policy or
    ///           \a parallel_task_execution_policy
    ///           and returns \a OutIter otherwise.
    ///           It returns \a last.
    ///
    template <typename ExPolicy, typename OutIter, typename Size, typename F,
    HPX_CONCEPT_REQUIRES_(
        is_execution_policy<ExPolicy>::value &&
        hpx::traits::is_iterator<OutIter>::value)>
    typename util::detail::algorithm_result<ExPolicy, OutIter>::type
    generate_n(ExPolicy && policy, OutIter first, Size count, F && f)
    {
        typedef typename std::iterator_traits<OutIter>::iterator_category
            iterator_category;

        static_assert(
            (boost::mpl::or_<
                boost::is_base_of<
                    std::forward_iterator_tag, iterator_category>,
                boost::is_same<
                    std::output_iterator_tag, iterator_category>
            >::value),
            "Requires at least output iterator.");

        if (detail::is_negative(count))
        {
            return util::detail::algorithm_result<ExPolicy, OutIter>::get(
                std::move(first));
        }

        typedef typename boost::mpl::or_<
            is_sequential_execution_policy<ExPolicy>,
            boost::is_same<std::output_iterator_tag, iterator_category>
        >::type is_seq;

        return detail::generate_n<OutIter>().call(
            std::forward<ExPolicy>(policy), is_seq(),
            first, std::size_t(count), std::forward<F>(f));
    }
}}}

#endif
