ifndef CONTIKI
  CONTIKI = ../..
endif

CONTIKI_PROJECT = tester/tester
all: $(CONTIKI_PROJECT)

SRCS += scopes.c scopes-selfur.c scopes-membership-simple.c
# scopes-flooding.c 

PROJECT_OBJECTFILES += frag_unicast.o scopes.o scopes-selfur.o scopes-membership-simple.o
PROJECT_LIBRARIES += frag_unicast.o scopes.o scopes-selfur.o scopes-membership-simple.o

CFLAGS += -I$(CONTIKI)/tud/TikiDB/scopes

include $(CONTIKI)/Makefile.include
