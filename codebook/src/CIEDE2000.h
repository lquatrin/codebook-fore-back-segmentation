/******************************************************************************  
 *  
 * CIEDE2000 Color Difference Calculator  
 *  
 * Copyright (c) 2007 yoneh (http://d.hatena.ne.jp/yoneh/)  
 *   
 * Permission is hereby granted, free of charge, to any person obtaining a copy  
 * of this software and associated documentation files (the "Software"), to deal  
 * in the Software without restriction, including without limitation the rights  
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
 * copies of the Software, and to permit persons to whom the Software is  
 * furnished to do so, subject to the following conditions:  
 *   
 * The above copyright notice and this permission notice shall be included in  
 * all copies or substantial portions of the Software.  
 *   
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  
 * THE SOFTWARE.  
 *  
 ******************************************************************************  
 *  
 * References:  
 * [1] Gaurav Sharma, Wencheng Wu, and Edul N. Dalal, ``The CIEDE2000 Color-  
 *     Difference Formula: Implementation Notes, Supplementary Test Data,  
 *     and Mathematical Observations'', Color Research and Application, Vol. 30,  
 *     No. 1, Feb 2005.  
 *  
 ******************************************************************************/   
#ifndef LAB_CIEDE2000_H
#define LAB_CIEDE2000_H

#define _USE_MATH_DEFINES
#include <cmath>   
#include <math.h>
#include <iostream> 

using namespace std;   
   
/**  
 * Can x be assumed to be 0  
 * @param x a value  
 * @return true if x can be assumed to be 0  
 */   
inline bool tolerance_zero(const double x)   
{   
    return fabs(x)<1e-9;   
}   
   
/**  
 * cosine  
 * @param degree angle [degree]  
 * @return cosine value  
 */   
inline double cosd(const double degree)   
{   
    return cos(degree*M_PI/180.0);   
}   
   
/**  
 * sine  
 * @param degree angle [degree]  
 * @return sine value  
 */   
inline double sind(const double degree)   
{   
    return sin(degree*M_PI/180.0);   
}   
   
/**  
 * arctangent (four quadrant)  
 * @param y y-coordinate  
 * @param x x-coordinate  
 * @return angle [degree]  
 */   
inline double fqatan(const double y,const double x)   
{   
    double  t=atan2(y,x)/M_PI*180.0;   
       
    if(t<0.0)   
        t+=360.0;   
       
    return t;   
}   
   
/**  
 * Calculate f7(x)=(x^7 / (x^7+25^7))^0.5  
 * @param x a value  
 * @return f7(x)  
 */   
inline double f7(const double x)   
{   
    // if x is small, using the following approx.   
    if(x<1.0)   
        return pow(x/25.0,3.5);   
       
    return 1.0/sqrt(1.0+pow(25.0/x,7.0));   
}   
   
/**  
 * Calculate `CIEDE2000 Color Difference'  
 * @param L1 L* parameter of a color  
 * @param a1 a* parameter of a color  
 * @param b1 b* parameter of a color  
 * @param L2 L* parameter of a color  
 * @param a2 a* parameter of a color  
 * @param b2 b* parameter of a color  
 * @return `CIEDE2000 Color Difference' between (L1,a1,b1) and (L2,a2,b2)  
 */   
double CIEDE2000(const double L1,const double a1,const double b1,   
                const double L2,const double a2,const double b2);

#endif