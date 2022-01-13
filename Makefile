bold := $(shell tput bold)
NC := $(shell tput sgr0)
red := $(shell tput setaf 1)
green := $(shell tput setaf 2)
yellow := $(shell tput setaf 3)
CC:=g++

HEADERS_DIR:= ./headers/
SRC_DIR:= ./src/

LIBS:= -limgui -lGL -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
SFML_DIR= ./Thirdparty/SFMLLinux/
IMGUI_DIR= ./Thirdparty/imgui/
LIBS_INC= -L$(SFML_DIR)lib/ -L$(IMGUI_DIR)BuildLib

SFML_INC= $(SFML_DIR)/include/

OBJECTS_DIR:= Objs/

OUTPUT:= Chess
OUTPUT_DIR:= ./Build/


SOURCES:= $(shell find $(SRC_DIR) -name  '*.cpp')
HEADERS_DIRS:=$(sort $(dir $(shell find $(HEADERS_DIR) -name '*.h')))

INC= $(addprefix -I, $(HEADERS_DIRS))
INC+= -I$(IMGUI_DIR)
CCFLAGS= $(INC) -std=c++17
OBJECTS:= $(patsubst %.cpp, %.o, $(subst ./src/, ./Objs/, $(SOURCES)))

VPATH_SRC:=$(sort $(dir $(VPATH_SRC)))

CREATE_DIRS:= $(OUTPUT_DIR) $(sort $(dir $(OBJECTS)))

vpath %.h $(HEADERS_DIR)
vpath %.cpp $(VPATH_SRC)

all: | prep
	@printf '$(red)Compiling and Linking the created objects\n$(NC)'
	@$(MAKE) -s $(OUTPUT_DIR)$(OUTPUT)
	@printf '$(green)Created binary file $(OUTPUT_DIR)$(OUTPUT)\n\n$(NC)'

$(OUTPUT_DIR)$(OUTPUT): $(OBJECTS)
	@$(CC) $(CCFLAGS) $(OBJECTS) -o $(OUTPUT_DIR)$(OUTPUT) $(LIBS_INC) $(LIBS)


prep:
	@mkdir -p $(CREATE_DIRS)
	@cd ./Thirdparty/imgui && $(MAKE) -s all

$(OBJECTS): $(OBJECTS_DIR)%.o : $(SRC_DIR)%.cpp	
	@printf '$(bold)========== Compiling $(red)$(notdir $^)$(NC)$(bold) ======\n$(NC)'
	@$(CC) -c $(CCFLAGS) $< -o $@
	@printf '$(green)========== Compiled Successfully ======\n$(NC)'

.PHONY: clean

clean:
	@$(RM) $(OBJECTS)
	@$(RM) $(OUTPUT_DIR)$(OUTPUT)
	@printf 'Successfully Cleaned\n'
	

print:
	@for obj in $(CREATE_DIRS); do\
		echo $$obj;\
	done