#include "CImgProcess.h"

int main(void) {
  CImgProcess tem1;
  tem1.Process1();
  tem1.Process2();
  tem1.Process3_drawBySelf();
  tem1.Process4_drawBySelf();
  tem1.Process5_drawBySelf();

  
  CImgProcess tem2;
  tem2.Process1();
  tem2.Process2();
  tem2.Process3_drawByCImg();
  tem2.Process4_drawByCImg();
  tem2.Process5_drawByCImg();

  tem2.Process6();
  
  return 0;
}
