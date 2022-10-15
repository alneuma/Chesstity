###
LIBS = -lm
 
###
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpedantic
# CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_OUTPUT_COLOR

objects = main.o graphic_output.o core_functions.o core_interface.o
objects_test = tui_lib.o test_chess.o tui_test_lib.o ds_lib.o chess_test_creator.o core_functions.o unity.o graphic_output.o core_interface.o
headers_test = tui_lib.h tui_test_lib.h ds_lib.h chess_test_creator.h core_functions.h core_interface.h test-framework/unity/unity.h test-framework/unity/unity_chess_extension.h graphic_output.h

### main target
chess.x: $(objects) chess_test_creator.o
	cc $(CFLAGS) $(objects) chess_test_creator.o -o chess.x $(LIBS)

main.o: main.c core_functions.h graphic_output.h core_interface.h
	cc $(CFLAGS) -c main.c -o main.o $(LIBS)

core_interface.o: core_interface.c core_functions.h core_interface.h
	cc $(CFLAGS) -c core_interface.c -o core_interface.o $(LIBS)

core_functions.o: core_functions.c core_functions.h
	cc $(CFLAGS) -c core_functions.c -o core_functions.o $(LIBS)
	
graphic_output.o: graphic_output.c core_functions.h graphic_output.h
	cc $(CFLAGS) -c graphic_output.c -o graphic_output.o $(LIBS)

ds_lib.o: ds_lib.c ds_lib.h mem_utilities.h
	cc $(CFLAGS) -c ds_lib.c -o ds_lib.o $(LIBS)

tui_test_lib.o: tui_test_lib.c tui_lib.h tui_test_lib.h mem_utilities.h
	cc $(CFLAGS) -c tui_test_lib.c -o tui_test_lib.o $(LIBS)

tui_lib.o: tui_lib.c tui_lib.h mem_utilities.h
	cc $(CFLAGS) -c tui_lib.c -o tui_lib.o $(LIBS)

test_chess.o: test_chess.c $(headers_test)
	cc $(CFLAGS) -c test_chess.c -o test_chess.o $(LIBS)

unity.o: test-framework/unity/unity.c test-framework/unity/unity.h test-framework/unity/unity_internals.h test-framework/unity/unity_chess_extension.h
	cc $(CFLAGS) -c test-framework/unity/unity.c -o unity.o $(LIBS)

chess_test_creator.o: chess_test_creator.c chess_test_creator.h core_functions.h graphic_output.h core_interface.h
	cc $(CFLAGS) -c chess_test_creator.c -o chess_test_creator.o $(LIBS) 

test.out: $(objects_test)
	cc $(CFLAGS) $(objects_test) -o test.out $(LIBS)

.PHONY: test
test: test.out
	./test.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.x
