CONTIKI_PROJECT = wrapper

ifndef CONTIKI
CONTIKI = ../..
endif

CFLAGS += -Wall -DDEBUG
#CFLAGS += -DPROJECT_CONF_H=1
#CFLAGS += -DSCOPES_NDEBUG
#CFLAGS += -DDEBUG_DB
#CFLAGS += -DFRAGMENTATION_ENABLED
APPS = sensing scopes tikidb

ifndef TARGET
TARGET=sky
endif

OBJECTDIR = obj_$(TARGET)

FOLDER1 = $(OBJECTDIR)/database
FOLDER2 = $(OBJECTDIR)/querymanager
FOLDER3 = $(OBJECTDIR)/queryresolver
FOLDER4 = $(OBJECTDIR)/test
FOLDER5 = $(OBJECTDIR)/utils

#TIKIDB_PATH = ../tikidb
all:
	if test -d $(FOLDER1); then echo ""; else mkdir -p $(FOLDER1); fi
	if test -d $(FOLDER2); then echo ""; else mkdir -p $(FOLDER2); fi
	if test -d $(FOLDER3); then echo ""; else mkdir -p $(FOLDER3); fi
	if test -d $(FOLDER4); then echo ""; else mkdir -p $(FOLDER4); fi
	if test -d $(FOLDER5); then echo ""; else mkdir -p $(FOLDER5); fi
	if test -d $(FOLDER6); then echo ""; else mkdir -p $(FOLDER6); fi
	if test -d $(FOLDER7); then echo ""; else mkdir -p $(FOLDER7); fi
	make $(CONTIKI_PROJECT)


#include ../scopes/Makefile.scopes
#include ../tikidb/Makefile
	
include $(CONTIKI)/Makefile.include
	
