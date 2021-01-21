#
# In order to execute this "Makefile" just type "make"
#
#

OBJS 	= main.o jsonStruct.o jsonParcer.o hashTable.o graph.o bagOfWords.o logisticRegression.o metrics.o jobScheduler.o red-black.o
OBJS1   = unit_test.o jsonStruct.o jsonParcer.o hashTable.o graph.o bagOfWords.o logisticRegression.o metrics.o jobScheduler.o red-black.o
SOURCE	= main.c jsonStruct.c jsonParcer.c hashTable.c graph.c bagOfWords.c metrics.c jobScheduler.c red-black.c
SOURCE1	= unit_test.c jsonStruct.c jsonParcer.c hashTable.c graph.c bagOfWords.c logisticRegression.c metrics.c jobScheduler.c red-black.c
HEADER  = jsonStruct.h acutest.h hashTable.h graph.h bagOfWords.h logisticRegression.h metrics.h jobScheduler.h red-black.h
OUT  	= project2
OUTTEST = testing
CC	= gcc
FLAGS   = -g -c -Wall -lm -pthread
LFLAGS	= -lm -pthread
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS) $(OBJS1)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUTTEST) $(LFLAGS)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

jsonStruct.o: jsonStruct.c
	$(CC) $(FLAGS) jsonStruct.c

jsonParcer.o: jsonParcer.c
	$(CC) $(FLAGS) jsonParcer.c

unit_test.o: unit_test.c 
	$(CC) $(FLAGS) unit_test.c

hashTable.o: hashTable.c
	$(CC) $(FLAGS) hashTable.c

graph.o: graph.c
	$(CC) $(FLAGS) graph.c

bagOfWords.o: bagOfWords.c
	$(CC) $(FLAGS) bagOfWords.c

logisticRegression.o: logisticRegression.c
	$(CC) $(FLAGS) logisticRegression.c

metrics.o: metrics.c
	$(CC) $(FLAGS) metrics.c

jobScheduler.o: jobScheduler.c
	$(CC) $(FLAGS) jobScheduler.c

red-black.o: red-black.c
	$(CC) $(FLAGS) red-black.c


# clean house
clean:
	rm -f $(OBJS) $(OBJS1) $(OUT) $(OUTTEST)

# do a bit of accounting
count:
	wc $(SOURCE) $(SOURCE1) $(HEADER)

test:
	./testing

run:
	./$(OUT) -x Datasets -c sigmod_large_labelled_dataset.csv

#debug the program for errors using the gdb debugger
debug:
	gdb --args ./$(OUT) -x Datasets -c sigmod_large_labelled_dataset.csv

# compile program with debugging information
valgrind: $(OUT)
	valgrind ./$(OUT) -x Datasets -c sigmod_medium_labelled_dataset.csv

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full ./$(OUT) -x Datasets -c sigmod_medium_labelled_dataset.csv

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes ./$(OUT) -x Datasets -c sigmod_medium_labelled_dataset.csv

test_leakcheck:
	valgrind --tool=memcheck ./testing
