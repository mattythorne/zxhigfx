Go to the source code of this file.

# Macros #

```
#define 	BLACK   0
#define 	BLUE   1
#define 	RED   2
#define 	MAGENTA   3
#define 	GREEN   4
#define 	CYAN   5
#define 	YELLOW   6
#define 	WHITE   7
#define 	XMAX   255
#define 	YMAX   191
```


# Functions #

```
int  	attr (int ink, int paper, int bright, int flash)
void 	cBorder (int color)
void 	cplot (int x, int y, int attr)
void 	cls (int attr)
void 	clsdf (void)
void 	cDrawRect (int startx, int starty, int endx, int endy, int attr)
void 	cDrawLine (int startx, int starty, int endx, int endy, int attr)
void 	cDrawCircle (int x0, int y0, int radius, int attr)
void 	cDrawFilledCircle (int x0, int y0, int radius, int attr)
void 	cDrawFilledRect (int startx, int starty, int endx, int endy, int attr)
```

# Function Documentation #


---


```
int attr(int ink,
         int paper,
         int bright,
         int flash 
        )		
```

Helper function - return the attribute value based on the specified ink and paper colours

**Parameters**

> <table>
<blockquote><tr><td><code>ink</code></td><td>an integer representing the ink colour (0 - 7) </td></tr>
<tr><td><code>paper</code></td><td>an integer representing the paper colour (0 - 7)</td></tr>
<tr><td><code>bright</code></td><td>an integer specifying wether bright is used (0, 1)</td></tr>
<tr><td><code>flash</code></td><td>an integer specifying wether flash is used (0, 1) </td></tr>
</table></blockquote>

**Returns**

> an integer representing the attribute value


---
