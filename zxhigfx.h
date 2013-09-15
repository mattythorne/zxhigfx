/**
 *  \mainpage
 *	High Resolution 256x192 Colour Graphics Library for ZX Spectrum
 *	---------------------------------------------------------------
 *	
 *  \section intro_sec Introduction
 *	C library providing assembly functions for plotting pixels to the ZX spectrum screen 
 *	and attribute changing (paper, ink) plus some c functions utilising these
 *	for drawing primitives.
 *	
 *	For use with z88dk cross compiler for z80 based machines
 *		http://www.z88dk.org/forum/
 *		
 *	\section testing_sec Testing
 *	Tested on 48k ZX Spectrum, ZX Spectrum 128k, ZX Spectrum +2, Spectrum +3
 *	known compatibility issue with 16k ZX Spectrum
 *	
 *  \section license_sec License
 *  Copyright 2013 Matthew Thorne
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

//TODO : Add screen page swapping and sprite handling functionality

#ifndef __ZXHIGFX_H__
#define __ZXHIGFX_H__

#include <stdlib.h>

/**
 *	Standard spectrum colour black
 */
#define BLACK 		0
/**
 *	Standard spectrum colour blue
 */
#define BLUE		1
/**
 *	Standard spectrum colour red
 */
#define RED			2
/**
 *	Standard spectrum colour magenta
 */
#define MAGENTA		3
/**
 *	Standard spectrum colour green
 */
#define GREEN		4
/**
 *	Standard spectrum colour cyan
 */
#define CYAN		5
/**
 *	Standard spectrum colour yellow
 */
#define YELLOW		6
/**
 *	Standard spectrum colour white
 */
#define WHITE		7


/**
 *	Spectrum horizontal screen boundary
 */
#define XMAX 		255
/**
 *	Spectrum vertical screen boundary
 */
#define YMAX		191


/** 
 *	Helper function - return the attribute value based on the specified
 *	ink and paper colours
 *	@param ink an integer representing the ink colour (0 - 7)
 *	@param paper an integer representing the paper colour (0 - 7)
 *	@param bright an integer specifying wether bright is used (0, 1)
 *  @param flash an integer specifying wether flash is used (0, 1)
 *  @return an integer representing the attribute value
 */
int attr(int ink, int paper, int bright, int flash){
	// Construct the attribute byte 
	return ink + (8 * paper) + (bright * 64) + (flash * 128);
}

/**
 *	Set the spectrum border colour
 *	@param color an integer representing the border colour (0 - 7)
 */  
void cBorder(int color){
	#asm
		LD		ix,0
		ADD		ix,sp
		LD		A,(ix+2)
		
		CALL	8859
		RET
	#endasm
}

/**
 *	plot a pixel to the screen
 *	@param x an integer specifying the horizontal position of the pixel
 *	@param y an integer specifying the vertical position of the pixel
 *  @param attr an integer specifying the attribute value of the pixel
 *  @see attr(int ink, int paper, int bright, int flash)
 */
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

/**
 *	Clear screen to specified attribute
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
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

/**
 *	Clear the spectrum displayfile
 */
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

/**
 *  Draw a rectangle primitive - origin topleft
 *	@param startx an integer specifying the horizontal origin
 *	@param starty an integer specifying the vertical origin
 *	@param endx an integer specifying the horizontal end point
 *	@param endy an integer specifying the vertical end point
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
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

/**
 *	Draw a line primitive 
 *	@param startx an integer specifying the horizontal origin
 *	@param starty an integer specifying the vertical origin
 *	@param endx an integer specifying the horizontal end point
 *	@param endy an integer specifying the vertical end point
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
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

/**
 *	Draw a circle primitive - origin centre
 *	@param x0 an integer specifying the horizontal origin
 *	@param y0 an integer specifying the vertical origin
 *	@param radius an integer specifying the radius of the circle
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
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

/**
 *	Draw a filled circle primitive - origin centre
 *	@param x0 an integer specifying the horizontal origin
 *	@param y0 an integer specifying the vertical origin
 *	@param radius an integer specifying the radius of the circle
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
void cDrawFilledCircle(int x0, int y0, int radius, int attr){
	int n;
	for(n=radius; n>0; n--) cDrawCircle(x0, y0, n, attr); 
}

/**
 *  Draw a filled rectangle primitive - origin topleft
 *	@param startx an integer specifying the horizontal origin
 *	@param starty an integer specifying the vertical origin
 *	@param endx an integer specifying the horizontal end point
 *	@param endy an integer specifying the vertical end point
 *	@param attr an integer specifying the attribute value
 *  @see attr(int ink, int paper, int bright, int flash)
 */
void cDrawFilledRect(int startx, int starty, int endx, int endy, int attr) {
	int x, y;
	for(y=starty; y<=endy; y++){
		for(x=startx; x<=endx; x++){
			cplot(x, y, attr);
		}
	}	
}



#endif