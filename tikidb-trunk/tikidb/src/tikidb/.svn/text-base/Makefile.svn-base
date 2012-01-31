CONTIKI_PROJECT = tester/tester

APPS = scopes-tikidb

all: $(CONTIKI_PROJECT)

ifndef TARGET
  TARGET = sky
endif

# Enable project-specific configuration:
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

SRCS                += table.c tikidb-buffer.c resultset.c dbms-operations.c tikidb-mgr.c statement.c queryresolver.c queryresolver-util.c tikidb.c 

PROJECT_OBJECTFILES += table.o tikidb-buffer.o resultset.o dbms-operations.o tikidb-mgr.o statement.o queryresolver.o queryresolver-util.o tikidb.o  
PROJECT_LIBRARIES   += table.o tikidb-buffer.o resultset.o dbms-operations.o tikidb-mgr.o statement.o queryresolver.o queryresolver-util.o tikidb.o

ifndef CONTIKI
  CONTIKI = ../..
endif

include $(CONTIKI)/Makefile.include
