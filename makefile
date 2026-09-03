CC       := cc
CFLAGS   := -Wall -Wextra -Werror

TARGET   := codexion
SRCS     := src/main.c src/threads/creating_threads.c src/threads/initialisations.c \
			src/utils.c src/simulation/edf_handling.c src/simulation/fifo_handling.c \
			src/simulation/ordering_schedul.c src/simulation/release_dongles.c src/simulation/compile_debug_refactor.c

# SRCS :=
#     src/main.c
#     src/threads/creating_threads.c
#     src/threads/initilisation.c
#     src/threads/utils.c
#     src/simulation/edf_handling.c
#     src/simulation/fifo_handling.c
#     src/simulation/ordering_schedul.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@
