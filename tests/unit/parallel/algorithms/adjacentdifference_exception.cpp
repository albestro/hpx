//  Copyright (c) 2015 Daniel Bourgeois
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_adjacent_difference.hpp>
#include <hpx/util/lightweight_test.hpp>
#include <boost/range/functions.hpp>

#include "test_utils.hpp"

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_adjacent_difference_exception(ExPolicy policy, IteratorTag)
{
    static_assert(
        hpx::parallel::is_execution_policy<ExPolicy>::value,
        "hpx::parallel::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator <base_iterator, IteratorTag>
        decorated_iterator;
    std::vector<std::size_t> c(10007);
    std::vector<std::size_t> d(10007);

    bool caught_exception = false;
    try {
        hpx::parallel::adjacent_difference(policy,
            decorated_iterator(boost::begin(c)),
            decorated_iterator(boost::end(c)), boost::begin(d),
            [](std::size_t lhs, std::size_t rhs) -> std::size_t
            {
                throw std::runtime_error("test");
                return lhs - rhs;
            });
        HPX_TEST(false);
    }
    catch (hpx::exception_list const& e) {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(policy, e);
    }
    catch (...) {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
}

template <typename ExPolicy, typename IteratorTag>
void test_adjacent_difference_exception_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator <base_iterator, IteratorTag>
        decorated_iterator;

    std::vector<std::size_t> c(10007);
    std::vector<std::size_t> d(10007);

    bool caught_exception = false;
    bool returned_from_algorithm = false;

    try {
        hpx::future<base_iterator> f =
        hpx::parallel::adjacent_difference(p,
            decorated_iterator(boost::begin(c)),
            decorated_iterator(boost::end(c)), boost::begin(d),
            [](std::size_t lhs, std::size_t rhs) -> std::size_t
            {
                throw std::runtime_error("test");
                return lhs - rhs;
            });

        returned_from_algorithm = true;

        f.get();

        HPX_TEST(false);
    }
    catch(hpx::exception_list const& e) {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(p, e);
    }
    catch (...) {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
    HPX_TEST(returned_from_algorithm);
}

template <typename IteratorTag>
void test_adjacent_difference_exception()
{
    using namespace hpx::parallel;

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_adjacent_difference_exception(seq, IteratorTag());
    test_adjacent_difference_exception(par, IteratorTag());

    test_adjacent_difference_exception_async(seq(task), IteratorTag());
    test_adjacent_difference_exception_async(par(task), IteratorTag());

    test_adjacent_difference_exception(execution_policy(seq), IteratorTag());
    test_adjacent_difference_exception(execution_policy(par), IteratorTag());

    test_adjacent_difference_exception(execution_policy(seq(task)), IteratorTag());
    test_adjacent_difference_exception(execution_policy(par(task)), IteratorTag());
}

void adjacent_difference_exception_test()
{
    test_adjacent_difference_exception<std::random_access_iterator_tag>();
    test_adjacent_difference_exception<std::forward_iterator_tag>();
}

int hpx_main(boost::program_options::variables_map& vm)
{
    unsigned int seed = (unsigned int)std::time(0);
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);

    adjacent_difference_exception_test();
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // add command line option which controls the random number generator seed
    using namespace boost::program_options;
    options_description desc_commandline(
        "Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ("seed,s", value<unsigned int>(),
        "the random number generator seed to use for this run")
        ;

    // By default this test should run on all available cores
    std::vector<std::string> cfg;
    cfg.push_back("hpx.os_threads=" +
        std::to_string(hpx::threads::hardware_concurrency()));

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(desc_commandline, argc, argv, cfg), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
