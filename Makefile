all:
	g++ src/DocumentXML.cpp src/Histoire.cpp src/resume.cpp src/Phrase.cpp -lm -std=c++11  -o resume
