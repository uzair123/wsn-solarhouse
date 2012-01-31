CONTIKI_PROJECT = tikidb-shell

APPS = serial-shell scopes-tikidb tikidb 

all: $(CONTIKI_PROJECT)

ifndef TARGET
TARGET=sky
endif

# Enable project-specific configuration:
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

SRCS			+= base64.c shell-tikidb.c

PROJECT_OBJECTFILES	+= base64.o shell-tikidb.o
PROJECT_LIBRARIES	+= base64.o shell-tikidb.o 

ifndef CONTIKI
  CONTIKI = ../..
endif

include $(CONTIKI)/Makefile.include
