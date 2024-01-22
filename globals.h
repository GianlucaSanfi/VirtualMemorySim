// virtual mem size: 16MB = 2^24
// physical mem size: 1MB = 2^20 

#define BIT_PHYSICAL_ADDR 20
#define BIT_LOGIC_ADDR 24

// frame e page size: 256B = 2^8 ?? 
#define BIT_FRAME 8                         /* TODO*/

//valid, unswappable, read_bit, write_bit
#define BIT_PAGE_FLAGS 4

#define SIZE_VIRTUAL_MEMORY (1 << BIT_LOGIC_ADDR)
#define SIZE_PHYSICAL_MEMORY (1 << BIT_PHYSICAL_ADDR)

#define SIZE_PAGE (1 << (BIT_FRAME))
#define NUM_PAGES (SIZE_VIRTUAL_MEMORY / (1 << BIT_FRAME))

//dimensione della tabella delle pagine
#define SIZE_PAGE_TABLE (NUM_PAGES * (BIT_FRAME + BIT_PAGE_FLAGS))

//totale dei frame memoria fisica
#define NUM_FRAMES (SIZE_PHYSICAL_MEMORY / (1 << BIT_FRAME))


