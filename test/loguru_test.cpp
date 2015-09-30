#include <chrono>
#include <string>
#include <thread>

#define LOGURU_WITH_STREAMS 1
#include "../loguru.hpp"

void sleep_ms(int ms)
{
	VLOG_F(3, "Sleeping for %d ms", ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void test_thread_names()
{
	LOG_SCOPE_FUNCTION_F(INFO);

	LOG_F(INFO, "Hello from main thread!");

	auto a = std::thread([](){
		LOG_F(INFO, "Hello from nameless thread!");
	});

	auto b = std::thread([](){
		loguru::set_thread_name("renderer");
		LOG_F(INFO, "Hello from render thread!");
	});

	auto c = std::thread([](){
		loguru::set_thread_name("abcdefghijklmnopqrstuvwxyz");
		LOG_F(INFO, "Hello from thread with a very long name");
	});

	a.join();
	b.join();
	c.join();
}

void test_scopes()
{
	LOG_SCOPE_FUNCTION_F(INFO);

	VLOG_F(1, "First thing");
	VLOG_F(1, "Second thing");

	{
		VLOG_SCOPE_F(1, "Some indentation");
		VLOG_F(2, "Some info");
		sleep_ms(123);
	}

	sleep_ms(200);
}

void test_levels()
{
	LOG_SCOPE_FUNCTION_F(INFO);
	VLOG_F(3,      "Only visible with -v 3 or higher");
	VLOG_F(2,      "Only visible with -v 2 or higher");
	VLOG_F(1,      "Only visible with -v 1 or higher");
	VLOG_F(0,      "VLOG_F(0)");
	LOG_F(INFO,    "This is some INFO");
	LOG_F(WARNING, "This is a WARNING");
	LOG_F(ERROR,   "This is a serious ERROR");
}

void test_stream()
{
	LOG_SCOPE_FUNCTION_F(INFO);
	LOG_S(INFO) << "Testing stream-logging.";
	VLOG_S(1) << "Stream-logging with verbosity 1";
	VLOG_S(2) << "Stream-logging with verbosity 2";
	VLOG_S(3) << "Stream-logging with verbosity 3";
	LOG_IF_S(INFO, true) << "Should be visible";
	LOG_IF_S(INFO, false) << "SHOULD NOT BE VISIBLE";
	VLOG_IF_S(1, true) << "Should be visible if verbosity is at least 1";
	VLOG_IF_S(1, false) << "SHOULD NOT BE VISIBLE";
}

void print_args(const char* prefix, int argc, char* argv[])
{
	CHECK_EQ_F(argv[argc], nullptr);

	std::string arguments;
	for (int i = 0; i < argc; ++i)
	{
		arguments += argv[i];
		if (i + 1 < argc)
		{
			arguments += " ";
		}
	}

	LOG_F(INFO, "%s%s", prefix, arguments.c_str());
}

int some_expensive_operation() { static int r=31; return r++; }
int BAD = 32;

int main3(int argc, char* argv[])
{
	loguru::init(argc, argv);
	LOG_SCOPE_FUNCTION_F(INFO);
	LOG_F(INFO, "Doing some stuff...");
	for (int i=0; i<2; ++i) {
		VLOG_SCOPE_F(1, "Iteration %d", i);
		auto result = some_expensive_operation();
		if (result == BAD) {
			LOG_F(WARNING, "Bad result");
		}
	}
	LOG_F(INFO, "Time to go!");
	return 0;
}

int main(int argc, char* argv[])
{
	loguru::init(argc, argv);
	loguru::add_file("test.log");
	print_args("Arguments after loguru: ", argc, argv);

	LOG_F(INFO, "Loguru test");

	test_thread_names();
	test_scopes();
	test_levels();
	test_stream();

	const char* ptr = 0;
	assert(ptr && "Error that was unexpected");
	CHECK_NOTNULL_F(ptr);
	CHECK_F(1 > 2, "Oh, no?");
	CHECK_EQ_F(3, 4, "Oh, no?");
	LOG_F(FATAL, "This is the end, beautiful friend");
	return 0;
}
