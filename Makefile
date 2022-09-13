
## Copyright 2022 XTRABYTES
## Copyright 2022 Zoltan Szabo

## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## version 3 as published by the Free Software Foundation.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.


DIRS=build build/tests
$(shell mkdir -p $(DIRS))

TARGET = build/xots
TTARGET = build/test-xots

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include

LINK:=$(CXX)

DEFS +=-std=c++17
DEFS += $(addprefix -I,$(CURDIR) $(CURDIR)/build)
DEFS += -I$(CURDIR)/includes
DEFS += -I$(GOOGLE_TEST_INCLUDE)

LIBS += \
   -Wl,-Bdynamic \
	-L /usr/local/lib -l $(GOOGLE_TEST_LIB) \
	-l pthread \
   -l ssl \
   -l dl \
   -l PocoFoundation \
   -l PocoUtil \
   -l crypto 

## debug
CXXFLAGS=-pthread -Wall -Wextra -Wno-ignored-qualifiers -Wformat -Wformat-security -Wno-unused-parameter -g $(DEFS)

SOBJS = \
    build/utils.o \
    build/filesystem.o \
    build/xkeys.o
     

OBJS = $(SOBJS)
TOBJS = $(SOBJS)

OBJS += build/xots.o

TOBJS += \
	 build/tests/utils.o \
	 build/tests/xkeys.o \
	 build/tests/test-xots.o

all: $(TARGET)

tests: $(TTARGET)

# auto-generated dependencies:
-include build/*.P

DEFS += -DHAVE_BUILD_INFO

build/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

$(TARGET): $(OBJS:build/%=build/%)
	$(LINK) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -g $(LIBS)

$(TTARGET): $(TOBJS:build/%=build/%)
	$(LINK) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -g $(LIBS)

clean:
	-rm -f $(TARGET) 
	-rm -f build/*.o
	-rm -f build/*.P
	-rm -f $(TTARGET) 
	-rm -f build/tests/*.o
	-rm -f build/tests/*.P

FORCE:
