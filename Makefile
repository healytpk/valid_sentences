prog.exe : main.cpp generator.hpp
	$(CXX) -o $@ -std=c++20 -fcoroutines -Wall -pedantic main.cpp
