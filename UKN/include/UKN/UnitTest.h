#ifndef Project_Unknown_UnitTest_H_
#define Project_Unknown_UnitTest_H_

#include "UKN/Error.h"
#include "UKN/Logger.h"

namespace ukn {

	class UnitTestError {
	
	};
	
	namespace unittest {
	
		#define UKN_TEST_CHECK_ERROR(cond, description) do { if(!(cond)) throw Error(description); } while(0);
		#define UKN_TEST_ASSERT(cond) do { ukn_test_check_error(cond, L""); } while(0);
		#define UKN_TEST_ERROR(cond) do { try { cond; throw UnitTestError(); } catch (const Error&) {} catch(const UnitTestError&) { ukn_test_check_error(false, L""); } } while(0) 
	
		#define UKN_TEST_CASE(name) \
			extern void ukn_testcase_##name();				\
			namespace vl_unittest_executors {				\
				class ukn_testcase_runner_##name {			\
				public:										\
					ukn_testcase_runner_##name() {			\
						log_info(ukn_wstring("TestCase: ")+L#name);					\
						ukn_testcase_##name();				\
					}										\
				} ukn_testcase_runner_##name##_instance;	\
			}												\
			void ukn_testcase_##name()
	
	} // namespace unittest
	
} // namespace ukn

#endif