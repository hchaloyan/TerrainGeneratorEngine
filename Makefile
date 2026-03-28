CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET   := terrain
SRCS     := src/main.cpp src/parser.cpp src/renderer.cpp third_party/stbImageWrite.cpp src/hillshade.cpp
OBJS     := $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)