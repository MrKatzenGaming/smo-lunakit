.PHONY: debug clean format check_includes

DEBUGLOG ?= 1 

debug: format
	cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUGLOG=$(DEBUGLOG) -S . -B build && $(MAKE) -C build

clean:
	rm -rf build

format:
	-clear
	find src include lib/custom -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i

check_includes:
	-clear
	python3 check_includes.py

check: check_includes format