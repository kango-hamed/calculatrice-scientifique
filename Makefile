# =============================================================================
#  Makefile — Calculatrice Scientifique en C
#  Usage :
#      make          → compile le projet complet
#      make test     → compile et execute les tests unitaires
#      make run      → compile et lance la calculatrice
#      make clean    → supprime les fichiers generes
#      make rebuild  → clean + recompile tout
# =============================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Wno-unused-parameter
LDFLAGS = -lm

# -----------------------------------------------------------------------------
#  Chemins des headers
#  Inclut automatiquement tous les sous-dossiers src/<module>/
#  ainsi que include/ si vous y placez des headers partages
# -----------------------------------------------------------------------------
INCLUDES = $(sort $(dir $(wildcard src/*/*.c))) include
CFLAGS  += $(addprefix -I,$(INCLUDES))

# -----------------------------------------------------------------------------
#  Modules du projet
# -----------------------------------------------------------------------------
MODULES = core basic memory functions calculus complex stat basen eqn matrix \
          constants interface table

# Sources : tous les .c dans src/<module>/
SRC     = $(foreach m,$(MODULES),$(wildcard src/$(m)/*.c))

# Objets dans build/ (miroir de src/)
OBJDIR  = build
OBJS    = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC))

# Fichiers de dependances auto-generes (.d) pour recompiler si .h change
DEPS    = $(OBJS:.o=.d)

TARGET  = calc

# =============================================================================
#  Build principal
# =============================================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo ""
	@echo "  ==> Linkage..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "  ==> Build OK : ./$(TARGET)"
	@echo ""

# -----------------------------------------------------------------------------
#  Compilation .c -> .o
#  -MMD -MP : genere un fichier .d listant les headers dont depend ce .c
#             => si un .h change, make recompile automatiquement les .c concernes
# -----------------------------------------------------------------------------
$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "  CC  $<"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Inclusion silencieuse des .d (absents au premier build, c'est normal)
-include $(DEPS)

# =============================================================================
#  Tests unitaires
#  Reutilise les .o deja compiles — ne recompile pas tout le projet
# =============================================================================

TEST_SRCS = $(foreach m,$(MODULES),$(wildcard tests/$(m)/test_$(m).c))
TEST_BINS = $(patsubst tests/%.c,$(OBJDIR)/tests/%,$(TEST_SRCS))

# Tous les .o sauf main.o pour eviter le symbole 'main' en double
OBJS_NO_MAIN = $(filter-out $(OBJDIR)/core/main.o, $(OBJS))

test: $(TARGET) $(TEST_BINS)
	@echo ""
	@echo "  ==> Execution des tests..."
	@failed=0; \
	for bin in $(TEST_BINS); do \
		echo "  -> $$bin"; \
		$$bin || failed=1; \
	done; \
	if [ $$failed -eq 0 ]; then \
		echo "  ==> Tous les tests passes."; \
	else \
		echo "  ==> Des tests ont echoue !"; exit 1; \
	fi
	@echo ""

$(OBJDIR)/tests/%: tests/%.c $(OBJS_NO_MAIN)
	@mkdir -p $(dir $@)
	@echo "  CC (test) $<"
	$(CC) $(CFLAGS) $< $(OBJS_NO_MAIN) -o $@ $(LDFLAGS)

# =============================================================================
#  Cibles utilitaires
# =============================================================================

run: $(TARGET)
	./$(TARGET)

# Usage : make script FILE=mon_script.txt
script: $(TARGET)
	./$(TARGET) -f $(FILE)

clean:
	@echo "  ==> Nettoyage..."
	rm -rf $(OBJDIR) $(TARGET)
	@echo "  ==> Clean OK"

rebuild: clean all

# Affiche les variables detectees (utile pour debugger le Makefile)
info:
	@echo "  Modules  : $(MODULES)"
	@echo "  Sources  : $(SRC)"
	@echo "  Objets   : $(OBJS)"
	@echo "  Tests    : $(TEST_SRCS)"
	@echo "  Includes : $(INCLUDES)"

# =============================================================================

.PHONY: all test run script clean rebuild info