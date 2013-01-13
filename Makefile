OBJS = main.o gameboard.o image.o 
CFLAGS = `sdl-config --cflags`
LIBS = `sdl-config --libs` -lm

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o nonogramu

%o:%c
	gcc $(CLAGS) $< -c

clean:
	rm -f $(OBJS) nonogramu
