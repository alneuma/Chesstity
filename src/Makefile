###
LIBS = -lm
 
###
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpedantic
# CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_OUTPUT_COLOR

objects = main.o graphic_output.o core_functions.o
objects_test = test_chess.o chess_test_creator.o core_functions.o unity.o graphic_output.o

### main target
chess.x: $(objects) chess_test_creator.o
	cc $(CFLAGS) $(objects) chess_test_creator.o -o chess $(LIBS)

main.o: main.c core_functions.h graphic_output.h
	cc $(CFLAGS) -c main.c -o main.o $(LIBS)

core_functions.o: core_functions.c core_functions.h
	cc $(CFLAGS) -c core_functions.c -o core_functions.o $(LIBS)
	
graphic_output.o: graphic_output.c core_functions.h graphic_output.h
	cc $(CFLAGS) -c graphic_output.c -o graphic_output.o $(LIBS)

test_chess.o: test_chess.c chess_test_creator.h core_functions.h test-framework/unity/unity.h test-framework/unity/unity_chess_extension.h graphic_output.h
	cc $(CFLAGS) -c test_chess.c -o test_chess.o $(LIBS)

unity.o: test-framework/unity/unity.c test-framework/unity/unity.h test-framework/unity/unity_internals.h test-framework/unity/unity_chess_extension.h
	cc $(CFLAGS) -c test-framework/unity/unity.c -o unity.o $(LIBS)

chess_test_creator.o: chess_test_creator.c chess_test_creator.h core_functions.h
	cc $(CFLAGS) -c chess_test_creator.c -o chess_test_creator.o $(LIBS) 

tests.out: $(objects_test)
	cc $(CFLAGS) $(objects_test) -o tests.out $(LIBS)

.PHONY: test
test: tests.out
	./tests.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.x
