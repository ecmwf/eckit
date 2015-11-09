#include "eckit/testing/UnitTest.h"



void UnitTestRunner::setUp()
{
}

void UnitTestRunner::test()
{
}

void UnitTestRunner::tearDown()
{
}

void UnitTestRunner::run()
{
    setUp();
    test();
    tearDown();
}

UnitTestRunner::UnitTestRunner(int argc, char** argv):
    eckit::Tool(argc, argv)
{
}

void SimpleTestRunner::test()
{
    t_();
}

void SimpleTestRunner::setUp()
{
    u_();
}

void SimpleTestRunner::tearDown()
{
    d_();
}

SimpleTestRunner::SimpleTestRunner(int argc, char** argv,
                                   test_proc_t t,
                                   test_proc_t u,
                                   test_proc_t d):
    UnitTestRunner(argc, argv),
    t_(t),
    u_(u),
    d_(d)
{

}


static OneTest* tests = 0;


OneTest::OneTest(const char* n, test_proc_t t):
    n_(n),
    t_(t),
    next_(tests)
{
    tests =  this;
}

void OneTest::test() {
    if(next_) next_->test();
    eckit::Log::info() << "Test " << n_ << std::endl;
    t_();
}

void AllTestsRunner::test()
{
    tests->test();
}

AllTestsRunner::AllTestsRunner(int argc, char** argv):
    UnitTestRunner(argc, argv)
{

}

