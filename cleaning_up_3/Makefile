TITLE_COLOR = \033[33m
NO_COLOR = \033[0m

# when executing make, compile all exe's
all: sensor_gateway sensor_node file_creator

# When trying to compile one of the executables, first look for its .c files
# Then check if the libraries are in the lib folder
sensor_gateway : main.c connmgr.c datamgr.c sensor_db.c sbuffer.c lib/libdplist.so lib/libtcpsock.so
	@echo "$(TITLE_COLOR)\n***** CPPCHECK *****$(NO_COLOR)"
	cppcheck --enable=all --suppress=missingIncludeSystem main.c connmgr.c datamgr.c sensor_db.c sbuffer.c
	@echo "$(TITLE_COLOR)\n***** COMPILING sensor_gateway *****$(NO_COLOR)"
	gcc -c main.c      -Wall -Werror -DSET_MIN_TEMP=10 -lpthread -DSET_MAX_TEMP=20 -DTIMEOUT=5 -o main.o      -fdiagnostics-color=auto
	gcc -c connmgr.c   -Wall -Werror -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -o connmgr.o   -fdiagnostics-color=auto
	gcc -c datamgr.c   -Wall -Werror -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -o datamgr.o   -fdiagnostics-color=auto
	gcc -c sensor_db.c -Wall -Werror -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -o sensor_db.o -fdiagnostics-color=auto
	gcc -c sbuffer.c   -Wall -std=c11 -Werror -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5 -o sbuffer.o   -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING sensor_gateway *****$(NO_COLOR)"
	gcc main.o connmgr.o datamgr.o sensor_db.o sbuffer.o -ldplist -ltcpsock -lpthread -o sensor_gateway -Wall -L./lib -Wl,-rpath=./lib -lsqlite3 -fdiagnostics-color=auto

file_creator : file_creator.c
	@echo "$(TITLE_COLOR)\n***** COMPILE & LINKING file_creator *****$(NO_COLOR)"
	gcc file_creator.c -o file_creator -Wall -fdiagnostics-color=auto

sensor_node : sensor_node.c lib/libtcpsock.so
	@echo "$(TITLE_COLOR)\n***** COMPILING sensor_node *****$(NO_COLOR)"
	gcc -c sensor_node.c -Wall  -o sensor_node.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING sensor_node *****$(NO_COLOR)"
	gcc sensor_node.o -ltcpsock -o sensor_node -Wall -L./lib -Wl,-rpath=./lib -fdiagnostics-color=auto

# If you only want to compile one of the libs, this target will match (e.g. make liblist)
libdplist : lib/libdplist.so
libsbuffer : lib/libsbuffer.so
libtcpsock : lib/libtcpsock.so

lib/libdplist.so : lib/dplist.c
	@echo "$(TITLE_COLOR)\n***** COMPILING LIB dplist *****$(NO_COLOR)"
	gcc -c lib/dplist.c -Wall -std=c11 -Werror -fPIC -o lib/dplist.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING LIB dplist< *****$(NO_COLOR)"
	gcc lib/dplist.o -o lib/libdplist.so -Wall -shared -lm -fdiagnostics-color=auto

lib/libsbuffer.so : lib/sbuffer.c
	@echo "$(TITLE_COLOR)\n***** COMPILING LIB sbuffer *****$(NO_COLOR)"
	gcc -c lib/sbuffer.c -Wall -std=c11 -Werror -fPIC -o lib/sbuffer.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING LIB sbuffer *****$(NO_COLOR)"
	gcc lib/sbuffer.o -o lib/libsbuffer.so -Wall -shared -lm -fdiagnostics-color=auto

lib/libtcpsock.so : lib/tcpsock.c
	@echo "$(TITLE_COLOR)\n***** COMPILING LIB tcpsock *****$(NO_COLOR)"
	gcc -c lib/tcpsock.c -Wall -std=c11 -Werror -fPIC -o lib/tcpsock.o -fdiagnostics-color=auto
	@echo "$(TITLE_COLOR)\n***** LINKING LIB tcpsock *****$(NO_COLOR)"
	gcc lib/tcpsock.o -o lib/libtcpsock.so -Wall -shared -lm -fdiagnostics-color=auto

# do not look for files called clean, clean-all or this will be always a target
.PHONY : clean clean-all 

clean:
	rm -rf *.o sensor_gateway sensor_node file_creator *~ 

clean-all: clean
	rm -rf lib/*.so

run : sensor_gateway sensor_node
	@echo "$(TITLE_COLOR)\n***** Make is done.Now running your server *****$(NO_COLOR)"
	./sensor_gateway 1234
	

