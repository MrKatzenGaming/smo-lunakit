FTP_IP ?= 192.168.178.78 # put console IP here
.PHONY: debug all clean

# all:
# 	cmake --toolchain=cmake/toolchain.cmake -S . -B build && $(MAKE) -C build subsdk9_meta

# send: format
# 	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

DEBUGLOG ?= 1 # defaults to disable debug logger 

PROJNAME ?= smo-lunakit

debug: format
	cmake -DCMAKE_BUILD_TYPE=Debug -DDEBUG=TRUE -DPROJNAME=$(PROJNAME) -DDEBUGLOG=$(DEBUGLOG) -S . -B build && $(MAKE) -C build


clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0

format:
	clear || true
	find ./src -name "*.*" | xargs clang-format -i || true
	find ./include -name "*.*" | xargs clang-format -i || true
	find ./lib/custom -name "*.*" | xargs clang-format -i || true
