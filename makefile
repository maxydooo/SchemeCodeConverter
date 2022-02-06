P3.out : Project3.o Set_Limits.o Lexical_Analyzer.o Syntactical_Analyzer.o Code_Generator.o
	g++ -g -o P3.out Project3.o Set_Limits.o Lexical_Analyzer.o Syntactical_Analyzer.o Code_Generator.o

Project3.o : Project3.cpp Set_Limits.h Syntactical_Analyzer.h
	g++ -g -c Project3.cpp

Set_Limits.o : Set_Limits.cpp Set_Limits.h
	g++ -g -c Set_Limits.cpp

Lexical_Analyzer.o : Lexical_Analyzer.o.save
	cp Lexical_Analyzer.o.save Lexical_Analyzer.o

Syntactical_Analyzer.o : Syntactical_Analyzer.cpp Syntactical_Analyzer.h Lexical_Analyzer.h
	g++ -g -c Syntactical_Analyzer.cpp

Code_Generator.o : Code_Generator.cpp Code_Generator.h Lexical_Analyzer.h
	g++ -g -c Code_Generator.cpp

clean : 
	rm *.o P3.out *.gch

submit : Project3.cpp Lexical_Analyzer.h Syntactical_Analyzer.h Syntactical_Analyzer.cpp makefile README.txt
	rm -rf lankfordP3
	mkdir lankfordP3
	cp Project3.cpp lankfordP3
	cp Syntactical_Analyzer.h lankfordP3
	cp Syntactical_Analyzer.cpp lankfordP3
	cp Code_Generator.h lankfordP3
	cp Code_Generator.cpp lankfordP3
	cp makefile lankfordP3
	cp README.txt lankfordP3
	tar cfvz lankfordP3.tgz lankfordP3
	cp lankfordP3.tgz ~tiawatts/cs460drop
