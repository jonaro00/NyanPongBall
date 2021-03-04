
// bg.c
void init_bg();
void update_star_bg();
void draw_star_bg();
void update_rain_bg();
void draw_rain_bg();

// ei.S
void enable_interrupt();

// font.c
extern const uint8_t const font[128*8];

// game.c
#define FPS 60
extern uint32_t tick_start;
extern int* DEBUG_ADDR;
void loop();
void main_tick();
void game_init();
void game_tick();
void level_type0_init();
void level_type0_update();
void level_type0_draw();
void level_type1_init();
void level_type1_update();
void level_type1_draw();
void gameover_init();
void gameover_tick();
void menu_main_init();
void menu_main_tick();
void menu_scores_init();
void menu_scores_tick();
void menu_gameover_init();
void menu_gameover_tick();

// graphics.c
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
#define CHAR_SPACES (SCREEN_WIDTH / 6 + 1) * SCREEN_HEIGHT / 8
#define ANIM_SPEED 4
void quicksleep(int cyc);
void num32asc(char *s, int n);
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
int getsw();
int getbtns();
int is_clicked(int btnm);
int is_pressed(int btnm);
int getpot();

// math.c
int round(float f);
float min(float a, float b);
float max(float a, float b);
float bound(float min, float f, float max);
float avg(float a, float b);
float abs(float f);
int sign(float f);
int ixor(float a, float b);
int floorMod(int d, int m);
float pow(float a, int b);
float random();

// score.c
#define MAX_SCORES 32
typedef struct Score{
    char name[4];
    int score;
} Score;
Score init_Score(char * name, int score);
void add_Score(Score s);
int get_scores_len();
char * get_scores_page();

// string.c
char *itoaconv(int num);
int indexOf(char c, char *str);
void insert(char * src, char * target, int i, uint8_t nullend);

// textures.c
#define BALL_FRAMES 4
extern uint8_t t_ball[BALL_FRAMES][11][11];
#define NYANCAT_FRAMES 4
extern uint8_t t_nyancat[NYANCAT_FRAMES][14][23];
extern uint8_t t_bullet[7][8];

// unit.c
typedef struct Unit{
    uint8_t active;
    uint8_t alive;
    float y;
    float x;
    float dy;
    float dx;
    float ay;
    float ax;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
    int xdir;
} Unit;
void init_Unit(Unit * u, float y, float x, float dy, float dx, float ay, float ax, uint8_t h, uint8_t w, uint8_t * texture, int xdir);
void move_Unit(Unit * u);
void draw_Unit(Unit * u);
typedef struct AnimUnit{
    uint8_t active;
    uint8_t alive;
    float y;
    float x;
    float dy;
    float dx;
    float ay;
    float ax;
    uint8_t h;
    uint8_t w;
    uint8_t * texture;
    int xdir;
    uint8_t frames;
    uint8_t frame;
    uint8_t period;
} AnimUnit;
void init_AnimUnit(AnimUnit * u, float y, float x, float dy, float dx, float ay, float ax, uint8_t h, uint8_t w, uint8_t * texture, int xdir, uint8_t frames, uint8_t period);
void draw_AnimUnit(AnimUnit * u);
uint8_t collides(Unit * u1, Unit * u2);
uint8_t Unit_on_screen(Unit * u);
