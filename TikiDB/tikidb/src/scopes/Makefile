CONTIKI_PROJECT = tester/tester



all: $(CONTIKI_PROJECT)


ifndef TARGET
  TARGET = sky
endif

# Enable project-specific configuration:
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

SRCS                += frag_unicast.o scopes.c scopes-selfur.c scopes-membership-simple.c scopes-repository.c # scopes-flooding.c 

PROJECT_OBJECTFILES += frag_unicast.o scopes.o scopes-selfur.o scopes-membership-simple.o scopes-repository.o # scopes-flooding.c
PROJECT_LIBRARIES   += frag_unicast.o scopes.o scopes-selfur.o scopes-membership-simple.o scopes-repository.o # scopes-flooding.c

ifndef CONTIKI
  CONTIKI = ../..
endif

include $(CONTIKI)/Makefile.include