/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: john
 *
 * Created on September 18, 2022, 4:48 PM
 */
#ifndef DEBUG
  #define DEBUG 0 
#endif

#ifndef VERBOSE
  #define VERBOSE 1
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "external/malloc_count/malloc_count.h"


#if _WIN32
    #include <windows.h>
    typedef __int64  int_t;
#else
    typedef int64_t  int_t;
#endif


#include <cstring>

#include <bitset>
#include <algorithm>


using namespace std;

const int KNN = 10;
const int MSIZE = 15; //qtdade de bitsets (numero de referencias)
const int BSSIZE = 10000000; //tamanho de cada bitset (não sabemos o tamanho)

int nn, n, dim, num_q;
int loi[MSIZE];  // lista ordenado do rank (MSIZE = n ?)

//long acc_timer;
time_t t_total=0;
clock_t c_total=0;

bitset<BSSIZE> bs[MSIZE]; // MSIZE = n ?
int offset[MSIZE];  // MSIZE = n ?
int lastValues[MSIZE];  // MSIZE = n ?

struct Node {
    int i;
    double d;
};

struct Decoded {
    int value;
    int position;
};

int p(int rj);
void fullPermutationIndexing(int * domainD, int * refsR);
void createOrderedList(int * oi, int * r);

string bsToString(int index, int num);
bool node_sorter(Node const& lhs, Node const& rhs);
void fullPermutationSearching(int * q, int * refsR, std::fstream& f_out, int knn);
//int_t currentTimeMillis();
void time_start(time_t *t_time, clock_t *c_clock);
double time_stop(time_t t_time, clock_t c_clock);

string toBinaryString(int decimal);
string gammaEncode(int x);
string deltaEncode(int x);
int deltaEncode(int x, int index, int offset);
Decoded deltaDecodeDiff(int bsindex, int index);
int getInt(int bsindex, int start, int end);
int indexOf(int bsindex, int startPos);

int write_output(struct Node *acc, int nn, string s_out);

int main() {
     
    string filestr = "input10.txt";
    //string filestr = "input_1E5.txt";
    string s_out = "saida_fred_10.out";
    
    std::fstream myfile(filestr, std::ios_base::in);
    
    myfile >> dim;
    myfile >> nn;
    myfile >> n;
    myfile >> num_q;
    
    //int o[dim * nn];
    //int r[dim * n];
    int *o = new int[dim * nn]; //objetos
    int *r = new int[dim * nn];
    
    for (int i = 0; i < dim * nn; i++) {
        myfile >> o[i];
    }
    
    for (int i = 0; i < dim * n; i++) {
        myfile >> r[i];
    }
    
    //int_t t1 = currentTimeMillis();
    time_start(&t_total, &c_total);
    
    
    fullPermutationIndexing(o, r);
    //int_t t2 = currentTimeMillis();
    delete[] o;
    
    cout << "dim = " << dim << "; nn = " << nn << "; n = " << n << "; q = " << num_q << endl;
    cout << "(n_objs x n_refs) x 32 bits = (" << nn << " x " << n << ") x 32 bits = " << nn * n * 32 << endl;
    
    int len = 0;
    for (int i = 0; i < n; i++) {
        len += offset[i];
    }
    cout << "Encoded size (bits) = " << len << endl;

    #if DEBUG
    for (int i = 0; i < n; i++) {
        cout << bsToString(i, offset[i]);
    }   
    cout << endl;
    #endif

    //cout << "Tempo para indexar (ms) = " << (t2 - t1) << endl;
    cout << "Tempo para indexar (ms)\n" << time_stop(t_total, c_total) << endl;
    
    std::fstream f_out(s_out, std::ios_base::out);
    
    //acc_timer = 0;
    time_start(&t_total, &c_total); 
    
    for (int i = 0; i < num_q; i++) {
        
        //cout << "Query " << i + 1 << ":" << endl;
        
        int q[dim];
        for (int j = 0; j < dim; j++) {
            myfile >> q[j];
        }
        
        fullPermutationSearching(q, r, f_out, KNN);
      
    }
    f_out.close();
    delete[] r;
    
    //cout << "Tempo para buscas (ms) = " << acc_timer << endl;
    cout << "Tempo para buscas (ms)\n" << time_stop(t_total, c_total) << endl;
    cout << "***********************************************\n";
    cout << "Programa terminado com sucesso!\n";

    return 0;
}

void fullPermutationIndexing(int * domainD, int * refsR) {
    
    //bs = new BitSet[n];
    //offset = new int[n];
    //lastValues = new int[n];

    //loi = new int[n];
    
    for (int i = 0; i < nn; i++) {
        
        int oi[dim];
        for (int kk = 0; kk < dim; kk++) {
            oi[kk] = domainD[dim * i + kk];
        }
        
        createOrderedList(oi, refsR);
        
        
        for (int j = 0; j < n; j++) {
            int value = i * n + p(j) + 1;
            
            if (offset[j] == 0) {
                lastValues[j] = value;
                offset[j] = deltaEncode(value, j, offset[j]);
            } else {
                int diff = value - lastValues[j];
                lastValues[j] = value;
                offset[j] = deltaEncode(diff, j, offset[j]);
            }
        }
        
    }
    
}

int p(int rj) {
    
    for (int i = 0; i < n; i++) {
        if (loi[i] == rj) {
            return i;
        }
    }
    
    return -1;
}

void createOrderedList(int * oi, int * r) {
    
    struct Node output[n];
    double d, diff;
    
    for (int i = 0; i < n; i++) {
        
        d = 0;
        for (int kk = 0; kk < dim; kk++) {
            int oikk = oi[kk];
            int coord = r[dim * i + kk];
            diff = oi[kk] - r[dim * i + kk];
            d += diff * diff;
        }
        
        struct Node tmp = {i, d};
        output[i] = tmp;
    }
    
    // efetua a ordenação
    std::sort(output, output + n, &node_sorter);
    
    for (int i = 0; i < n; i++) {
        loi[i] = output[i].i;
    }
    
}

string bsToString(int index, int num) {

    string out = "";
    for (int i = 0; i < num; i++) {
        if (bs[index].test(i)) {
            out += '1';
        } else {
            out += '0';
        }
    }

    return out;

}

bool node_sorter(Node const& lhs, Node const& rhs) {

    if (lhs.d != rhs.d) {
        return lhs.d < rhs.d;
    }
    return lhs.i < rhs.i;

}

void fullPermutationSearching(int * q, int * refsR, std::fstream& f_out, int knn) {
    
    //int_t t1 = time_start();
        
    //struct Node acc[nn];
    struct Node *acc = new struct Node[nn];
    
    for (int j = 0; j < nn; j++) {
        struct Node tmp = {j, 0};
        acc[j] = tmp;
    }
    
    createOrderedList(q, refsR);
    
    for (int j = 0; j < n; j++) {
        
        int refPosQ = p(loi[j]);
        
        int oid = 0;
        
        int bucket = loi[j];
        
        int msa_k = 0;
        int index = 0;
        
        for (int k = loi[j] * nn; k < loi[j] * nn + nn; k++) {
            
            Decoded tmp = deltaDecodeDiff(bucket, index);
            msa_k += tmp.value;
            index = tmp.position;

            int refPos = msa_k % n;

            acc[oid].d += abs(refPosQ - refPos);

            oid++; 
            
        }
        
    }
    
    delete[] acc;
    
    #if VERBOSE
    std::sort(acc, acc + nn, &node_sorter);
    
    //for (int i = 0; i < nn; i++) {
    for (int i = 0; i < knn; i++) {
      f_out << acc[i].i << " "; 
      //printf("%d ", acc[i].i);
    }
    f_out << endl; 
    //printf("\n");
    #endif
}

void time_start(time_t *t_time, clock_t *c_clock){

  *t_time = time(NULL);
  *c_clock =  clock();
}

double time_stop(time_t t_time, clock_t c_clock){

  double aux1 = (clock() - c_clock) / (double)(CLOCKS_PER_SEC);
  double aux2 = difftime (time(NULL),t_time);

  printf("CLOCK = %lf TIME = %lf\n", aux1, aux2);

  return aux1;
}

/**
int_t currentTimeMillis()
{
static const int_t magic = 116444736000000000; // 1970/1/1
  SYSTEMTIME st;
  GetSystemTime(&st);
  FILETIME   ft;
  SystemTimeToFileTime(&st,&ft); // in 100-nanosecs...
  int_t t;
  memcpy(&t,&ft,sizeof t);
  return (t - magic)/10000; // scale to millis.
}
**/

string toBinaryString(int decimal) {
    if (decimal == 0) return "0";
    int rem;
    string binary = "";
    
    while (decimal != 0) {
        rem = decimal % 2;
        binary = to_string(rem) + binary;
        decimal /= 2;
    }
    return binary;
}

string gammaEncode(int x) {

    string bin = toBinaryString(x);

    string out = "";

    for (long unsigned i = 0; i < bin.length() - 1; i++) {
        out += "0";
    }

    return out + bin;
}

string deltaEncode(int x) {

    string bin = toBinaryString(x);
    string out = gammaEncode(bin.length());

    out += bin.substr(1);

    return out;

}

int deltaEncode(int x, int index, int offset) {

    string output = deltaEncode(x);

    for (long unsigned i = 0; i < output.length(); i++) {
        if (output[i] == '1') {
            bs[index].set(offset + i);
        }
    }

    return offset + output.length();
}

Decoded deltaDecodeDiff(int bsindex, int index) {

    int pos = indexOf(bsindex, index) - index;
    pos += pos;

    int nPlus1 = getInt(bsindex, index, index + pos + 1);
    int n0 = nPlus1 - 1;

    int d = getInt(bsindex, index + pos + 1, index + pos + n0 + 1);
    int number = 1 << n0;
    number += d;

    if (index == 0) {
        struct Decoded tmp = {number - 1, index + pos + n0 + 1};
        return tmp;
    }

    struct Decoded tmp = {number, index + pos + n0 + 1};
    return tmp;

}

int getInt(int bsindex, int start, int end) {
    int len = end - start;
    int acc = 0;
    for (int i = 0; i < len; i++) {
        if (bs[bsindex].test(end - i - 1)) {
            acc += (1 << i);
        }
    }
    return acc;
}

int indexOf(int bsindex, int startPos) {
    for (int i = startPos; i < offset[bsindex]; i++) {
        if (bs[bsindex].test(i)) {
            return i;
        }
    }
    return -1;
}

