EXECUTABLE=p3
CFLAGS= -Wall -g

shell: p3.c
	gcc $(CFLAGS) -o $(EXECUTABLE) p0.c p1.c p2.c p3.c file_list.c  hist_list.c mem_list.c proc_list.c dir_list.c Library.h 
