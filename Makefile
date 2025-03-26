# Compiler et options
CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread -lstdc++fs

# Dossiers
SRC_DIR = Fonctions
BUILD_DIR = build
BIN = projet

# Fichiers sources et objets
SRCS = main.cpp $(SRC_DIR)/Scannerusb.cpp $(SRC_DIR)/detecterusb.cpp $(SRC_DIR)/affichercontenu.cpp
OBJS = $(SRCS:.cpp=.o)

# Règle principale
all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(BIN) $(OBJS)
