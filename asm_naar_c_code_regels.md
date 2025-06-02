Je krijgt zo een blok Game Boy (Z80) assembly-code uit het pokered-project.
Doel: schrijf functionele C-code die byte-voor-byte identiek gedrag oplevert, zodat we de C-versie later via een handgemaakte transpiler kunnen terugdraaien naar exact dezelfde ASM-instructies (checksum identiek).

Regels waaraan jouw output moet voldoen

Spiegel elke ASM-regel – behoud register-flow, side-effects en volgorde (geen optimalisaties).

Gebruik dezelfde symbolische labels/variabelen als in pokered (wStatusFlags4, BIT_SCRIPTED_MOVEMENT_STATE, …).

Vervang xor a door uint8_t a = 0; of a = 0; en laat vervolgens de oorspronkelijke store-instructie zien.

Simuleer swap a met bit-shifts ( (x<<4)|(x>>4) ).

Voor bit-instructies:

res BIT_X, [hl] → flags &= ~(1 << BIT_X);

set BIT_Y, [hl] → flags |= (1 << BIT_Y);

Roep subroutines op met exact dezelfde naam (DecodeRLEList(&dst, src);).

Lever één self-contained C-functie, bijv.

c
Copy
Edit
void PalletMovementScript_WalkToLab(void) { … }
en sluit af met return; om de ret te spiegelen.

Voeg beknopte inline comments toe, zodat duidelijk is welke ASM-regel gerepliceerd wordt (handig voor diff-checks).

Geen extra logica, optimalisaties of C-helpers die niet strikt nodig zijn om de oorspronkelijke instructie te spiegelen.