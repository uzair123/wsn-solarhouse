CONTIKI_PROJECT = solarhouse-collect

all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifndef TARGET
  TARGET = z1
endif

ifndef CONTIKI
  CONTIKI = /usr/local/contiki-2.x
endif

include $(CONTIKI)/Makefile.include
