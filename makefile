CC  :=  g++
CFLAG := -lm -std=c++11
SRC := ./src
OBJ := ./obj
INC := ./include
EXE := tree
GDB := -g


tree : main.o $(OBJ)/treeBitOperations.o $(OBJ)/timePushDown.o $(OBJ)/continuoustpda.o $(OBJ)/pds.o $(OBJ)/tpdaCGPP.o $(OBJ)/tpdaZone.o $(OBJ)/tpda2.o drawsystem
	g++ $(GDB) main.o $(OBJ)/treeBitOperations.o $(OBJ)/timePushDown.o $(OBJ)/continuoustpda.o $(OBJ)/pds.o $(OBJ)/tpdaCGPP.o $(OBJ)/tpdaZone.o $(OBJ)/tpda2.o -o $(EXE) $(CFLAG)

main.o : main.cpp
	g++ $(GDB) -I $(INC) -c main.cpp -o main.o $(CFLAG)
	
$(OBJ)/tpdaZone.o : $(SRC)/tpdaZone.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/tpdaZone.cpp -o $(OBJ)/tpdaZone.o $(CFLAG)
	
$(OBJ)/tpdaCGPP.o : $(SRC)/tpdaCGPP.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/tpdaCGPP.cpp -o $(OBJ)/tpdaCGPP.o $(CFLAG)
	
$(OBJ)/continuoustpda.o : $(SRC)/continuoustpda.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/continuoustpda.cpp -o $(OBJ)/continuoustpda.o $(CFLAG)
	
$(OBJ)/tpda2.o : $(SRC)/tpda2.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/tpda2.cpp -o $(OBJ)/tpda2.o $(CFLAG)
	
$(OBJ)/pds.o : $(SRC)/pds.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/pds.cpp -o $(OBJ)/pds.o $(CFLAG)

drawsystem : $(OBJ)/drawsystem.o $(OBJ)/treeBitOperations.o
	g++ $(GDB) $(OBJ)/drawsystem.o $(OBJ)/treeBitOperations.o -o drawsystem $(CFLAG)
	
$(OBJ)/drawsystem.o : $(SRC)/drawsystem.cpp 
	g++ $(GDB) -I $(INC) -c $(SRC)/drawsystem.cpp -o $(OBJ)/drawsystem.o $(CFLAG)	
	
$(OBJ)/timePushDown.o : $(SRC)/timePushDown.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/timePushDown.cpp -o $(OBJ)/timePushDown.o $(CFLAG)
	
$(OBJ)/treeBitOperations.o : $(SRC)/treeBitOperations.cpp
	g++ $(GDB) -I $(INC) -c $(SRC)/treeBitOperations.cpp -o $(OBJ)/treeBitOperations.o $(CFLAG)
