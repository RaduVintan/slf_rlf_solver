all: sdn

SOURCE = hlb.h local_search.h lp_optimizer.h generator.h hlb.cpp local_search.cpp lp_optimizer.cpp generator.cpp peacock.cpp peacock.h utilities.h utilities.cpp sdn.cpp 

utilities: utilities.h utilities.cpp
	g++ -c utilities.cpp -o $@

hlb: hlb.h hlb.cpp
	g++ -c hlb.cpp -o $@

local_search: local_search.h local_search.cpp
	g++ -c local_search.cpp -o $@

lp_optimizer: lp_optimizer.h lp_optimizer.cpp
	g++ -c lp_optimizer.cpp -o $@

generator: generator.h generator.cpp
	g++ -c generator.cpp -o $@

peacock: peacock.h peacock.cpp
	g++ -c peacock.cpp -o $@

sdn: utilities hlb local_search lp_optimizer generator peacock utilities.o hlb.o local_search.o lp_optimizer.o generator.o peacock.o sdn.cpp
	g++ sdn.cpp hlb.o local_search.o utilities.o lp_optimizer.o generator.o peacock.o -o $@
