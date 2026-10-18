# Linux/macOS (raylib instalada no sistema)
all:
	gcc main.c player.c -o leon_teste -lraylib -lm

# Windows (MinGW + raylib):
# gcc main.c player.c -o leon_teste.exe -lraylib -lopengl32 -lgdi32 -lwinmm
