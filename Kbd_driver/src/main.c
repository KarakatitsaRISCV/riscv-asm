#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "vhid.h"

//char *cfgname = "~/.config/COKP_kbd.cfg";
char *cfgname = "COKP_kbd.cfg";
volatile int t_default_ms = 1000;

union{
  struct{
    char restore:1;
    char usb_err:1;
    char usb_pause:1;
  };
  int raw;
}initflags = {.raw = 0};


void usb_update();
void help(char name[]);
void cfg_load();

struct termios saved_attributes;

void reset_input_mode(){
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode(){
  struct termios tattr;
  tcgetattr (STDIN_FILENO, &saved_attributes);

  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO);
  tattr.c_cc[VMIN] = 1;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

enum btn_status{
  BTN_RELEASED = 0,
  BTN_PRESSED,
  BTN_HOLD,
};

typedef struct{
  char caption[6];
  char hotkey;
  char hotkey_hold;
  uint16_t mask;
  union{
    enum btn_status status;
    int flag;
  };
}button_t;
#define BTN_W	8
#define BTN_H	4

volatile button_t button[17] = {
  {.caption="  7  ", .hotkey='1', .hotkey_hold='!', .flag=0, .mask=0x0001},
  {.caption="  8  ", .hotkey='2', .hotkey_hold='@', .flag=0, .mask=0x0002},
  {.caption="  9  ", .hotkey='3', .hotkey_hold='#', .flag=0, .mask=0x0004},
  {.caption="  *  ", .hotkey='4', .hotkey_hold='$', .flag=0, .mask=0x0008},

  {.caption="  4  ", .hotkey='q', .hotkey_hold='Q', .flag=0, .mask=0x0010},
  {.caption="  5  ", .hotkey='w', .hotkey_hold='W', .flag=0, .mask=0x0020},
  {.caption="  6  ", .hotkey='e', .hotkey_hold='E', .flag=0, .mask=0x0040},
  {.caption="  /  ", .hotkey='r', .hotkey_hold='R', .flag=0, .mask=0x0080},

  {.caption="  1  ", .hotkey='a', .hotkey_hold='A', .flag=0, .mask=0x0100},
  {.caption="  2  ", .hotkey='s', .hotkey_hold='S', .flag=0, .mask=0x0200},
  {.caption="  3  ", .hotkey='d', .hotkey_hold='D', .flag=0, .mask=0x0400},
  {.caption="  -  ", .hotkey='f', .hotkey_hold='F', .flag=0, .mask=0x0800},

  {.caption="  <  ", .hotkey='z', .hotkey_hold='Z', .flag=0, .mask=0x1000},
  {.caption="  0  ", .hotkey='x', .hotkey_hold='X', .flag=0, .mask=0x2000},
  {.caption="  >  ", .hotkey='c', .hotkey_hold='C', .flag=0, .mask=0x4000},
  {.caption="  +  ", .hotkey='v', .hotkey_hold='V', .flag=0, .mask=0x8000},
  
  {.caption="", .hotkey=-1, .hotkey_hold=-1, .flag=0},
};

int btn_cur = 16;
volatile int t_press_ms = 1;

void draw_btn(uint8_t i){
  if(i > 15)return;
  int x = (i % 4) * BTN_W + 1;
  int y = (i / 4) * BTN_H + 1;
  char *color = "\033[1;32m";
  char *hkcolor = "\033[0;90m";
  char *bgcolor = "\033[0;1m";
  
  if(button[i].status == BTN_RELEASED){
    color = "\033[1;31m";
    printf("\033[%i;%iH%s┌─────┐", y, x, color);
    printf("\033[%i;%iH│%s%5.5s%s│", y+1, x, bgcolor, button[i].caption, color);
    printf("\033[%i;%iH│%s %c/%c %s│", y+2, x, hkcolor, button[i].hotkey, button[i].hotkey_hold, color);
    printf("\033[%i;%iH└─────┘", y+3, x);
  }else if(button[i].status == BTN_PRESSED){
    printf("\033[%i;%iH%s┌─────┐", y, x, color);
    printf("\033[%i;%iH│%s%5.5s%s│", y+1, x, bgcolor, button[i].caption, color);
    printf("\033[%i;%iH│%s %c/%c %s│", y+2, x, hkcolor, button[i].hotkey, button[i].hotkey_hold, color);
    printf("\033[%i;%iH└─────┘", y+3, x);
  }else if(button[i].status == BTN_HOLD){
    printf("\033[%i;%iH%s╔═════╗", y, x, color);
    printf("\033[%i;%iH║%s%5.5s%s║", y+1, x, bgcolor, button[i].caption, color);
    printf("\033[%i;%iH║%s %c/%c %s║", y+2, x, hkcolor, button[i].hotkey, button[i].hotkey_hold, color);
    printf("\033[%i;%iH╚═════╝", y+3, x);
  }
}

void redraw_all(){
  initflags.usb_pause = 1;
  printf("\033[1J\033[0m");
  for(int i=0; i<16; i++){
    btn_cur = i;
    draw_btn(i);
  }
  btn_cur = 16;
  initflags.usb_pause = 0;
  usb_update();
}

void redraw(int b_cur){
  if(b_cur != btn_cur)draw_btn(btn_cur);
  draw_btn(b_cur);
  printf("\033[17H\033[0m");
  fflush(stdout);
  usb_update();
}

void sig_timer(int sig){
  if(t_press_ms > 0){
    t_press_ms -= 100;
    if(t_press_ms <= 0){
      button[btn_cur].status = BTN_RELEASED;
      redraw(btn_cur);
      btn_cur = 16;
    }
  }
  static int t;
  t++;
  ///printf("\033[17;1H%i", t);
  if(initflags.usb_err){
    printf("\033[17;1H\033[41;1mDevice disconnected\033[0m\n");
  }else{
    printf("\033[17;1H\033[42;1mDevice connected\033[0m\n");
  }
  fflush(stdout);
}

void timer_init(){
  signal(SIGALRM, sig_timer);
  struct itimerval t_set, t_prev;
  t_set.it_interval.tv_sec = 0;
  t_set.it_interval.tv_usec = 100000;
  t_set.it_value.tv_sec = 0;
  t_set.it_interval.tv_usec = 100000;
  setitimer(ITIMER_REAL, &t_set, &t_prev);
}

void timer_soft(){
  pid_t pid;
  pid = fork();
  if(pid == 0){
    signal(SIGALRM, sig_timer);
    return;
  }
  pid_t res = 0;
  int stat;
  do{
    usleep(100000);
    kill(pid, SIGALRM);
    res = waitpid(pid, &stat, WNOHANG);
  }while(res == 0);
  exit(0);
}

#define StrEq(str, sample) (strncmp(str, sample, sizeof(sample)) == 0)

int main(int argc, char **argv){
  for(int i=1; i<argc; i++){
    if(StrEq(argv[i], "--colors")){
      for(int i=0; i<255; i++)printf("\033[%im%3i\033[0m ", i, i);
      printf("\n");
      return 0;
    }else if(StrEq(argv[i], "-c")){
      if((i+2) > argc){fprintf(stderr, "Wrong command line: -c option requires an argument\n"); return 0;}
      cfgname = argv[i+1];
      i++;
    }else if(StrEq(argv[i], "--restore")){
      initflags.restore = 1;
    }else if(StrEq(argv[i], "-h")){
      help(argv[0]);
      return 0;
    }
  }
  cfg_load();
  //return 0;
  
  char ch;
  set_input_mode();
  //timer_init();
  timer_soft();
  
  redraw_all();
  do{
    ch = getchar();
    for(int i=0; i<16; i++){
      if(ch == button[i].hotkey){
        if(i != btn_cur){
          if(button[btn_cur].status == BTN_PRESSED)button[btn_cur].status = BTN_RELEASED;
        }
        button[i].status = BTN_PRESSED;
        t_press_ms = t_default_ms;
        redraw(i);
        btn_cur = i;
      }
      if(ch == button[i].hotkey_hold){
        if(i != btn_cur){
          if(button[btn_cur].status == BTN_PRESSED)button[btn_cur].status = BTN_RELEASED;
        }
        if(button[i].status == BTN_HOLD)button[i].status=BTN_RELEASED; else button[i].status=BTN_HOLD;
        redraw(i);
        btn_cur = i;
      }
    }
  }while(ch != 27);
  reset_input_mode();
  printf("\033[2J\033[1H\033[0m");
}

void help(char name[]){
  printf("Usage: %s [--colors][-c cfgname][--restore][-h]\n", name);
  printf("  --colors\tdisplay colors avaible in current terminal\n");
  printf("  -c cfgname\tspecify name of the configuration file (now: [%s])\n", cfgname);
  printf("  --restore\trestore config file to default value\n");
  printf("  -h     \tShow this help\n");
  printf("\nESC - exit\n");
}

void cfg_load(){
  char *cfg = cfgname;
  char name[1000] = "";
  if(cfg[0] == '~'){
    char *env = getenv("HOME");
    strcpy(name, env);
    cfg++;
  }
  strcat(name, cfg);
  FILE *pf;
  if(initflags.restore){
    pf = fopen(name, "wt");
    if(pf == NULL){fprintf(stderr, "Can not open config file [%s]\n", name); return;}
    for(int i=0; i<4; i++){
      for(int j=0; j<4; j++)fprintf(pf, "%.5s\t", button[j+4*i].caption);
      fprintf(pf, "\n");
      for(int j=0; j<4; j++)fprintf(pf, " %c/%c \t", button[j+4*i].hotkey, button[j+4*i].hotkey_hold);
      fprintf(pf, "\n");
      for(int j=0; j<4; j++)fprintf(pf, "0x%.4X \t", button[j+4*i].mask);
      fprintf(pf, "\n\n");
    }
    fprintf(pf, "T_delay_ms = %u\n", t_default_ms);
    fprintf(pf, "\n");
    fprintf(pf, "#==== Comment ====\n");
    fprintf(pf, "#  ┌──────────────────────┐\n");
    fprintf(pf, "#  │      caption         │\n");
    fprintf(pf, "#  ├──────────────────────┤\n");
    fprintf(pf, "#  │ hotkey / hotkey_hold │\n");
    fprintf(pf, "#  └──────────────────────┘\n");
    fprintf(pf, "#  [caption]    \t 5-character caption of button\n");
    fprintf(pf, "#  [hotkey]     \t 1-character hotkey to press the button\n");
    fprintf(pf, "#  [hotkey_hold]\t 1-character hotkey to press and hold the button\n");
    fclose(pf);
    return;
  }

  pf = fopen(name, "rt");
  if(pf == NULL){fprintf(stderr, "Can not open config file [%s]\n", name); return;}
  int res;
  char str[1000];
  int line = 0;
  for(int i=0; i<4; i++){
    if( fgets(str, sizeof(str), pf) == NULL){
      fclose(pf); fprintf(stderr, "Unexpected end of configuration file\n"); exit(0);
    }
    line++;
    res = sscanf(str, "%5s%5s%5s%5s",
                 button[0+4*i].caption, button[1+4*i].caption,
                 button[2+4*i].caption, button[3+4*i].caption);
    if(res != 4){fclose(pf); fprintf(stderr, "Error 1 in configuration file %i\n", line); exit(0);}
    for(int j=0; j<4; j++){
      res = strlen((char*)(button[j+4*i].caption));
      if(res == 1){
        strcat((char*)(button[j+i*4].caption), "  ");
      }else if((res == 2)||(res==3)){
        strcat((char*)(button[j+i*4].caption), " ");
      }
    }
    if( fgets(str, sizeof(str), pf) == NULL){
      fclose(pf); fprintf(stderr, "Unexpected end of configuration file\n"); exit(0);
    }
    line++;
    res = sscanf(str, " %c/%c %c/%c %c/%c %c/%c",
                 &(button[0+4*i].hotkey), &(button[0+4*i].hotkey_hold),
                 &(button[1+4*i].hotkey), &(button[1+4*i].hotkey_hold),
                 &(button[2+4*i].hotkey), &(button[2+4*i].hotkey_hold),
                 &(button[3+4*i].hotkey), &(button[3+4*i].hotkey_hold)
          );
    if(res != 8){fclose(pf); fprintf(stderr, "Error 2 in configuration file %i %i\n", line, res); exit(0);}

    if( fgets(str, sizeof(str), pf) == NULL){
      fclose(pf); fprintf(stderr, "Unexpected end of configuration file\n"); exit(0);
    }
    line++;
    res = sscanf(str, "%"SCNx16"%"SCNx16"%"SCNx16"%"SCNx16,
                 &(button[0+4*i].mask),
                 &(button[1+4*i].mask),
                 &(button[2+4*i].mask),
                 &(button[3+4*i].mask)
          );
      
    if(res != 4){fclose(pf); fprintf(stderr, "Error 3 in configuration file %i %i\n", line, res); exit(0);}

    if( fgets(str, sizeof(str), pf) == NULL){
      fclose(pf); fprintf(stderr, "Unexpected end of configuration file\n"); exit(0);
    }
    line++;
  }
  while(1){
    if( fgets(str, sizeof(str), pf) == NULL)break;
    line++;
    char name[100], val[100];
    if(sscanf(str, "%99s%*[ \t=]%99s", name, val) < 1)continue;
    if(name[0] == '#')continue;
    if(StrEq(name, "T_delay_ms")){
      t_default_ms = atoi(val);
      if(t_default_ms == 0)t_default_ms = 1000;
    }
  }
  fclose(pf);
}

void usb_update(){
  if(initflags.usb_pause)return;
  initflags.usb_err = 0;
  hiddevice_t *dev = HidOpen(0x16C0, 0x05DF, L"COKPOWEHEU", L"USB RISCV programmer");
  if(!dev){
    initflags.usb_err = 1;
    return;
  }
  uint16_t data = 0;
  for(int i=15; i>=0; i--){
    data <<= 1;
    if(button[i].status != BTN_RELEASED)data |= 1;
  }
  int res = HidWrite(dev, &data, 2);
  //printf("\033[19H%.4X  \t%i", data, res);
  if(res != 2)initflags.usb_err = 1;
  HidClose(dev);
}