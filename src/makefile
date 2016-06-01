edit: editor

repo: report

test: routing_test

clean:
	rm *.exe *.o

report: report.o message.o routing.o routingalgo.o checker.o evaluate.o
	g++ report.o message.o routing.o routingalgo.o checker.o evaluate.o -lz3 -o report

editor: editor.o demo.o message.o routing.o routingalgo.o
	g++ editor.o demo.o message.o routing.o routingalgo.o -lz3 -o editor

editor.o: editor.cpp demo.h routing.h routingalgo.h message.h
	g++ -c editor.cpp -o editor.o

demo.o: demo.h demo.cpp routing.h routingalgo.h message.h
	g++ -c demo.cpp -o demo.o

routing_test: routing_test.o message.o routing.o routingalgo.o checker.o
	g++ routing_test.o message.o routing.o routingalgo.o checker.o -lz3 -o routing_test

routing.o: routing.cpp routing.h routingalgo.h message.h
	g++ -c routing.cpp -o routing.o

routingalgo.o: routingalgo.cpp routingalgo.h message.h
	g++ -c routingalgo.cpp -o routingalgo.o

message.o: message.h message.cpp
	g++ -c message.cpp -o message.o

routing_test.o: routing_test.cpp routing.h routingalgo.h message.h checker.h
	g++ -c routing_test.cpp -o routing_test.o

checker.o: checker.cpp checker.h message.h
	g++ -c checker.cpp -o checker.o

evaluate.o: evaluate.cpp message.h checker.h routing.h routingalgo.h evaluate.h
	g++ -c evaluate.cpp -o evaluate.o

report.o: report.cpp message.h checker.h routing.h routingalgo.h evaluate.h
	g++ -c report.cpp -o report.o 