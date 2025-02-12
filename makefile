compile:csv_editor.exe



# csv_editor.exe:obj/main.o obj/config.o obj/colors.o obj/table.o obj
# 	gcc -Wall obj/main.o obj/config.o obj/colors.o obj/table.o -o csv_editor.exe -lncurses

# obj/main.o:src/main.c obj
# 	gcc -Wall -c src/main.c -o obj/main.o -lncurses

# obj/config.o:src/config.c obj
# 	gcc -Wall -c src/config.c -o obj/config.o -lncurses

# obj/colors.o:src/colors.c obj
# 	gcc -Wall -c src/colors.c -o obj/colors.o -lncurses

# obj/table.o:src/table.c obj
# 	gcc -Wall -c src/table.c -o obj/table.o -lncurses



csv_editor.exe:obj/main.o obj/config.o obj/colors.o obj/table.o obj
	gcc -Wall obj/main.o obj/config.o obj/colors.o obj/table.o -o csv_editor.exe

obj/main.o:src/main.c obj
	gcc -Wall -c src/main.c -o obj/main.o

obj/config.o:src/config.c obj
	gcc -Wall -c src/config.c -o obj/config.o

obj/colors.o:src/colors.c obj
	gcc -Wall -c src/colors.c -o obj/colors.o

obj/table.o:src/table.c obj
	gcc -Wall -c src/table.c -o obj/table.o



obj:
	mkdir obj

clean:
	rm -r obj

clear:clean
	rm *.exe

zip:
	zip "csv_editor.zip" src/*.c lib/*.h csv_editor.conf README.md makefile LICENSE