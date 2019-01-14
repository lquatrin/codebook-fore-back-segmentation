#include "CIEDE2000.h"

double CIEDE2000(const double L1,const double a1,const double b1,   
                const double L2,const double a2,const double b2)   
{   
    const double epsilon=1e-9;   
    double  C1ab,C2ab;   
    double  Cab,G;   
    double  a1_,a2_;   
    double  C1_,C2_;   
    double  h1_,h2_;   
       
    C1ab=sqrt(a1*a1+b1*b1);   
    C2ab=sqrt(a2*a2+b2*b2);   
    Cab=(C1ab+C2ab)/2.0;   
    G=0.5*(1.0-f7(Cab));   
    a1_=(1.0+G)*a1;   
    a2_=(1.0+G)*a2;   
    C1_=sqrt(a1_*a1_+b1*b1);   
    C2_=sqrt(a2_*a2_+b2*b2);   
    
    if(tolerance_zero(a1_) && tolerance_zero(b1))   
        h1_=0.0;   
    else   
        h1_=fqatan(b1,a1_);   
    if(tolerance_zero(a2_) && tolerance_zero(b2))   
        h2_=0.0;   
    else   
        h2_=fqatan(b2,a2_);   
       
    // Calculate dL', dC', and dH'   
    double  dL_,dC_,dH_,dh_;   
    double  C12;   
       
    dL_=L2-L1;   
    dC_=C2_-C1_;   
    C12=C1_*C2_;   
    if(tolerance_zero(C12))   
    {   
        dh_=0.0;   
    }   
    else   
    {   
        double  tmp=h2_-h1_;   
           
        if(fabs(tmp)<=180.0+epsilon)   
            dh_=tmp;   
        else if(tmp>180.0)   
            dh_=tmp-360.0;   
        else if(tmp<-180.0)   
            dh_=tmp+360.0;   
    }   
    dH_=2.0*sqrt(C12)*sind(dh_/2.0);   
       
       
    // Calculate L', C', h', T, and dTh   
    double  L_,C_,h_,T,dTh;   
       
    L_=(L1+L2)/2.0;   
    C_=(C1_+C2_)/2.0;   
    if(tolerance_zero(C12))   
    {   
        h_=h1_+h2_;   
    }   
    else   
    {   
        double  tmp1=fabs(h1_-h2_);   
        double  tmp2=h1_+h2_;   
           
        if(tmp1<=180.0+epsilon)   
            h_=tmp2/2.0;   
        else if(tmp2<360.0)   
            h_=(tmp2+360.0)/2.0;   
        else if(tmp2>=360.0)   
            h_=(tmp2-360.0)/2.0;   
    }   
    T=1.0-0.17*cosd(h_-30.0)+0.24*cosd(2.0*h_)   
        +0.32*cosd(3.0*h_+6.0)-0.2*cosd(4.0*h_-63.0);   
    dTh=30.0*exp(-pow((h_-275.0)/25.0,2.0));   
       
       
    // Calculate RC, SL, SC, SH, and RT   
    double  RC,SL,SC,SH,RT;   
    double  L_2=(L_-50.0)*(L_-50.0);   
       
    RC=2.0*f7(C_);   
    SL=1.0+0.015*L_2/sqrt(20.0+L_2);   
    SC=1.0+0.045*C_;   
    SH=1.0+0.015*C_*T;   
    RT=-sind(2.0*dTh)*RC;   
       
       
    //Calculate dE00   
    const double kL = 1.f; // These are proportionally coefficients   
    const double kC = .8f; // and vary according to the condition.   
    const double kH = .8f; // These mostly are 1.   
    double LP=dL_/(kL*SL);   
    double CP=dC_/(kC*SC);   
    double HP=dH_/(kH*SH);   
       
    return sqrt(LP*LP+CP*CP+HP*HP+RT*CP*HP);   
}   