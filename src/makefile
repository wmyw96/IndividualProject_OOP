all: editor

test: routing_test

clean:
	rm *.exe *.o

editor: editor.o demo.o message.o routing.o routingalgo.o
	g++ editor.o demo.o message.o routing.o routingalgo.o -lz3 -o editor

editor.o: editor.cpp demo.h routing.h routingalgo.h message.h
	g++ -c editor.cpp -o editor.o

demo.o: demo.h demo.cpp routing.h routingalgo.h message.h
	g++ -c demo.cpp -o demo.o

routing_test: routing_test.o message.o routing.o routingalgo.o
	g++ routing_test.o message.o routing.o routingalgo.o -lz3 -o routing_test

routing.o: routing.cpp routing.h routingalgo.h message.h
	g++ -c routing.cpp -o routing.o

routingalgo.o: routingalgo.cpp routingalgo.h message.h
	g++ -c routingalgo.cpp -o routingalgo.o

message.o: message.h message.cpp
	g++ -c message.cpp -o message.o

routing_test.o: routing_test.cpp
	g++ -c routing_test.cpp -o routing_test.o