ifeq ($(OS), Windows_NT)
  RM = CMD /C del /Q /F
  RRM = CMD /C rmdir /Q /S
  BACKSLASH := \$(strip)
else
  RM = rm -f
  RRM = rm -f -r
	BACKSLASH := /$(strip)
endif

TARGET 				= 	$(CUR_DIR_NAME)
CC 						= 	gcc
CCFLAGS 			=
CXX 					= 	g++
CXXFLAGS 			= 	-g -Wall -Werror
LFLAGS				=		-pthread -ldl

DIRS					=		binaries 	\
									objects		\

SRC_DIR				=		./source
BIN_DIR 			=		./binaries
OBJ_DIR				=		./objects
INC_DIR				=		./include
MKF_PTH 			:= 	$(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR				:=	$(strip $(subst Makefile,, $(MKF_PTH)))
RUN_DIR				:= 	$(addprefix $(CUR_DIR), $(strip $(subst .\,, $(BIN_DIR))))
CUR_DIR_NAME	:= 	$(notdir $(patsubst %/,%,$(dir $(MKF_PTH))))
SRCS 					=		$(wildcard $(SRC_DIR)/*.cpp)
INCLUDES			=		-I$(INC_DIR)
OBJS 					=		$(subst $(SRC_DIR),$(OBJ_DIR),$(SRCS:.cpp=.o))

.PHONY: clean
.PHONY: directories
.PHONY: compileSQL
.PHONY: run
.PHONY: cmp

all: directories compileSQL $(BIN_DIR)/$(TARGET) run

cmp: directories compileSQL $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(OBJ_DIR)/sqlite3.o $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MD -MP $(INCLUDES) -c $< -o $@

compileSQL:
	$(CC) $(CCFLAGS) -MD -MP $(INCLUDES) -c $(SRC_DIR)/sqlite3.c -o $(OBJ_DIR)/sqlite3.o

directories: $(DIRS)

$(DIRS):
	mkdir $(DIRS)

clean:
	$(RRM) $(subst /,$(BACKSLASH), $(BIN_DIR))
	$(RRM) $(subst /,$(BACKSLASH), $(OBJ_DIR))

run:
	$(RUN_DIR)/$(TARGET)

-include $(OBJS:.o=.d)
