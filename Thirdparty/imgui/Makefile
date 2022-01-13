
bold := $(shell tput bold)
NC := $(shell tput sgr0)
green := $(shell tput setaf 2)
red := $(shell tput setaf 1)

CC:=g++
SRC_DIR:= ./

OUTPUT:= libimgui.so
BUILD_DIR = ./BuildLib/
SOURCES:= $(wildcard *.cpp)
OBJECTS:= $(patsubst %.cpp, %.o, $(addprefix $(BUILD_DIR)Objs/, $(SOURCES)))
OBJECTS_DIR=./Objs
DIRS = $(BUILD_DIR) $(BUILD_DIR)Objs/ $(sort $(dir $(OBJECTS)))
INC = -I./
CCFLAGS:= $(INC) -std=c++17

vpath %.h ./include/
vpath %.cpp $(VPATH_SRC)
VPATH_SRC:=$(sort $(dir $(VPATH_SRC)))

all: | prep
	@printf '$(red)Compiling ImGui to a static library\n$(NC)'
	@$(MAKE) -s $(BUILD_DIR)$(OUPTUT)
	@printf '$(green)Successfully Created $(OUTPUT)\n\n$(NC)'

$(BUILD_DIR)$(OUPTUT): $(OBJECTS)
	@ar rvs ./BuildLib/$(OUTPUT) $(OBJECTS)
	

prep:
	@mkdir -p $(DIRS)

$(OBJECTS): $(BUILD_DIR)Objs/%.o : $(SRC_DIR)%.cpp	
	@printf '$(bold)========== Compiling $(red)$(notdir $^)$(NC)$(bold) ======\n$(NC)'
	@$(CC) -c $(CCFLAGS) $< -o $@
	@printf '$(green)========== Compiled Successfully ======\n$(NC)'
	

.PHONY: clean

clean:
	@$(RM) $(OBJECTS)
	@$(RM) $(OUTPUT_DIR)$(OUTPUT)
	@printf 'Successfully Cleaned\n'

print:
	@for obj in $(OBJECTS); do\
		echo $$obj;\
	done