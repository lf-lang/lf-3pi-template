TEST_DIR := test
TEST_RES_DIR := $(TEST_DIR)/results

.PHONY: test
test:
	echo "--- Running Tests ---"

.PHONY: clean
clean:
	rm -rf src-gen/
	rm -rf bin/
	rm -rf include/
	rm -rf build/
	rm -rf target/
