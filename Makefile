.PHONY: clean

default: valtlb379.c mergesort.c quicksort.c heapsort.c bubblesort.c
	gcc -std=c99 -o valtlb379 valtlb379.c -lm
	gcc -std=c99 -o mergesort mergesort.c
	gcc -std=c99 -o quicksort quicksort.c
	gcc -std=c99 -o heapsort heapsort.c
	gcc -std=c99 -o bubblesort bubblesort.c

clean:
	rm -rf valtlb379 *.o
	rm -rf mergesort *.o
	rm -rf quicksort *.o
	rm -rf heapsort *.o
	rm -rf bubblesort *.o
