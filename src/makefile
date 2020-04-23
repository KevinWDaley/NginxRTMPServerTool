nsst: main.o nginx-process-id.o clean
	g++ main.o nginx-process-id.o -lncurses -lmenu -o nsst

main.o: main.cpp
	g++ -c main.cpp

nginx-process-id.o: nginx-process-id.cpp
	g++ -c nginx-process-id.cpp

.PHONY: clean
clean:
	mv *.o nsst ../bin/
