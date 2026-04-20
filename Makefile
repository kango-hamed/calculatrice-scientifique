# =============================================================================
#  Makefile — Calculatrice Scientifique en C
#  Usage :
#      make        → compile le projet complet
#      make test   → compile et exécute les tests
#      make clean  → supprime les fichiers générés
# =============================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Wno-unused-parameter -Iinclude
LDFLAGS = -lm

# Modules du projet
MODULES = core basic memory functions calculus complex stat basen eqn matrix constants interface

# Sources et objets
SRC     = $(foreach m,$(MODULES),$(wildcard src/$(m)/*.c))
OBJS    = $(SRC:.c=.o)

TARGET  = calc

# =============================================================================
# Build principal
# =============================================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "==> Build OK : ./$(TARGET)"

# Compilation des .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# =============================================================================
# Tests
# =============================================================================

test:
	@echo "==> Compilation des tests..."
	@for mod in $(MODULES); do \
		if [ -f tests/$$mod/test_$$mod.c ]; then \
			echo "  -> test $$mod"; \
			$(CC) $(CFLAGS) tests/$$mod/test_$$mod.c $(SRC) -o tests/$$mod/runner_$$mod $(LDFLAGS) && \
			./tests/$$mod/runner_$$mod || exit 1; \
		else \
			echo "  -> test $$mod ignoré (fichier manquant)"; \
		fi \
	done
	@echo "==> Tous les tests exécutés."

# =============================================================================
# Clean
# =============================================================================

clean:
	rm -f $(OBJS) $(TARGET)
	find tests -type f -name "runner_*" -delete
	@echo "==> Clean OK"

# =============================================================================

.PHONY: all test clean