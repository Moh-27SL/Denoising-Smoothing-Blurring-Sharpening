#include <iostream>
#include "DSBS.h"
#include "header/png.h"

using namespace std;

int main()
{
    PngImage img = load_png("result/mary.png");

    img.data = Sharpen(img.data,img.channels,img.height,img.width,3,0.6);

    save_png("result/tst_.png",img);

    return 0;
}
