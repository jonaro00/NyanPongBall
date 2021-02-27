
// ei.S
void enable_interrupt();

// font.c
extern const uint8_t const font[128*8];

// game.c
extern int* DEBUG_ADDR;
void game_init();
void loop();
void game_tick();
void btn_press(int btn_i);
void btn_hold(int btn_i);
void debug_screen();

// graphics.c
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
#define ANIM_SPEED 4
void quicksleep(int cyc);
void num32asc(char *s, int n);
char *itoaconv(int num);
uint8_t spi_send_recv(uint8_t data);
void display_init(void);
extern uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
void screen_fill(uint8_t p);
int is_on_screen(int y, int x);
void screen_render();
uint8_t screen_get_strip(uint8_t y, uint8_t x);
void screen_set_strip(int y, int x, uint8_t b);
void screen_display_string(int y, int x, char *s);
void screen_display_texture(int y, int x, uint8_t h, uint8_t w, uint8_t *t, int xdir);
void screen_draw_box(int y, int x, uint8_t h, uint8_t w, uint8_t p);
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

// math.c
int round(float f);
float bound(float min, float f, float max);
float avg(float a, float b);
float abs(float f);

// textures.c
extern uint8_t t_ball[11][11];
#define NYANCAT_FRAMES 4
extern uint8_t t_nyancat[NYANCAT_FRAMES][14][23];

// unit.c
typedef struct Unit{
    float y;
    float x;
    float dy;
    float dx;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
    int xdir;
} Unit;
void init_Unit(Unit * u, float y, float x, float dy, float dx, uint8_t h, uint8_t w, uint8_t * texture, int xdir);
void draw_Unit(Unit * u);
typedef struct AnimUnit{
    float y;
    float x;
    float dy;
    float dx;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
    int xdir;
    uint8_t frames;
    uint8_t frame;
} AnimUnit;
void init_AnimUnit(AnimUnit * u, float y, float x, float dy, float dx, uint8_t h, uint8_t w, uint8_t * texture, int xdir, uint8_t frames);
void draw_AnimUnit(AnimUnit * u);
