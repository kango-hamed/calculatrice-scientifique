#ifndef BASEN_H
#define BASEN_H

/**
 * Module : basen
 * Calcul en base N — mode BASE-N, conversions, opérateurs logiques
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet. Toute demande de modification
 * passe par une GitHub Issue avec le label "interface".
 */
/* --- Conversions de base --- */
char *to_hex(long val);
char *to_bin(long val);
char *to_oct(long val);
long  from_base(const char *s, int base);
long  from_binary(const char *s);
long  from_octal(const char *s);
long  from_hex(const char *s);
char *basen_convert(long val, int from_base, int to_base);

/* --- Opérations arithmétiques en base courante --- */
long basen_add(long a, long b);
long basen_sub(long a, long b);
long basen_mul(long a, long b);
long basen_div(long a, long b);

/* --- Opérateurs logiques bit à bit --- */
long logic_and(long a, long b);
long logic_or(long a, long b);
long logic_xor(long a, long b);
long logic_xnor(long a, long b);
long logic_not(long a);
long logic_neg(long a);

/* --- Détection de préfixe inline (0x, 0b, 0o, d) --- */
int prefix_detect(const char *token);



/*long     basen_and(long a, long b);
long     basen_or(long a, long b);
long     basen_xor(long a, long b);*/


#endif /* BASEN_H */
