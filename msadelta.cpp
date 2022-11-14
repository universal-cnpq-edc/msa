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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <windows.h>
#include <cstring>

#include <bitset>
#include <algorithm>


using namespace std;

const int MSIZE = 15;
const int BSSIZE = 100000;
int nn, n, dim, num_q;
int loi[MSIZE];  // MSIZE = n ?
long acc_timer;
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
void fullPermutationSearching(int * q, int * refsR);
__int64 currentTimeMillis();

string toBinaryString(int decimal);
string gammaEncode(int x);
string deltaEncode(int x);
int deltaEncode(int x, int index, int offset);
Decoded deltaDecodeDiff(int bsindex, int index);
int getInt(int bsindex, int start, int end);
int indexOf(int bsindex, int startPos);

int main() {
    
    string filestr = "input4.txt";
    
    std::fstream myfile(filestr, std::ios_base::in);
    
    myfile >> dim;
    myfile >> nn;
    myfile >> n;
    myfile >> num_q;
    
    int o[dim * nn];
    int r[dim * n];
    
    for (int i = 0; i < dim * nn; i++) {
        myfile >> o[i];
    }
    
    for (int i = 0; i < dim * n; i++) {
        myfile >> r[i];
    }
    
    __int64 t1 = currentTimeMillis();
    
    
    fullPermutationIndexing(o, r);
    
    __int64 t2 = currentTimeMillis();
    
    cout << "dim = " << dim << "; nn = " << nn << "; n = " << n << "; q = " << num_q << endl;
    cout << "(n_objs x n_refs) x 32 bits = (" << nn << " x " << n << ") x 32 bits = " << nn * n * 32 << endl;
    
    int len = 0;
    for (int i = 0; i < n; i++) {
        len += offset[i];
    }
    cout << "Encoded size (bits) = " << len << endl;
    
//    for (int i = 0; i < n; i++) {
//        cout << bsToString(i, offset[i]);
//    }   
//    cout << endl;
    cout << "Tempo para indexar (ms) = " << (t2 - t1) << endl;
    
    acc_timer = 0;
    for (int i = 0; i < num_q; i++) {
        
        int q[dim];
        for (int j = 0; j < dim; j++) {
            myfile >> q[j];
        }
        
        fullPermutationSearching(q, r);
    }
    
    cout << "Tempo para buscas (ms) = " << acc_timer << endl;
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

void fullPermutationSearching(int * q, int * refsR) {
    
    __int64 t1 = currentTimeMillis();
        
    struct Node acc[nn];
    
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
    
    std::sort(acc, acc + nn, &node_sorter);
    
    acc_timer += currentTimeMillis() - t1;
    
    for (int i = 0; i < nn; i++) {
        printf("%d ", acc[i].i);
    }
    printf("\n");
    
}


__int64 currentTimeMillis()
{
static const __int64 magic = 116444736000000000; // 1970/1/1
  SYSTEMTIME st;
  GetSystemTime(&st);
  FILETIME   ft;
  SystemTimeToFileTime(&st,&ft); // in 100-nanosecs...
  __int64 t;
  memcpy(&t,&ft,sizeof t);
  return (t - magic)/10000; // scale to millis.
}

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

    for (int i = 0; i < bin.length() - 1; i++) {
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

    for (int i = 0; i < output.length(); i++) {
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