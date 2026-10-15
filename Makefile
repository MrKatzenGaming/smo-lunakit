.PHONY: debug clean format check_includes
 
CMAKE_FLAGS =	-G Ninja \
				-DCMAKE_C_COMPILER_LAUNCHER=ccache \
				-DCMAKE_CXX_COMPILER_LAUNCHER=ccache

debug: format
	cmake $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=Debug -S . -B build && cmake --build build

clean:
	rm -rf build

format:
	-clear
	find src include lib/custom -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \) -print0 | xargs --null clang-format -i

check_includes:
	-clear
	python3 check_includes.py

check: check_includes format