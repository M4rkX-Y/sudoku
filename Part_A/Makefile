OBJS = d_matrix.o d_except.o main.o

all: $(OBJS)
	g++ -o main $(OBJS)

%o: %.h %.cpp
	g++ -c $<

.PHONY: clean

clean:
	rm -f *.o main