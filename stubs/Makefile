# =============================================================================
#  Makefile — Calculatrice Scientifique en C
#  Usage :
#    make           → compile avec les STUBS (mode développement)
#    make REAL=1    → compile avec les vrais modules (quand disponibles)
#    make test      → compile et exécute les tests
#    make clean     → supprime les fichiers générés
# =============================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Wno-unused-parameter

MODULES = core basic memory functions calculus complex stat basen eqn matrix constants interface

# --- Sources réelles (un .c par module) ---
REAL_SRC = $(foreach m,$(MODULES),$(wildcard src/$(m)/$(m).c))

# --- Stubs (remplacent les modules non encore implémentés) ---
STUB_SRC = stubs/core_stub.c \
           stubs/basic_stub.c \
           stubs/memory_stub.c \
           stubs/functions_stub.c \
           stubs/calculus_stub.c \
           stubs/complex_stub.c \
           stubs/stat_stub.c \
           stubs/basen_stub.c \
           stubs/eqn_stub.c \
           stubs/matrix_stub.c \
           stubs/constants_stub.c \
           stubs/interface_stub.c

# Par défaut : utilise les stubs
ifdef REAL
  SRC = main.c $(REAL_SRC)
else
  SRC = main.c $(STUB_SRC)
endif

OBJS   = $(SRC:.c=.o)
TARGET = calc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@echo "==> Build OK : ./$(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	@echo "==> Compilation des tests (avec stubs)..."
	@for mod in $(MODULES); do \
	    echo "  test $$mod..."; \
	    $(CC) $(CFLAGS) tests/$$mod/test_$$mod.c $(STUB_SRC) -o tests/$$mod/runner_$$mod -lm 2>&1 && \
	    ./tests/$$mod/runner_$$mod || exit 1; \
	done
	@echo "==> Tous les tests passent."

clean:
	rm -f $(OBJS) $(TARGET)
	find tests -name "runner_*" -delete
	@echo "==> Clean OK"

.PHONY: all test clean
d