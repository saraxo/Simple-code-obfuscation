#include <iostream>
using namespace std;


int main(){
    int a=5,b=10,c=15,d=-3,e=-6,f=25,g=32,h=14,k=59,l=16,m=2,n=33,o=46,p=222,r=7,s=11,t=23,x=2000,y=213,w=6,z=-20,u=17,v=-10,j=52,i=-7,q=4;

        a=b+c+w;
        g=h+k;
        k=l+m;

        for(int i = 1;i<10;i++){
            r=g+z;
            s=m+t;
            e=o+l;
            f=e+h+k;
        }

        d=r+s;
        b=c+e;
        m=k+o;
        p=m+s+k;

      for(int i = 0; i< 5;i++){
            o=k+m;
            l=z+t;
        }

        h=b+p+r;
        y=h+w;
        w=x+f;
        n=h+s;

      for(int i=7;i<12;i++){
            q=u+v+y;
            i=q+j;
      }

     cout<<"Ispis varijabli: "<<endl;
     cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<" "<<g<<endl;
     cout<<h<<" "<<k<<" "<<l<<" "<<m<<" "<<n<<" "<<o<<" "<<p<<endl;
     cout<<r<<" "<<s<<" "<<t<<" "<<x<<" "<<y<<" "<<w<<" "<<z<<endl;
     cout<<u<<" "<<v<<" "<<j<<" "<<i<<" "<<q<<endl;

}

