CONTIKI_PROJECT = distrib
all: $(CONTIKI_PROJECT)

CONTIKI = /home/wsn/SVN
include $(CONTIKI)/Makefile.include

#DEFINES=ELFLOADER_DATAMEMORY_SIZE=0x1000,ELFLOADER_TEXTMEMORY_SIZE=22500
DEFINES=ELFLOADER_TEXTMEMORY_SIZE=7000

%.shell-upload: %.ce
	(echo; sleep 4; echo "~K"; sleep 4; \
         echo "dec64 | write firmware | null"; sleep 4; \
	 /opt/contiki-2.x/tools/base64-encode < $<; sleep 4; \
         echo ""; echo "~K"; echo "read firmware | size"; sleep 4) | make login
         
%.new-upload: %.ce
	(echo; sleep 4; echo "~K"; sleep 2; \
	echo "rm encoded"; sleep 2; \
         echo "write encoded | null"; sleep 4; \
	 /opt/contiki-2.x/tools/base64-encode < $<; sleep 60; \
         echo "~K"; ) | make login

compile-executable: hello-world.ce
	echo Compiled Contiki executable: $<
	
upload-executable: hello-world.shell-upload
	echo Uploaded Contiki executable: $<
