CXX = clang++
CXXFLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp -O3
TARGET = main
SRC = main.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
# ^ Tu musi być TAB, nie spacje!

run: all
	./$(TARGET)
# ^ Tu musi być TAB

clean:
	rm -f $(TARGET)
# ^ Tu musi być TAB
