#ifndef CONTEST_H
#define CONTEST_H

#include <string>
#include <sstream>
#include <vector>
#include <memory>

#include "contest_report.h"

namespace CTNS {

class ContestException {
  std::string msg_;
public:
  ContestException(const std::string &msg): msg_(msg) {}
  const std::string& what() const { return msg_; }
};

class Contest {
  std::string name_;
public:
  Contest(const std::string &name): name_(name) {}
  const std::string& get_name() { return name_; }
  void run() { return do_run(); }
  virtual void do_run() = 0;
};

class ContestSuite {
  typedef std::vector<Contest*> TestVector;
  TestVector tests_;
  std::string name_;
  std::unique_ptr<TestReportIface> reporter;

  static ContestSuite *instance_;
  ContestSuite():reporter(new ConsoleReport()) {}
  ContestSuite(const ContestSuite&);
  ContestSuite& operator=(const ContestSuite&);
 
public:
  void set_name(const std::string& name) { name_ = name; }
  void register_test(Contest *test) { tests_.push_back(test); }

  static ContestSuite* get() {
    if (!instance_) {
      instance_ = new ContestSuite();
    }
    return instance_;
  }

  int run_tests() {
    int errors = 0;
    reporter->test_suite_init(name_);
    for (TestVector::iterator it = tests_.begin();
        it != tests_.end();
        it++) {
      reporter->test_case_init((*it)->get_name());
      try {
        (*it)->run();
        reporter->success("");
      } catch(const ContestException &e) {
        reporter->failure(e.what());
        errors++;
      } catch(...) {
        reporter->error("Unknown exception thrown.");
        errors++;
      }
      reporter->test_case_end();
    }
    reporter->test_suite_end();
    return errors > 0;
  }
};

template<typename T>
bool compare(T a, T b) {return a == b;}

template<typename T>
bool is_true(T a) {return a ? true : false;}

} // ends namespace CTNS

#define CONTEST_SUITE(suite_name)               \
using namespace CTNS;                           \
ContestSuite* ContestSuite::instance_ = 0;      \
void test_suite_initialize() {                  \
  ContestSuite::get()->set_name(#suite_name);   \
}                                               \
namespace { /* Isolates the test-case */        \


#define CONTEST_SUITE_END                       \
}                                               \
int main(int argc, char **argv) {               \
  (void)argc;                                   \
  (void)argv;                                   \
  test_suite_initialize();                      \
  return ContestSuite::get()->run_tests();      \
}                                               \

#define CONTEST_CASE(test_name)                 \
class test_name : public Contest {              \
public:                                         \
  test_name(): Contest(#test_name)              \
  { ContestSuite::get()->register_test(         \
      reinterpret_cast<Contest*>(this)); }      \
                                                \
  virtual void do_run();                        \
} test_name ## instance;                        \
void test_name::do_run()                        \


#define CONTEST_EQUAL(a, b)                     \
do {                                            \
  if (!compare((a), (b))) {                     \
      std::ostringstream oss;                   \
      oss << " [" << #a << " != " << #b << "] ";\
      oss << __FILE__ << ": " << __LINE__;      \
      throw ContestException(oss.str());        \
  }                                             \
} while(0);                                     \

#define CONTEST_TRUE(a)                         \
do {                                            \
  if (!is_true((a))) {                          \
      std::ostringstream oss;                   \
      oss << " [" << #a << " != true] ";        \
      oss << __FILE__ << ": " << __LINE__;      \
      throw ContestException(oss.str());        \
  }                                             \
} while(0);                                     \

#endif // CONTEST_H
