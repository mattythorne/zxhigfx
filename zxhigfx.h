

#ifndef __ZXHIGFX_H__
#define __ZXHIGFX_H__

#include <stdlib.h>

#define BLACK 		0
#define BLUE		1
#define RED			2
#define MAGENTA		3
#define GREEN		4
#define CYAN		5
#define YELLOW		6
#define WHITE		7

#define XMAX 		255
#define YMAX		191



int attr(int ink, int paper, int bright, int flash){
	// Construct the attribute byte 
	return ink + (8 * paper) + (bright * 64) + (flash * 128);
}

void cplot(int x, int y, int attr){


//Bounds checking
if (x>XMAX) x=XMAX;
if (y>YMAX) y=YMAX;
if (attr>255) attr=255;

// Parameters - 
//            (ix+2) - attr 
//            (ix+4) - y
//            (ix+6) - x 

// Assembly Pixel Plotting Routine
// From "Advanced Spectrum Machine Language" by David Webb
// Published by Melbourne House 1984
// Page 20

	#asm
			LD		ix,0
			ADD		ix,sp
			LD		C,(ix+2)
			LD		L,(ix+4) 
			LD		H,(ix+6)    
			
		PLOT:
			LD 		A,L
			AND 	0C0H
			RRA
			SCF
			RRA
			RRCA
			XOR 	L
			AND 	0F8H
			XOR 	L
			LD 		D,A
			LD 		A,H
			RLCA
			RLCA
			RLCA
			XOR 	L
			AND 	0C7H
			XOR 	L
			RLCA
			RLCA
			LD 		E,A
			PUSH 	DE 			;address is stored in DE
			
			; Find attribute address
			LD 		A,D
			RRCA
			RRCA
			RRCA
			AND 	3
			OR 		58H
			LD		D,A
			LD 		B,0
			
			; Change attribute
			LD  	A,(DE)
			XOR 	C
			AND 	B
			XOR  	C
			LD 		(DE),A
			
			; Retrieve D.F. address
			POP 	DE
			LD 		A,H
			AND 	7
			LD 		B,A
			INC 	B
			
			; B holds target bit number plus 1
			LD 		A,0FEH
			
			; Rotate a window to the target bit
		PLOOP:
			RRCA
			DJNZ	PLOOP
			LD  	B,A
			LD  	A,0
			LD  	C,A
			
			; Take byte from D.F
			LD  	A,(DE)
			
			; Check for over 1
			BIT  	1,C
			JR		NZ,OVER1
			AND		B
			
			; Check for inverse 1
		OVER1:
			BIT 	3,C
			JR 		NZ,INV1
			XOR		B
			CPL
		INV1:
			LD		(DE),A
			RET
			
	#endasm

} 

// Clear screen to a particular attribute
void cls(int attr){

	// Assembly Screen Attribute Clearing Routine
	// From "Advanced Spectrum Machine Language" by David Webb
	// Published by Melbourne House 1984
	// Page 11
	#asm
			LD		ix,0
			ADD		ix,sp
			LD		A,(ix+2)
		CLS:
			LD		HL,4000H
			LD		BC,1800H
			LD		(HL),L
			LD		D,H
			LD		E,1
			LDIR
			LD		(HL),A
			LD		BC,02FFH
			LDIR
			RET
	#endasm
}

// Clear displayfile - removes text from screen
void clsdf(void){

	// Assembly Display File Clearing Routine
	// From "Advanced Spectrum Machine Language" by David Webb
	// Published by Melbourne House 1984
	// Page 07
	#asm
		CLSDF:
			LD		HL,4000H
			LD		BC,17FFH
			LD		(HL),L
			LD		D,H
			LD		E,1
			LDIR
			RET
	#endasm
}

void cDrawRect(int startx, int starty, int endx, int endy, int attr) {
	int x, y;
	
	for(x=startx; x<=endx; x++){
		cplot(x, starty, attr);
	}
	
	for(y=starty; y<=endy; y++){
		cplot(endx, y, attr);
	}
	
	for(x=endx; x>=startx; x--){
		cplot(x, endy, attr);
	}
	
	for(y=endy; y>=starty; y--){
		cplot(startx, y, attr);
	}
}

void cDrawLine(int startx, int starty, int endx, int endy, int attr) {
// http://cboard.cprogramming.com/game-programming/67832-line-drawing-algorithm.html

    int t, distance;
    int xerr=0, yerr=0, delta_x, delta_y;
    int incx, incy;
 
    /* compute the distances in both directions */
    delta_x=endx-startx;
    delta_y=endy-starty;
 
    /* Compute the direction of the increment,
       an increment of 0 means either a horizontal or vertical
       line.
    */
    if(delta_x>0) incx=1;
    else if(delta_x==0) incx=0;
    else incx=-1;
 
    if(delta_y>0) incy=1;
    else if(delta_y==0) incy=0;
    else incy=-1;
 
    /* determine which distance is greater */
    delta_x=abs(delta_x);
    delta_y=abs(delta_y);
    if(delta_x>delta_y) distance=delta_x;
    else distance=delta_y;
 
    /* draw the line */
    for(t=0; t<=distance+1; t++) {
        cplot(startx, starty, attr);
         
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance) {
            xerr-=distance;
            startx+=incx;
        }
        if(yerr>distance) {
            yerr-=distance;
            starty+=incy;
        }
    }
}
	
void cDrawCircle(int x0, int y0, int radius, int attr){
		int x, y, radiusError;
		
		x = radius; 
		y = 0;
		radiusError = 1-x;
 
		while(x >= y)
		{
			cplot(x + x0, y + y0, attr);
			cplot(y + x0, x + y0, attr);
			cplot(-x + x0, y + y0, attr);
			cplot(-y + x0, x + y0, attr);
			cplot(-x + x0, -y + y0, attr);
			cplot(-y + x0, -x + y0, attr);
			cplot(x + x0, -y + y0, attr);
			cplot(y + x0, -x + y0, attr); 
			y++;
			if(radiusError<0)
                radiusError+=2*y+1;
			else
			{
                x--;
                radiusError+=2*(y-x+1);
			}
		}
}

void cDrawFilledCircle(int x0, int y0, int radius, int attr){
	int n;
	for(n=radius; n>0; n--) cDrawCircle(x0, y0, n, attr); 
}

void cDrawFilledRect(int startx, int starty, int endx, int endy, int attr) {
	int x, y;
	for(y=starty; y<=endy; y++){
		for(x=startx; x<=endx; x++){
			cplot(x, y, attr);
		}
	}	
}


#endif