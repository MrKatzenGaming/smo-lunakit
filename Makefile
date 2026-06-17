.PHONY: debug clean
FTP_IP ?= 192.168.178.78 # put console IP here

DEBUGLOG ?= 1 # defaults to disable debug logger 

PROJNAME ?= smo-lunakit

debug: format
	cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=TRUE -DPROJNAME=$(PROJNAME) -DDEBUGLOG=$(DEBUGLOG) -S . -B build && $(MAKE) -C build

clean:
	rm -rf build

format:
	clear
	find src include lib/custom -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i
