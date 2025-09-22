
SHELL := /bin/bash
CXX := c++
CXXFLAGS := -std=c++20 -Wall -O2 -pthread

OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl@3)
BOOST_PREFIX   := $(shell brew --prefix boost       2>/dev/null || echo /opt/homebrew/opt/boost)
JSON_PREFIX    := $(shell brew --prefix nlohmann-json 2>/dev/null || echo /opt/homebrew/opt/nlohmann-json)

INCLUDES := -I. -I$(BOOST_PREFIX)/include -I$(OPENSSL_PREFIX)/include -I$(JSON_PREFIX)/include
LIBS     := -L$(OPENSSL_PREFIX)/lib -lssl -lcrypto -lpthread

SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)
BIN := l2_recorder

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(BIN) *.o

