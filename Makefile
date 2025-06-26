objects = main.o lexer.o parser.o


a.out: $(objects)  lexer.h parser.h
	@echo $^
	g++ $(objects)

%.h:
	@echo $@

$(objects): %.o: %.cpp
	g++ -c $^

clean:
	@rm *.o *.out
	@echo "Removed object files and executables"
