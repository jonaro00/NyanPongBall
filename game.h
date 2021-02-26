
// ei.S
void enable_interrupt(void);

// font.c
extern const uint8_t const font[128*8];

// game.c
extern int* DEBUG_ADDR;
void game_init();
void loop();
void DEBUG(int* a);
void game_tick();
void btn_press(int btn_i);
void btn_hold(int btn_i);
void debug_screen();

// graphics.c
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
void quicksleep(int cyc);
void num32asc( char * s, int );
uint8_t spi_send_recv(uint8_t data);
void display_init(void);
extern uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
void screen_reset();
void screen_render();
uint8_t screen_get_strip(uint8_t y, uint8_t x);
void screen_set_strip(uint8_t y, uint8_t x, uint8_t b);
void screen_display_string(uint8_t y, uint8_t x, char *s);
void screen_display_texture(uint8_t y, uint8_t x, uint8_t h, uint8_t w, uint8_t *t);
void screen_draw_box(uint8_t y, uint8_t x, uint8_t h, uint8_t w, uint8_t p);
void print_debug(const volatile int* addr);

// io.c
#define BTN4 0b1000
#define BTN3 0b0100
#define BTN2 0b0010
#define BTN1 0b0001
int getsw(void);
int getbtns(void);
int is_clicked(int btnm);
int is_pressed(int btnm);

// textures.c
extern uint8_t ball[11][11];
#define NYANCAT_FRAMES 4
extern uint8_t nyancat[NYANCAT_FRAMES][14][23];

// unit.c
typedef struct Unit{
    int y;
    int x;
    int dy;
    int dx;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
} Unit;
void init_Unit(Unit * u, int y, int x, int dy, int dx, uint8_t h, uint8_t w, uint8_t * texture);
void draw_Unit(Unit * u);
typedef struct AnimUnit{
    int y;
    int x;
    int dy;
    int dx;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
    uint8_t frames;
    uint8_t frame;
} AnimUnit;
void init_AnimUnit(AnimUnit * u, int y, int x, int dy, int dx, uint8_t h, uint8_t w, uint8_t * texture, uint8_t frames);
void draw_AnimUnit(AnimUnit * u);
