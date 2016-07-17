OBJS = main.o words.o lexer.o

htfc: $(OBJS)
	g++ $(OBJS) -o htfc

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.cc
	g++ -c $(CFLAGS) $*.cc -o $*.o
	g++ -MM $(CFLAGS) $*.cc > $*.d

clean:
	rm -f *.exe *.o *.d
