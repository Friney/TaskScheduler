all: tests

tests:
	g++ tests/task_scheduler_test.cc  src/task_scheduler.cc  -o test_scheduler -lgtest -lgtest_main -pthread && ./test_scheduler

clean:
	rm test_scheduler
	
.PHONY: tests clean