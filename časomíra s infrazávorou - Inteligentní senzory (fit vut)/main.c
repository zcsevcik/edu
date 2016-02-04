#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

char last_ch;
char last_opto;
unsigned long t0;
unsigned long t;

char isrunning;

#define OPTO_dir_in()   ((P4DIR) &= (~BIT0))
#define OPTO_read()     (!!((P4IN) & (BIT0)))

/* 1 tik casovace = 30,518 us */
#define TIMER_TICK 30.518

/* globalni promenne */
unsigned long global_time = 0;

/*******************************************************************************
 * Aktualizace globalniho casu
*******************************************************************************/
void global_time_update() {

   static unsigned long act_time, prev_time = 0;

   CCTL0 &= ~CCIE;  // disable interrupt               

   act_time = TAR;

   if (act_time >= prev_time)
      global_time += (act_time - prev_time); // v ramci stejneho cyklu
   else
      global_time += (act_time + (65536 - prev_time)); // preteceni

   prev_time = act_time;

   CCTL0 = CCIE;  // enable interrupt               
}

/*******************************************************************************
 * Aktualizace globalniho casu
*******************************************************************************/
unsigned long get_time() {

   global_time_update();

   return global_time;
}

/*******************************************************************************
 * Obsluha preruseni casovace timer A0
*******************************************************************************/
interrupt (TIMERA0_VECTOR) Timer_A (void) {

   global_time_update();

   CCR0 += 0x80;
}

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
}

/*******************************************************************************
 * Casomira
*******************************************************************************/
int stopwatch_reset()
{
    t0 = get_time();
    t = t0;
}

int stopwatch_start()
{
    stopwatch_reset();
    isrunning = 1;
}

int stopwatch_stop()
{
    isrunning = 0;
}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
int keyboard_idle()
{
  char ch;
  ch = key_decode(read_word_keyboard_4x4());
  if (ch != last_ch) 
  {
    last_ch = ch;
    if (ch != 0) 
    {
      if (ch == '*') {
          stopwatch_reset();
      }
      else if (ch == '#' && isrunning) {
          stopwatch_stop();
      }
      else if (ch == '#' && !isrunning && (t == t0)) {
          stopwatch_start();
      }
    }
  }
  return 0;
}

/*******************************************************************************
 * Obsluha optozavory
*******************************************************************************/
int opto_idle()
{
    char opto;
    opto = OPTO_read();

    if (opto != last_opto) {
        last_opto = opto;
        if (opto == 1) {
            if (!isrunning && (t == t0)) {
                stopwatch_start();
            }
            else {
                stopwatch_stop();
            }
        }
    }

    return 0;
}

/*******************************************************************************
 * Zobrazeni casu na displeji
*******************************************************************************/
int display_idle()
{
    long microseconds, milliseconds, seconds, minutes, hours;
    char buf[20];

    if (isrunning)
        t = get_time();

    microseconds = (long)(((float)(t - t0))*TIMER_TICK);
    milliseconds = (long)(microseconds / 1000) % 1000;
    seconds = (((long)(microseconds / 1000) - milliseconds) / 1000) % 60;
    minutes = (((((long)(microseconds / 1000) - milliseconds) / 1000) - seconds) / 60) % 60;
    hours = ((((((long)(microseconds / 1000) - milliseconds) / 1000) - seconds) / 60) - minutes) / 60;

    if (hours < 10) {
        buf[0] = '0';
        long2str(hours, &buf[1], 10);
    }
    else {
        long2str(hours, &buf[0], 10);
    }
    buf[2] = ':';

    if (minutes < 10) {
        buf[3] = '0';
        long2str(minutes, &buf[4], 10);
    }
    else {
        long2str(minutes, &buf[3], 10);
    }
    buf[5] = ':';

    if (seconds < 10) {
        buf[6] = '0';
        long2str(seconds, &buf[7], 10);
    }
    else {
        long2str(seconds, &buf[6], 10);
    }
    buf[8] = '.';

    if (milliseconds < 10) {
        buf[9] = '0';
        buf[10] = '0';
        long2str(milliseconds, &buf[11], 10);
    }
    else if (milliseconds < 100) {
        buf[9] = '0';
        long2str(milliseconds, &buf[10], 10);
    }
    else {
        long2str(milliseconds, &buf[9], 10);
    }
    buf[12] = '\0';

    LCD_clear();
    LCD_append_string(buf);

    return 0;
}


/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
  LCD_init();
  LCD_clear();
}


/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  unsigned int cnt = 0;
  last_ch = 0;
  last_opto = 0;
  isrunning = 0;

  OPTO_dir_in();
  initialize_hardware();
  keyboard_init();

  set_led_d6(1);                       // rozsviceni D6
  set_led_d5(1);                       // rozsviceni D5

  //term_send_crlf();
  //term_send_str(" >");

  /* nastaveni casovace na periodu 10 ms */
  CCTL0 = CCIE;  // enable interrupt               
  CCR0 = 0x80;
  TACTL = TASSEL_1 + MC_2;

  stopwatch_reset();

  while (1)
  {
    delay_ms(1);

    cnt++;
    if (cnt >= 500)
        cnt = 0;

    if (cnt == 0)
    {
      cnt = 0;
      flip_led_d6();                   // negace portu na ktere je LED
    }

    if (cnt % 10 == 1) {
        keyboard_idle();                   // obsluha klavesnice
        terminal_idle();                   // obsluha terminalu
    }
    if (cnt % 100 == 2) {
        display_idle();
    }

    opto_idle();
  }         

  //CCTL0 &= ~CCIE;  // disable interrupt

}
