SRCS = simpleloop.c matmul.c blocked.c heaploop.c
PROGS = simpleloop matmul blocked heaploop

all : $(PROGS)

$(PROGS) : % : %.c
	gcc -Wall -g -o $@ $<


traces: $(PROGS)
	./runit simpleloop
	./runit matmul 100
	./runit blocked 100 25
	./runit heaploop

.PHONY: clean
clean : 
	rm -f simpleloop matmul blocked heaploop tr-*.ref *.marker *~
