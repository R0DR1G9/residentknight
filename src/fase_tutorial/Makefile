RAYLIB := $(shell pkg-config --cflags --libs raylib 2>/dev/null)
ifeq ($(RAYLIB),)
RAYLIB := -lraylib
endif

FONTES = main_tutorial.c fase.c fase_desenho.c

tutorial: $(FONTES) fase.h
	gcc $(FONTES) -o tutorial $(RAYLIB) -lm

tutorial-win: $(FONTES) fase.h
	gcc $(FONTES) -o tutorial.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lm

clean:
	rm -f tutorial tutorial.exe

.PHONY: clean tutorial-win
