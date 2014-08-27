#CC = arm-linux-gnueabihf-gcc
#CXX =  arm-linux-gnueabihf-g++

CPP_FILES += $(wildcard *.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
#INCLUDES := -I . -I ../alsa-lib/include
#INCLUDES := -I . -I $(SDKTARGETSYSROOT)/usr/src/debug/alsa-lib/1.0.27.2-r0/alsa-lib-1.0.27.2/include/
CC_FLAGS += -MMD $(INCLUDES)

#LD_FLAGS += -L/home/dvincelette/projects/beagle/alsa-lib/src/.libs -l asound
#LD_FLAGS += -L$(SDKTARGETSYSROOT)/usr/lib -l asound

all:	stamps obj mt_test

stamps:
	touch main.cpp

mt_test: $(OBJ_FILES)
	echo $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) -o $@ $^

obj:
	mkdir -p obj

obj/%.o: %.cpp
	$(CXX) $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf obj

-include $(OBJ_FILES:.o=.d)
