
all:
	gcc perf_counter.c pc_test.c -lpthread -o testpc
bgt:
	gcc perf_counter.c pc_test.c -lpthread -Dbgtime -o testpc
clean:
	rm -f testpc
