CC=g++
CFLAGS=-std=c++11 -lstdc++
TEST_SRC=test/hot_cold/main.cc
TEST_EXEC=main

compile_test: $(TEST_EXEC)

$(TEST_EXEC): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $@ $^

run_test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -f $(TEST_EXEC)

