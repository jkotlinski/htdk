OBJS = main.o words.o scanner.o

htfc: $(OBJS)
	g++ $(OBJS) --std=c++14 -o htfc

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.cc
	g++ --std=c++14 -c $(CFLAGS) $*.cc -o $*.o
	g++ --std=c++14 -MM $(CFLAGS) $*.cc > $*.d

clean:
	rm -f *.exe *.o *.d
