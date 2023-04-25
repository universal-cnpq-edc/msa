/* TODO:
 * > remover variaveis globais - OK
 * > mudar entrada para (dim, refs, objetos, consultas) - OK
 * > ler a entrada por partes (1 objeto por vez e construir o indice) - OK
 * > criar CLI - OK
 * > mudar o bitset para vector<bool> - OK
 * > inverter looping busca (bucket->Oid para Oid->bucket) - OK
 * > usar PQ para evitar ACC[1..n] - OK
 * 
 * > usar sdsl para o bitvector
 * > usar sdsl para Elias?
 * 
 */

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

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <cfloat>
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

struct Node {
    int i;
    double d;
};

struct CompareNode {
    bool operator()(Node const& a, Node const& b){
        if (a.d != b.d) return a.d < b.d;
        return a.i < b.i;
    }
};

typedef priority_queue<Node, vector<Node>, CompareNode> priority_queue_Node;

struct Decoded {
    int value;
    int position;
};

int p(int* loi, int rj, int refs);
void fullPermutationIndexing(int refs, int dim, int n, int * refsR, int *offset, int *lastValues, std::fstream& f_in, vector<bool> *BS);
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, int *M);

void createOrderedList(int refs, int dim, int* oi, int* loi, int* r);

string bsToString(int index, int num, vector<bool> *BS);
bool node_sorter(Node const& lhs, Node const& rhs);
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int* offset, vector<bool> *BS, priority_queue_Node &PQ, int knn);
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int *M, priority_queue_Node &PQ, int knn);

void time_start(time_t *t_time, clock_t *c_clock);
double time_stop(time_t t_time, clock_t c_clock);

string toBinaryString(int decimal);
string gammaEncode(int x);
string deltaEncode(int x);
int deltaEncode(int x, int index, int offset_value, vector<bool> *BS);
Decoded deltaDecodeDiff(int bsindex, int index, int offset_value, vector<bool> *BS);
int getInt(int bsindex, int start, int end, vector<bool> *BS);
int indexOf(int bsindex, int startPos, int offset_value, vector<bool> *BS);

int write_output(struct Node *acc, int n, std::fstream& f_out, int knn, priority_queue_Node &PQ);

int cMSA(string s_in, string s_out, int output, int verbose, int time, int knn);
int MSA(string s_in, string s_out, int output, int verbose, int time, int knn);

void push_pq_fixed_size(priority_queue_Node &PQ, Node tmp, int knn);
void init_pq_fixed_size(priority_queue_Node &PQ, int knn);


void usage(char *name){
    puts("Available options:");
    puts("\t-A a\talgorithm to use (default 2)");
    puts("\t-o\toutput computed file");
    puts("\t-t\ttime (seconds)");
    puts("\t-k\tknn-queries");
    puts("\t-v\tverbose output");
    puts("Debug options:");
    puts("\t-h\tthis help message");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {

    /**/
    extern char *optarg;
    extern int optind; //, opterr, optopt;
  
    int c=0, verbose=0, time=0, output=0, knn=0;
    int ALG=2;//Algorithm
  
    while ((c=getopt(argc, argv, "A:ovtk:h")) != -1) {
      switch (c)
      {
        case 'A':
          ALG=(size_t)atoi(optarg); 
          break;
        case 'o':
          output++; break;
        case 'v':
          verbose++; break;
        case 't':
          time++; break;
        case 'k':
          knn=(size_t)atoi(optarg);  break;
        case 'h':
          usage(argv[0]); break;       // show usage and stop
        case '?':
          exit(EXIT_FAILURE);
      }
    }
    free(optarg);
    
    string s_in;
    if(optind+1==argc)
        s_in = argv[optind++];
    else 
        usage(argv[0]);
    /**/
    
    string s_out;
    if(output){
      size_t i = s_in.rfind('.', s_in.length());
      if(verbose)
        s_out = s_in.substr(0, i) + ".A"+ to_string(ALG) + ".out";
      else
        s_out = s_in.substr(0, i) + ".out";
    }
    
    switch(ALG){
        /****/
        case 1: 
            cout<<"## MSA ##"<<endl;//no compression
            MSA(s_in, s_out, output, verbose, time, knn);
            break;
        case 2: 
            cout<<"## cMSA ##"<<endl;
            cMSA(s_in, s_out, output, verbose, time, knn);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
    }
    return 0;
}

int cMSA(string s_in, string s_out, int output, int verbose, int time, int knn){

    time_t t_start=0;
    clock_t c_start=0;
    
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    
    int dim, refs, n, num_q;
    
    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    int *r = new int[dim * refs];    
    for (int i = 0; i < dim * refs; i++) f_in >> r[i]; //referencias
    
    int *offset = new int[refs]; 
    int *lastValues = new int[refs];
    
    //bitset<BSSIZE> *bs = new bitset<BSSIZE>[refs]; // MSIZE = n ?
    vector<bool> *BS = new vector<bool>[refs];
    
    /*
    int *o = new int[dim * n]; //objetos
    for (int i = 0; i < dim * n; i++) f_in >> o[i]; 
    */
    
    if(time) time_start(&t_start, &c_start);
    
    fullPermutationIndexing(refs, dim, n, r, offset, lastValues, f_in, BS);
    
    /**/
    //delete[] o;
    delete[] lastValues;
    /**/

    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      cout << "Indexing:" << endl;
      time_stop(t_start, c_start);
    }
    
    int len = 0;
    for (int i = 0; i < refs; i++) {
        len += offset[i];
    }
    cout << "Encoded size (bits) = " << len << endl;
    cout << "########" <<endl;

    #if DEBUG
    for (int i = 0; i < refs; i++) {
        cout << bsToString(i, offset[i]);
    }   
    cout << endl;
    #endif
    
    if(knn>0){//knn queries
      
       
        std::fstream f_out;
        if(output){
          f_out.open(s_out, std::ios_base::out);  
          if(!f_out.is_open()) exit(EXIT_FAILURE);
          if(verbose) cout << s_out <<endl;
        }
               
        if(time) time_start(&t_start, &c_start);
        
        for (int i = 0; i < num_q; i++) {

            int q[dim];
            for (int j = 0; j < dim; j++) {
                f_in >> q[j];
            }
            //struct Node *acc = new struct Node[n];//?ok
            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);
            
            fullPermutationSearching(refs, dim, n, q, r, acc, offset, BS, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }
        
        if(output) f_out.close();
        
        if(time){
          cout << "Searching:" << endl;
          time_stop(t_start, c_start);
        }
    }

    delete[] r;
    delete[] offset;
    //delete[] bs;
    delete[] BS;    
    f_in.close();
    
    return 0;
}

int MSA(string s_in, string s_out, int output, int verbose, int time, int knn){

    time_t t_start=0;
    clock_t c_start=0;
    
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);
    
    int dim, refs, n, num_q;
    
    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    int *r = new int[dim * refs];    
    for (int i = 0; i < dim * refs; i++) f_in >> r[i]; //referencias
    
    //vector<int> *M = new vector<int>[refs];
    int *M = new int[n * refs];
    
    if(time) time_start(&t_start, &c_start);
    
    fullPermutationIndexing(refs, dim, n, r, f_in, M);

    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      cout << "Indexing:" << endl;
      time_stop(t_start, c_start);
    }
    
    cout << "Encoded size (bits) = " << n * refs * 32 << endl;
    cout << "########" <<endl;
    
    if(knn>0){//knn queries
              
        std::fstream f_out;
        if(output){
          f_out.open(s_out, std::ios_base::out);  
          if(!f_out.is_open()) exit(EXIT_FAILURE);
          if(verbose) cout << s_out <<endl;
        }
               
        if(time) time_start(&t_start, &c_start);
        
        for (int i = 0; i < num_q; i++) {

            int q[dim];
            for (int j = 0; j < dim; j++) {
                f_in >> q[j];
            }
            //struct Node *acc = new struct Node[n];//?ok
            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);
            
            
            fullPermutationSearching(refs, dim, n, q, r, acc, M, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }
        
        if(output) f_out.close();
        
        if(time){
          cout << "Searching:" << endl;
          time_stop(t_start, c_start);
        }
    }

    delete[] r;
    delete[] M;
    f_in.close();
    
    return 0;
}

void fullPermutationIndexing(int refs, int dim, int n, int* refsR, int *offset, int *lastValues, std::fstream& f_in, vector<bool> *BS) {
    
    //bs = new BitSet[n];
    //offset = new int[n];
    //lastValues = new int[n];

    int *loi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    for (int i = 0; i < n; i++) {
        
        //int oi[dim];
        for (int kk = 0; kk < dim; kk++) {
            //oi[kk] = domainD[dim * i + kk]; //objeto da lista
            f_in >> oi[kk];
        }
        
        createOrderedList(refs, dim, oi, loi, refsR);
        
          
        for (int j = 0; j < refs; j++) {
            int value = i * refs + p(loi, j, refs) + 1;
            
            if (offset[j] == 0) {//proprio valor
                lastValues[j] = value;
                offset[j] = deltaEncode(value, j, offset[j], BS);
            } else { //um lastValues e offset para cada referencia
                int diff = value - lastValues[j];
                lastValues[j] = value;
                offset[j] = deltaEncode(diff, j, offset[j], BS);
            }
        }
        
    }
    delete[] oi;
    delete[] loi;
    
}

void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, int* M) {
    
    int *loi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    for (int i = 0; i < n; i++) {
        
        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }
        
        createOrderedList(refs, dim, oi, loi, refsR);
        
        for (int j = 0; j < refs; j++) {
            M[j * n + i] = i * refs + p(loi, j, refs);
        }
        
    }
    delete[] oi;
    delete[] loi;
    
}

int p(int* loi, int rj, int refs) {
    
    for (int i = 0; i < refs; i++) {
        if (loi[i] == rj) {
            return i;
        }
    }
    
    return -1;
}

void createOrderedList(int refs, int dim, int* oi, int* loi, int* r) {
    
    struct Node *output = new struct Node[refs];
    double d, diff;
    
    for (int i = 0; i < refs; i++) {
        
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
    std::sort(output, output + refs, &node_sorter);
    
    for (int i = 0; i < refs; i++) {
        loi[i] = output[i].i;
    }
    
    delete[] output;
}

string bsToString(int index, int num, vector<bool> *BS) {

    string out = "";
    for (int i = 0; i < num; i++) {
        //if (bs[index].test(i)) {
        if (BS[index][i]==1) {
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

void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int* offset, vector<bool> *BS, priority_queue_Node &PQ, int knn) {
    
    //int_t t1 = time_start();
    /*
    for (int j = 0; j < n; j++) {
        struct Node tmp = {j, 0.0};
        acc[j] = tmp;
    }
    */
    
    
    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);
    
    /*
    for (int j = 0; j < refs; j++) {
        
        int refPosQ = p(loi, loi[j], refs);
        int oid = 0;
        int bucket = loi[j];
        
        int msa_k = 0;
        int index = 0;
        
        for (int k = loi[j] * n; k < loi[j] * n + n; k++) {
            
            Decoded tmp = deltaDecodeDiff(bucket, index, offset[bucket], BS);
            msa_k += tmp.value;
            index = tmp.position;

            int refPos = msa_k % refs;
            acc[oid].d += abs(refPosQ - refPos);
            oid++; 
        }
    }
    */
    /**/
    int *MSA_k = new int[refs];
    int *Index = new int[refs];
    for (int j = 0; j < refs; j++) Index[j] = MSA_k[j] = 0;
    /**/
    
    for (int oid = 0; oid < n; oid++) {
      double Acc = 0.0;
      for (int j = 0; j < refs; j++) {
        
        int refPosQ = p(loi, loi[j], refs);//?
        int bucket = loi[j];
        
        //int k = loi[j] * n + oid;
       
        Decoded tmp = deltaDecodeDiff(bucket, Index[j], offset[bucket], BS);
        MSA_k[j] += tmp.value;
        Index[j] = tmp.position;

        int refPos = MSA_k[j] % refs;
        Acc += abs(refPosQ - refPos);
      }
      
      struct Node tmp = {oid, Acc};
      push_pq_fixed_size(PQ, tmp, knn);                
      //acc[oid].d = Acc; 
    }
    
    delete[] MSA_k;
    delete[] Index;
    
    delete[] loi;
}

void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int* M, priority_queue_Node &PQ, int knn) {
    
    //int_t t1 = time_start();
    /*
    for (int j = 0; j < n; j++) {
        struct Node tmp = {j, 0.0};
        acc[j] = tmp;
    }
    */
    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    /*
    for (int j = 0; j < refs; j++) {
        int refPosQ = j;
        int oid = 0;
        for (int k = loi[j] * n; k < loi[j] * n + n; k++) {
            int refPos = M[k] % refs;
            acc[oid].d += abs(refPosQ - refPos);
            oid++; 
        }
    }
    */
    
    for (int oid = 0; oid < n; oid++) {
        double Acc = 0.0;
        for( int j = 0; j < refs; j++) {
        
          int refPosQ = j;
          int k = loi[j] * n + oid;
          int refPos = M[k] % refs;
          //acc[oid].d += abs(refPosQ - refPos);
          Acc += abs(refPosQ - refPos);
        }
        
        struct Node tmp = {oid, Acc};
        push_pq_fixed_size(PQ, tmp, knn);                
        //acc[oid].d = Acc;
    }
    
    delete[] loi;
}

//keep PQ with maximum knn elements
void push_pq_fixed_size(priority_queue_Node &PQ, Node tmp, int knn){
/*
  if(PQ.size() < (long unsigned) knn){
    PQ.push(tmp);
  }
  else{
    if(tmp.d < PQ.top().d){
      PQ.pop();
      PQ.push(tmp);
    }
  }  
*/
  if(tmp.d < PQ.top().d){
    PQ.pop();
    PQ.push(tmp);
  }
}

void init_pq_fixed_size(priority_queue_Node &PQ, int knn){

  struct Node tmp = {0, DBL_MAX};
  for( int i=0; i < knn; i++){
      PQ.push(tmp);
  }
}

void time_start(time_t *t_time, clock_t *c_clock){

  *t_time = time(NULL);
  *c_clock =  clock();
}

double time_stop(time_t t_time, clock_t c_clock){

  double aux1 = (clock() - c_clock) / (double)(CLOCKS_PER_SEC);
  double aux2 = difftime (time(NULL),t_time);

  printf("CLOCK = %lf TIME = %lf\n", aux1, aux2);
  printf("########\n");

  return aux1;
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

int deltaEncode(int x, int index, int offset_value, vector<bool> *BS) {

    string output = deltaEncode(x);

    for (long unsigned i = 0; i < output.length(); i++) {
        BS[index].push_back(output[i]=='1'?1:0);
    }

    return offset_value + output.length();
}

Decoded deltaDecodeDiff(int bsindex, int index, int offset_value, vector<bool> *BS) {

    int pos = indexOf(bsindex, index, offset_value, BS) - index;
    pos += pos;

    int nPlus1 = getInt(bsindex, index, index + pos + 1, BS);
    int n0 = nPlus1 - 1;

    int d = getInt(bsindex, index + pos + 1, index + pos + n0 + 1, BS);
    int number = 1 << n0;
    number += d;

    if (index == 0) {
        struct Decoded tmp = {number - 1, index + pos + n0 + 1};
        return tmp;
    }

    struct Decoded tmp = {number, index + pos + n0 + 1};
    return tmp;

}

int getInt(int bsindex, int start, int end, vector<bool> *BS) {
    int len = end - start;
    int acc = 0;
    for (int i = 0; i < len; i++) {
        if (BS[bsindex][end - i - 1]==1) {
            acc += (1 << i);
        }
    }
    return acc;
}

int indexOf(int bsindex, int startPos, int offset_value, vector<bool> *BS) {
    for (int i = startPos; i < offset_value; i++) {
        if (BS[bsindex][i]==1) {
            return i;
        }
    }
    return -1;
}
 
int write_output(struct Node *acc, int n, std::fstream& f_out, int knn, priority_queue_Node &PQ){
    
    /*
    std::sort(acc, acc + n, &node_sorter);    
    //for (int i = 0; i < n; i++) {
    int k = min(knn,n);
    for (int i = 0; i < k; i++) {
      f_out << acc[i].i << " "; 
    }
    f_out << endl; 
    */
    //invert the PQ
    stack<Node> S;
    while (!PQ.empty()) {
      S.push(PQ.top());
      PQ.pop();
    }
    while(!S.empty()){
        Node acc = S.top();
        S.pop();
        f_out << acc.i << " ";
    }
    f_out << endl;
  
return 0;
}
