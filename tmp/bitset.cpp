#include <iostream>
#include <cstdlib>

using namespace std;

/**/
unsigned char mask[]={0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
#define bit_set(i, b) B[(i)/8]=(b) ? (mask[(i)%8]|B[(i)/8]) : ((~mask[(i)%8])&B[(i)/8])
#define bit_get(i) ( (B[(i)/8]&mask[(i)%8]) ? 1 : 0 )
/**/

int main(int argc, char** argv){

  if(argc!=2) return 1;
  int n = atoi(argv[1]); //number of bits

  unsigned char *B = new unsigned char[n/8+1]; 

  for(int i=0; i<n; i++){
    int r = rand()%2;
    if(r==1) bit_set(i, 1);
  }

  for(int i=0; i<n; i++){
    cout<<bit_get(i);
  }
  cout<<endl;


  delete[] B;

  return 0;
}
