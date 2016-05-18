all: routing_test
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