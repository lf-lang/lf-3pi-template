TEST_DIR := test
TEST_RES_DIR := $(TEST_DIR)/results

SRCS = $(wildcard src/*.lf)
ELFS = $(patsubst src/%.lf, build/%.elf, $(SRCS))

build/%.elf: src/%.lf
	./run/build.sh -m $^

.PHONY: test
test: ${ELFS}
	echo "--- Running Tests ---"

.PHONY: clean
clean:
	rm -rf src-gen/
	rm -rf bin/
	rm -rf include/
	rm -rf build/
	rm -rf target/