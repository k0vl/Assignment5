all: life life_debug validate 

life_debug: life.cpp submit.cpp 
	icc -DDEBUG=1 -o life_debug life.cpp submit.cpp -g

life: life.cpp submit.cpp
	icc -DDEBUG=0 -o life life.cpp submit.cpp -g

validate: validate.cpp
	g++ -o validate validate.cpp	
clean: 
	rm life validate life_debug
