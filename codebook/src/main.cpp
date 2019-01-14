#include <cstdio>

#include "codebooksmanager.h"
CodeBookColorSpace clrsp = CodeBookColorSpace::RGB;
CodeBookManager cbm(clrsp);

void cbImage ()
{
  cbm.CbImage();
}

void cbVideo ()
{
  cbm.CbVideo();
}

void cbVideoStreaming()
{
  cbm.CbVideoStreaming();
}

int main()
{
    void(*f[3])();
    f[0] = cbImage;
    f[1] = cbVideo; 
    f[2] = cbVideoStreaming;

    int n;
    printf(". Image           - 1\n");
    printf(". Video           - 2\n");
    printf(". Video Streaming - 3\n");
    printf(". Exit            - 0\n");
    scanf_s("%d", &n);
    
    if(n > 0 && n < 4)
        f[n-1]();

    return 0;
}