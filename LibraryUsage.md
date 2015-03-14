Example of using the library...

```

#include <stdio.h>
#include <zxhigfx.h>

#define TRUE 	1
#define FALSE 	0

void main(void) {

	int x, y, myattr;
	
	x=10;
	y=10;
	myattr = attr(WHITE, RED, TRUE, FALSE);
	cls(myattr);

        // Plot a horizontal line in pixels
	for(x=10;x<80;x++) cplot(x,y,myattr);
	
	myattr = attr(CYAN, RED, TRUE, FALSE);
        // Draw a diagonal line
	cDrawLine(0,0,255,192,myattr);
	
	myattr = attr(MAGENTA, RED, TRUE, FALSE);
        // Draw a filled rectangle at 30,30
	cDrawFilledRect(30,30,130,80,myattr);
	
	myattr = attr(GREEN, RED, TRUE, FALSE);
        // Draw a filled circle centre of screen
	cDrawFilledCircle(127, 96, 50, myattr);
	
        // set the border to green
	cBorder(GREEN);
	
	while(1){};
    
}

```

Compile the example with...

```
zcc +zx -O3 -v main.c -o main.bin -create-app -lndos
```