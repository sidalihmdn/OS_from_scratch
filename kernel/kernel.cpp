#include "../drivers/HAL.h"
#include "../drivers/screen.h"
#include "../includes/time.h"

#if DEBUG
// some debug code
#endif

int main() {
  clean_screen();
  zprint("berkukes # ");
  HAL_init();
  // print_string(int2String(1234),4);
  zprint("\nhi");
  print_date();
  //    int a = 12/0;
  //    zprint("hrhr\n");
  //    int b = 12/0;

  // testing the hex2string io_function
  // zprint(hex2String(0x1234));
  // zprint("\n");
  // zprint(int2String(1234));
  // print_char(0, -1 , -1 , '0'+ len((char *)"les"));
  return 0;
}
