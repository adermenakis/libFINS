#include "FINS.h"
#include <iostream>

int main()
{
  FINS_t* c = FINS_create_eth_connection("eth0", "192.168.2.212", 9600);
  
  int i = -20;
  i = FINS_connect(c);
  std::cout << i << "\n";
  
  unsigned short arr[40];
  
  for(int i = 0; i < 40; i++)
    arr[i] = 0;
  
  i = FINS_read(c, 0xB0, 22, 1, arr);
  std::cout << i << "\n";

  for(int i = 0; i < 40; i++)
    std::cout << "i : " << arr[i] << "\n";
  
  i = FINS_close(c);
  std::cout << i << "\n";
  FINS_free(c);
  
  return 0;
}