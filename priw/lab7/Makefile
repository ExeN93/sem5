CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -march=native
LDFLAGS = -ltbb

TARGET = mandelbrot

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) $(LDFLAGS)

clean:
	# Usuwa plik wykonywalny i wygenerowane obrazki ppm
	rm -f $(TARGET) $(TARGET).exe *.ppm