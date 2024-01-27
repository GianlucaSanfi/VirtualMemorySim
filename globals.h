#define BIT_PHYSICAL_ADDR 20 //1 MB
#define BIT_LOGICAL_ADDR 24 //16 MB

#define BIT_FRAME 12 //4 KB
#define BIT_FRAME_NUMBER 8

//valid, unswappable, read_bit, write_bit
#define BIT_PAGE_FLAGS 5

#define SIZE_VIRTUAL_MEMORY (1 << BIT_LOGICAL_ADDR)
#define SIZE_PHYSICAL_MEMORY (1 << BIT_PHYSICAL_ADDR)

#define SIZE_PAGE (1 << BIT_FRAME)
#define NUM_PAGES (SIZE_VIRTUAL_MEMORY / SIZE_PAGE)

//dimensione della tabella delle pagine in Byte (esclusa sizeof(int) della size)
#define SIZE_PAGE_TABLE ((NUM_PAGES * (BIT_FRAME_NUMBER + BIT_PAGE_FLAGS))/8)

//totale dei frame memoria fisica
#define NUM_FRAMES (SIZE_PHYSICAL_MEMORY / SIZE_PAGE)

//totale di frame del TLB
#define NUM_TLB_FRAMES 1


