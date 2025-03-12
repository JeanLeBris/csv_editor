compile:csv_editor.exe



################################################################################
# Compilation process for Linux OS                                             #
################################################################################

# csv_editor.exe:obj/main.o obj/config.o obj/colors.o obj/table.o obj/actions.o obj
# 	gcc -Wall obj/main.o obj/config.o obj/colors.o obj/table.o obj/actions.o -o csv_editor.exe -lncurses

# obj/main.o:src/main.c obj
# 	gcc -Wall -c src/main.c -o obj/main.o -lncurses

# obj/config.o:src/config.c obj
# 	gcc -Wall -c src/config.c -o obj/config.o -lncurses

# obj/colors.o:src/colors.c obj
# 	gcc -Wall -c src/colors.c -o obj/colors.o -lncurses

# obj/table.o:src/table.c obj
# 	gcc -Wall -c src/table.c -o obj/table.o -lncurses

# obj/actions.o:src/actions.c obj
# 	gcc -Wall -c src/actions.c -o obj/actions.o -lncurses

# obj:
# 	mkdir obj



################################################################################
# Compilation process for Windows OS                                           #
################################################################################

csv_editor.exe:obj/main.o obj/config.o obj/colors.o obj/table.o obj/actions.o obj
	gcc -Wall obj/main.o obj/config.o obj/colors.o obj/table.o obj/actions.o -o csv_editor.exe

obj/main.o:src/main.c obj
	gcc -Wall -c src/main.c -o obj/main.o

obj/config.o:src/config.c obj
	gcc -Wall -c src/config.c -o obj/config.o

obj/colors.o:src/colors.c obj
	gcc -Wall -c src/colors.c -o obj/colors.o

obj/table.o:src/table.c obj
	gcc -Wall -c src/table.c -o obj/table.o

obj/actions.o:src/actions.c obj
	gcc -Wall -c src/actions.c -o obj/actions.o

obj:
	mkdir obj



################################################################################
# Supplementary tools                                                          #
################################################################################

clean:
	rm -r obj

clear:clean
	rm *.exe

zip:
	zip "csv_editor.zip" src/*.c lib/*.h csv_editor.conf README.md makefile LICENSE