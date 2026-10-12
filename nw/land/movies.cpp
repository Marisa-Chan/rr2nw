#include "_land.h"

inline double AddMRel(int xm,int ym){

    int xr=xm&255;
    int yr=ym&255;
    //ind=ROR(*(maps[0]->mapadr+()+xm)),11);

    return (*(relMap[0].mapadr+(yr<<8)+xr))/100.;
}

double Height(int x,int y){

    x>>=2;
    y>>=2;

    double h0,h1,h2,h3;
    double lambda;                   //        3              0
                                     //           ----------
                                     //    Y    |        / |
    int crdX0=((x>>8)+1)&511;        //      ³   |      /   |
    int crdX1=((x>>8)+1)&511;        //      ³   |    /     |
    int crdX2=(x>>8)&511;            //      ³   |  /       |
    int crdX3=(x>>8)&511;            //      ³   |/         |
                                     //      ³ 2  ----------  1
    int crdY0=((y>>8)+1)&511;        //      ÀÄÄÄÄÄÄÄÄÄÄÄÄ
    int crdY1=(y>>8)&511;            //                   X
    int crdY2=((y>>8))&511;
    int crdY3=((y>>8)+1)&511;

    h0 = (map.mapadr[crdX0+(crdY0<<9)]*madsh.hratio);
    h1 = (map.mapadr[crdX1+(crdY1<<9)]*madsh.hratio);
    h2 = (map.mapadr[crdX2+(crdY2<<9)]*madsh.hratio);
    h3 = (map.mapadr[crdX3+(crdY3<<9)]*madsh.hratio);

    if ((x&255)>=(y&255)){

        lambda=(256.0-(double)(x&255))/((double)(x&255)+.001);
        h1=(h1+lambda*h2)/(.001+lambda);

        h0=(h0+lambda*h2)/(.001+lambda);

        lambda=((double)(y&255)+1)/((double)(x&255)-(double)(y&255)+.001);
        h1=(h1+lambda*h0)/(.001+lambda);
        return (h1+AddMRel(x,y));

	}
	else{

	    lambda=(double)(x&255)/(256.0-(double)(x&255));
            h3=(h3+lambda*h0)/(lambda+.001);

	    h2=(h2+lambda*h0)/(lambda+.001);

            lambda=(256.0-(double)(y&255))/((double)(y&255)-(double)(x&255));
            h3=(h3+lambda*h2)/(.001+lambda);
            return (h3+AddMRel(x,y));

	}
}

