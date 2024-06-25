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
   SDSL install instructions
   git clone https://github.com/simongog/sdsl-lite.git
   cd sdsl-lite
   ./install.sh
 */

#ifndef DEBUG
  #define DEBUG 0
#endif

#ifndef BOOST
  #define BOOST 0
#endif

#ifndef SDSL
  #define SDSL 1
#endif

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <cfloat>  
#include <map>
#include "external/malloc_count/malloc_count.h"
#if BOOST
  #include "boost/dynamic_bitset.hpp"
#endif
#if SDSL
  #include <sdsl/int_vector.hpp>
  #include <sdsl/wavelet_trees.hpp>
  using namespace std;
  using namespace sdsl;
  //typedef wt_int<> wt_sdsl;
  //typedef wt_ap<> wt_sdsl;
  typedef wt_gmr<> wt_sdsl; //-> OK
#endif

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

struct Parameter {
    int m;
    int p;
};

int p(int* loi, int rj, int refs);
void fullPermutationIndexing(int refs, int dim, int n, int * refsR, std::fstream& f_in, vector<bool> *BS);
void fullPermutationIndexingCounter(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * offset); // <<----- unsigned char array (A5)
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char * * BARR); // <<----- unsigned char array (A5)
void fullPermutationIndexingA9(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char *BARR, long long int * shiftArr); // <<----- unsigned char array v3 (A9)
void fullPermutationIndexingA6(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char * * BARR, int block_size); // <<----- cMSA Diff (A6)
#if BOOST
void fullPermutationIndexingBoost(int refs, int dim, int n, int* refsR, std::fstream& f_in, boost::dynamic_bitset<> * DBS); // <<----- boost (A4)
#endif
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned int * M); // <<----- MSA (A1)
void fullPermutationIndexing_m(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * M); // <<----- MSA melhorado (A8)
void fullSequentialIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * * O); // <<----- Naive (A0)

void createOrderedList(int refs, int dim, int* oi, int* loi, int* R);
void createPositionList(int refs, int dim, int* oi, int* lpi, int* R);

string bsToString(int index, int num, vector<bool> *BS);
bool node_sorter(Node const& lhs, Node const& rhs);

void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, vector<bool> *BS, priority_queue_Node &PQ, int knn);
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * * BARR, priority_queue_Node &PQ, int knn); // <<----- unsigned char array (A5)
void fullPermutationSearchingA9(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * BARR, priority_queue_Node &PQ, int knn, long long int * shiftArr); // <<----- unsigned char array v3 (A9)
void fullPermutationSearchingA6(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * * BARR, priority_queue_Node &PQ, int knn, int block_size); // <<----- cMSA Diff (A6)
#if BOOST
void fullPermutationSearchingBoost(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, boost::dynamic_bitset<> * DBS, priority_queue_Node &PQ, int knn); // <<----- boost (A4)
#endif
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned int * M, priority_queue_Node &PQ, int knn); // <<----- MSA (A1)
#if SDSL
void fullPermutationSearching_sdsl(int refs, int dim, int n, int * q, int * refsR, struct Node *acc,  unsigned int * M, wt_sdsl &WT, priority_queue_Node &PQ, int knn); // <<----- MSA (A10)
#endif
void fullPermutationSearching_m(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int * M, priority_queue_Node &PQ, int knn); // <<----- MSA melhorado (A8)
void fullSequentialSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int * * O, priority_queue_Node &PQ, int knn); // <<----- Naive (A0)

void time_start(time_t *t_time, clock_t *c_clock);
double time_stop(time_t t_time, clock_t c_clock);


unsigned int mask[]={0x01, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000};  
unsigned char mask2[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

string toBinaryString(int decimal);
string gammaEncode(int x);

int deltaEncodeInt(int x);
int deltaEncode(int x, int index, vector<bool> *BS);
#if BOOST
int deltaEncodeBoost(int x, int index, boost::dynamic_bitset<> * DBS); // <<----- boost (A4)
#endif
int deltaEncode(int x, int index, unsigned char * * BARR, int * offset); // <<----- unsigned char array (A5)
int deltaEncodeA9(int x, int index, unsigned char * BARR, int * offset, long long int * shiftArr); // <<----- unsigned char array v3 (A9)
int deltaEncodeA6(int x, int index, unsigned char * * BARR, int * offset, int block_size); // <<----- cMSA Diff (A6)
Decoded deltaDecodeDiff(int bsindex, int index, vector<bool> *BS);

#if BOOST
Decoded deltaDecodeDiffBoost(int bsindex, int index, boost::dynamic_bitset<> * DBS); // <<----- boost (A4)
#endif

Decoded deltaDecodeDiff(int bsindex, int index, unsigned char * * BARR); // <<----- unsigned char array (A5)
Decoded deltaDecodeDiffA9(int bsindex, int index, unsigned char * BARR, long long int * shiftArr); // <<----- unsigned char array v3 (A9)
Decoded deltaDecodeDiffA6(int bsindex, int index, unsigned char * * BARR, int block_size); // <<----- cMSA Diff (A6)
int getInt(int bsindex, int start, int end, vector<bool> *BS);
int indexOf(int bsindex, int startPos, int offset_value, vector<bool> *BS);

string eliasDeltaEncode(int num);

#define bit_len(n) floor(log(n)/log(2))+1

int write_output(struct Node *acc, int n, std::fstream& f_out, int knn, priority_queue_Node &PQ);

int cMSA(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q);
#if BOOST
int cMSAdeltaBoost(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- boost (A4)
#endif
int cMSAdelta_v2(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- unsigned char array (A5)
int cMSAdelta_v3(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- unsigned char array v3 (A9)
int cMSA_diff(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- cMSA Diff (A6)
int MSA(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- MSA original (A1)
int cMSA_sdsl(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- MSA sdsl (A10)
int MSA_m(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- MSA melhorado (A8)
int naive(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q);

void push_pq_fixed_size(priority_queue_Node &PQ, Node tmp, int knn);
void init_pq_fixed_size(priority_queue_Node &PQ, int knn);

/* SIMPLE-9 (A3) */
int cMSAs(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q);
int cMSA_Simple9(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // <<----- SIMPLE-9 caontagem previa (A7)
void fullPermutationIndexingSIMPLE9(int refs, int dim, int n, int* refsR, std::fstream& f_in);
void fullPermutationIndexingSIMPLE9(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * * SIMPlocal); // <<----- SIMPLE-9 caontagem previa (A7)
void fullPermutationIndexingSIMPLE9counter(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * COUNTERlocal); // <<----- SIMPLE-9 caontagem previa (A7)
int getCode(int * x, int k, int m, int p);
Parameter getMP(int * x, int k);
int binaryLength(int xk);
int deltaLength(int n);
void fullPermutationSearchingSIMPLE9(int refs, int dim, int n, int * q, int * refsR, priority_queue_Node &PQ, int knn);
void fullPermutationSearchingSIMPLE9(int refs, int dim, int n, int * q, int * refsR, priority_queue_Node &PQ, int knn, int * * SIMPlocal); // <<----- SIMPLE-9 caontagem previa (A7)
int retrieve(int bucket, int j);
int retrieve(int bucket, int j, int * * SIMPlocal, int * INT_ID_local, int * INT_NEXT_POS_local); // <<----- SIMPLE-9 caontagem previa (A7)

//=================================
// Sequential scan (exact search)
//#define MANHATTAN
#define EUCLIDEAN
//#define DEBUG_EXACT_SEARCH
class result;
float distance(int *o1, int *o2, int dim);
int exact(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q); // exact search
int write_output(std::fstream& f_out, result *r);
//=================================

/* ********** VARIAVEIS GLOBAIS SIMPLE-9 ******** */
int CN = 30;
vector<int *> CBUFFER;
int * CBPTR;
int * LASTPTR;
int * ACCUM;
vector<vector<int>> SIMP;

int * INT_ID;
int * INT_NEXT_POS;

/**************************************************/

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

    int c=0, verbose=0, time=0, output=0, knn=0, r=0, num_o=0, sel_q=0;
    int ALG=2;//Algorithm

    while ((c=getopt(argc, argv, "A:ovtk:r:n:q:h")) != -1) {
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
        case 'r':
          r=(size_t)atoi(optarg);  break;
        case 'n':
          num_o=(size_t)atoi(optarg);  break;
        case 'q':
          sel_q=(size_t)atoi(optarg);  break;
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
        case -1:
            cout<<"## sequential scan (exact) search ##"<<endl;//no compression
            exact(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 0:
            cout<<"## naive ##"<<endl;//no compression
            naive(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 1:
            cout<<"## MSA ##"<<endl;//no compression
            MSA(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 2:
            cout<<"## cMSA DELTA (vector) ##"<<endl;
            cMSA(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 3:
            cout<<"## cMSA SIMPLE-9 ##"<<endl;
            cMSAs(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 4:
            cout<<"## cMSA DELTA (boost) ##"<<endl;
            #if BOOST
            cMSAdeltaBoost(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            #else
            cout << "Not supported" <<endl;
            #endif
            break;
        case 5:
            cout<<"## cMSA DELTA (char array) ##"<<endl;
            cMSAdelta_v2(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 6:
            cout<<"## cMSA Diff (char array) ##"<<endl;
            cMSA_diff(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 7:
            cout<<"## cMSA SIMPLE-9 (prev count array) ##"<<endl;
            cMSA_Simple9(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 8:
            cout<<"## MSA melhorado ##"<<endl;//no compression
            MSA_m(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 9:
            cout<<"## cMSA DELTA (char array) v3 ##"<<endl;
            cMSAdelta_v3(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        case 10:
            cout<<"## cMSA SDSL (wavelet tree) ##"<<endl;
            
            #if SDSL
            cMSA_sdsl(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            #else
            cout << "Not supported" <<endl;
            #endif
            
//            #if SDSL
//            cMSA_sdsl(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
//            #else
//            cout << "Not supported" <<endl;
//            #endif
            break;
        default:
            exit(EXIT_FAILURE);
            break;
    }
    return 0;
}

// MSA com SIMPLE-9
int cMSAs(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);


    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
//    if (r != 0) {
//        if (r > refs) n = n - (r - refs);
//        refs = r;
//    }
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) {
        f_in >> R[i]; //referencias
    }

    //int *offset = new int[refs];

    //bitset<BSSIZE> *bs = new bitset<BSSIZE>[refs]; // MSIZE = n ?
    //vector<bool> *BS = new vector<bool>[refs];

    
    
    /*
    int *o = new int[dim * n]; //objetos
    for (int i = 0; i < dim * n; i++) f_in >> o[i];
    */
    
    /*************************************************************/
    /* inicializa CBUFFER e outros bagulhos para o SIMPLE-9      */
    for (int i = 0; i < refs; i++) {
        int * tmp = new int[CN];
        CBUFFER.push_back(tmp);
    }
    
    CBPTR = new int[ refs ];
    LASTPTR = new int[ refs ];
    ACCUM = new int[ refs ];
    
    for (int j = 0; j < refs; j++) {
        CBPTR[j] = LASTPTR[j] = ACCUM[j] = 0;
    }
    
    // inicialiação de vector<vector<int>>
    for (int i = 0; i < refs; i++) {
        vector<int> tmp = {};
        SIMP.push_back(tmp);
    }
    /*************************************************************/

    if(time) time_start(&t_start, &c_start);

    fullPermutationIndexingSIMPLE9(refs, dim, n, R, f_in);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados

    
//    for (int i = 0; i < refs; i++) {
//        for (int j = 0; j < (int) SIMP.at(i).size(); j++) {
//            printf("SIMP[%d][%d] = %d\n", i, j, SIMP.at(i).at(j));
//        }
//    }

    /**/
    //delete[] o;
    //delete[] lastValues;
    /**/

    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      printf("A3 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

//    int len = 0;
//    for (int i = 0; i < refs; i++) {
//        //len += offset[i];
//        len += BS[i].size();
//    }
//    //cout << "Encoded size (bits) = " << len << endl;
//    cout << "Encoded size (bytes) = " << int(len/8) << endl;
//    cout << "########" <<endl;
    
    long long int len2 = 0;
    for (vector<int> v : SIMP) {
        len2 += v.size() * 4; // 4 = 32 / 8
    }
    
    printf("A3 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, len2);
    //cout << "Encoded size (bytes) = " << len2 << endl;
    //cout << "########" <<endl;

    #if DEBUG
    /*
    for (int i = 0; i < refs; i++) {
        cout << bsToString(i, offset[i]);
    }
    cout << endl;
    */
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

            fullPermutationSearchingSIMPLE9(refs, dim, n, q, R, PQ, knn);
            
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A3 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    //delete[] offset;
    //delete[] bs;
    //delete[] BS;
    
    CBUFFER.clear();
    delete[] CBPTR;
    delete[] LASTPTR;
    delete[] ACCUM;
    SIMP.clear();

    delete[] INT_ID;
    delete[] INT_NEXT_POS;
    
    f_in.close();

    return 0;
}

// MSA com SIMPLE-9 usando contagem previa
int cMSA_Simple9(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    int dim, refs, n, num_q;
    int datasetrefs;
    int ignore;
    
    time_t t_start=0;
    clock_t c_start=0;
    
    std::fstream f_in_tmp(s_in, std::ios_base::in);
    if(!f_in_tmp.is_open()) {
        printf("PROBLEMA NA LEITURA DA ENTRADA");
        cout << " em " << s_in << endl;
        exit(EXIT_FAILURE);
    }

    f_in_tmp >> dim; //dimensoes
    f_in_tmp >> refs; //referencias (pivos)
    f_in_tmp >> n; //objetos
    f_in_tmp >> num_q; //consultas
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in_tmp >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) {
        f_in_tmp >> R[i]; //referencias
    }
    
    
    
    /*************************************************************/
    /* inicializa CBUFFER e outros bagulhos para o SIMPLE-9      */
    
    int * COUNTERlocal = new int[ refs ]; // contador de inteiros para cada bucket
    
    for (int j = 0; j < refs; j++) {
        COUNTERlocal[j] = 0;
    }
     
    int * * SIMPlocal = new int * [ refs ]; // estrutura principal SIMPLE-9
    
    /*************************************************************/

    /* ************************************************************************* */
    /* ************************ SECAO DE CONTAGEM  ***************************** */
    /* ************************************************************************* */
    
    if(time) time_start(&t_start, &c_start);
    
    fullPermutationIndexingSIMPLE9counter(refs, dim, n, R, f_in_tmp, COUNTERlocal);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_in_tmp >> ignore; // registros não utilizados
    
    // definição da estrutura com as dimensões contadas previamente 
    for (int i = 0; i < refs; i++) {
        SIMPlocal[i] = new int[ COUNTERlocal[i] ];
        //printf("COUNTERlocal[%d] = %d\n", i, COUNTERlocal[i]);
        for (int j = 0; j < COUNTERlocal[i]; j++) {
            SIMPlocal[i][j] = 0;
        }
    }
    

    f_in_tmp.close();
    /* *********************** CONTAGEM FINALIZADA **************************** */

    
    
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    for (int i = 0; i < dim * refs; i++) f_in >> ignore; //referencias já foram lidas

    /* ************************************************************************* */
    /* ************************ SECAO DE INDEXACAO ***************************** */
    /* ************************************************************************* */
    
    fullPermutationIndexingSIMPLE9(refs, dim, n, R, f_in, SIMPlocal);
    
    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
//    for (int i = 0; i < refs; i++) {
//        for (int j = 0; j < COUNTERlocal[i]; j++) {
//            printf("SIMPlocal[%d][%d] = %d\n", i, j, SIMPlocal[i][j]);
//        }
//    }
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      printf("A7 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    long long int len2 = 0;
    for (int i = 0; i < refs; i++) {
        len2 += COUNTERlocal[i] * 4; // cada int acrescenta 4 bytes
    }
    
    printf("A7 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, len2);
    
    
    /* ************************************************************************* */
    /* ************************ SECAO DE CONSULTAS ***************************** */
    /* ************************************************************************* */
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

            fullPermutationSearchingSIMPLE9(refs, dim, n, q, R, PQ, knn, SIMPlocal);
            
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A7 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }
    
    //printf(" ---------- PASSOU AQUI 4\n");

    delete[] R;
    
//    for (int i = 0; i < refs; i++) {
//        delete [] SIMPlocal[i];
//    }
    delete [] SIMPlocal;
    
    f_in.close();
    
    //printf(" ---------- PASSOU AQUI 5\n");

    return 0;
}

// MSA com ELIAS DELTA
int cMSA(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q) {

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    
    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
//    if (r != 0) {
//        if (r > refs) n = n - (r - refs);
//        refs = r;
//    }
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    //int *offset = new int[refs];

    //bitset<BSSIZE> *bs = new bitset<BSSIZE>[refs]; // MSIZE = n ?
    vector<bool> *BS = new vector<bool>[refs];

    /*
    int *o = new int[dim * n]; //objetos
    for (int i = 0; i < dim * n; i++) f_in >> o[i];
    */

    if(time) time_start(&t_start, &c_start);

    fullPermutationIndexing(refs, dim, n, R, f_in, BS);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    /**/
    //delete[] o;
    //delete[] lastValues;
    /**/

    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      printf("A2 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      //cout << "Indexing:" << endl;
      time_stop(t_start, c_start);
    }

    long long int len = 0;
    for (int i = 0; i < refs; i++) {
        //len += offset[i];
        len += BS[i].size();
    }
    //cout << "Encoded size (bits) = " << len << endl;
    //cout << "Encoded size (bytes) = " << len << endl;
    //cout << "########" <<endl;
    printf("A2 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, len / 8);

    #if DEBUG
    /*
    for (int i = 0; i < refs; i++) {
        cout << bsToString(i, offset[i]);
    }
    cout << endl;
    */
    #endif

    if(knn>0){//knn queries


        std::fstream f_out;
        if(output){
          f_out.open(s_out, std::ios_base::out);
          if(!f_out.is_open()) exit(EXIT_FAILURE);
          if(verbose) cout << s_out <<endl;
        }

        if(time) time_start(&t_start, &c_start);

        #ifdef DEBUG_EXACT_SEARCH
        cout << endl << "First 10 queries:" << endl;
        #endif
        for (int i = 0; i < num_q; i++) {

            int q[dim];
            for (int j = 0; j < dim; j++) {
                f_in >> q[j];
                #ifdef DEBUG_EXACT_SEARCH
                if (i < 10) cout << q[j] << " ";
                #endif
            }
            #ifdef DEBUG_EXACT_SEARCH
            if (i < 10) cout << endl;
            #endif
            
            //struct Node *acc = new struct Node[n];//?ok
            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);

            fullPermutationSearching(refs, dim, n, q, R, acc, BS, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A2 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    //delete[] offset;
    //delete[] bs;
    delete[] BS;
    f_in.close();

    return 0;
}

#if BOOST
// MSA com ELIAS DELTA & BOOST
int cMSAdeltaBoost(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q) {

//    boost::dynamic_bitset<> x(5);
//    std::cout << "x.size() = " << x.size() << std::endl;
//
//    x[1] = 1;
//    x.push_back(true);
//    x.push_back(0);
//    x.push_back(1);
//
//    std::cout << x << "\n";
//
//    for (boost::dynamic_bitset<>::size_type i = 0; i < x.size(); ++i)
//        std::cout << x[i];
//    std::cout << "\n";
//    
//    cout << "AINDA EM CONSTRUCAO" << endl;
//    
//    return 0;
    
    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);


    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    /* ************************************************************************* */
    /* ************************ SECAO DE INDEXACAO ***************************** */
    /* ************************************************************************* */
    //vector<bool> *BS = new vector<bool>[refs];
    
    boost::dynamic_bitset<> * DBS = new boost::dynamic_bitset<>[refs];

    if(time) time_start(&t_start, &c_start);

    fullPermutationIndexingBoost(refs, dim, n, R, f_in, DBS);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      printf("A4 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    long long int len = 0;
    for (int i = 0; i < refs; i++) {
        len += DBS[i].size();
    }

    printf("A4 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, len / 8);


    /* ************************************************************************* */
    /* ************************ SECAO DE CONSULTAS ***************************** */
    /* ************************************************************************* */
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

            fullPermutationSearchingBoost(refs, dim, n, q, R, acc, DBS, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A4 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    delete[] DBS;
    f_in.close();

    return 0;
}
#endif

// MSA com ELIAS DELTA (char array)
int cMSAdelta_v2(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q) {
    
    int dim, refs, n, num_q;
    int datasetrefs;
    int ignore;
    
    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_int_tmp(s_in, std::ios_base::in);
    if(!f_int_tmp.is_open()) exit(EXIT_FAILURE);

    f_int_tmp >> dim; //dimensoes
    f_int_tmp >> refs; //referencias (pivos)
    f_int_tmp >> n; //objetos
    f_int_tmp >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_int_tmp >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_int_tmp >> R[i]; //referencias

    /* ************************************************************************* */
    /* ************************ SECAO DE CONTAGEM  ***************************** */
    /* ************************************************************************* */

    if(time) time_start(&t_start, &c_start);
    
    int * offset = new int[refs]; // estrutura para armazenar tamanho dos buckets
    
    for (int i = 0; i < refs; i++) {
        offset[i] = 0;
    }

    fullPermutationIndexingCounter(refs, dim, n, R, f_int_tmp, offset);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_int_tmp >> ignore; // registros não utilizados
    
//    long long int summ = 0;
//    for (int i = 0; i < refs; i++) {
//        printf("offset[%d] = %d      numero de bytes = %d\n", i, offset[i], offset[i] / 8 + 1);
//        summ += offset[i] / 8 + 1;
//    }
//    printf("TOTAL = %lld bytes\n", summ);
    
    /* *********************** CONTAGEM FINALIZADA **************************** */
    
    f_int_tmp.close();

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    for (int i = 0; i < dim * refs; i++) f_in >> ignore; //referencias já foram lidas

    /* ************************************************************************* */
    /* ************************ SECAO DE INDEXACAO ***************************** */
    /* ************************************************************************* */
    //vector<bool> *BS = new vector<bool>[refs];
    
    unsigned char * * BARR = new unsigned char * [refs];
    
    for (int i = 0; i < refs; i++) {
        int bucket_size_bytes = offset[i] / 8 + 1;  // número de bytes alocados para um bucket
        BARR[i] = new unsigned char[bucket_size_bytes];
        for (int k = 0; k < bucket_size_bytes; k++) {
            BARR[i][k] = 0;
        }
    }
    
    fullPermutationIndexing(refs, dim, n, R, f_in, BARR);
    
    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados

    //    for (int i = 0; i < refs; i++) {
//        int bucket_size_bytes = offset[i] / 8 + 1;
//        for (int k = 0; k < bucket_size_bytes; k++) {
//            printf("%d ", BARR[i][k]);
//        }
//        printf("\n");
//    }
    
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      printf("A5 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    long long int len = 0;
    for (int i = 0; i < refs; i++) {

        len += offset[i] / 8 + 1;
    }

    printf("A5 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, len);
    
    /* ************************************************************************* */
    /* ************************ SECAO DE CONSULTAS ***************************** */
    /* ************************************************************************* */
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

            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);

            fullPermutationSearching(refs, dim, n, q, R, acc, BARR, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){

          printf("A5 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    for (int i = 0; i < refs; i++) {
        delete [] BARR[i];
    }
    delete[] BARR;
    f_in.close();

    return 0;
}

// algoritmo A9 - MSA com ELIAS DELTA (char array) v3
int cMSAdelta_v3(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q) {
    
    int dim, refs, n, num_q;
    int datasetrefs;
    int ignore;
    
    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_int_tmp(s_in, std::ios_base::in);
    if(!f_int_tmp.is_open()) exit(EXIT_FAILURE);

    f_int_tmp >> dim; //dimensoes
    f_int_tmp >> refs; //referencias (pivos)
    f_int_tmp >> n; //objetos
    f_int_tmp >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_int_tmp >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_int_tmp >> R[i]; //referencias

    /* ************************************************************************* */
    /* ************************ SECAO DE CONTAGEM  ***************************** */
    /* ************************************************************************* */

    if(time) time_start(&t_start, &c_start);
    
    int * offset = new int[refs]; // estrutura para armazenar tamanho dos buckets
    
    for (int i = 0; i < refs; i++) {
        offset[i] = 0;
    }

    // conta previamente o tamanho do array que será usado
    fullPermutationIndexingCounter(refs, dim, n, R, f_int_tmp, offset);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_int_tmp >> ignore; // registros não utilizados
    
//    long long int summ = 0;
//    for (int i = 0; i < refs; i++) {
//        printf("offset[%d] = %d      numero de bytes = %d\n", i, offset[i], offset[i] / 8 + 1);
//        summ += offset[i] / 8 + 1;
//    }
//    printf("TOTAL = %lld bytes\n", summ);
    
    /* *********************** CONTAGEM FINALIZADA **************************** */
    
    f_int_tmp.close();

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    for (int i = 0; i < dim * refs; i++) f_in >> ignore; //referencias já foram lidas

    /* ************************************************************************* */
    /* ************************ SECAO DE INDEXACAO ***************************** */
    /* ************************************************************************* */
    //vector<bool> *BS = new vector<bool>[refs];
    
    
//    int max_bucket = 0;
//    for (int i = 0; i < refs; i++) {
//        int bucket_size_bytes = offset[i] / 8 + 1;  // número de bytes alocados para um bucket
//        if (bucket_size_bytes > max_bucket) {
//            max_bucket = bucket_size_bytes;
//        }
//    }
//    long long int arrSize = refs;
//    arrSize *= max_bucket;
    
    
    long long int * shiftArr = new long long int[refs + 1];
    shiftArr[0] = 0;
    for (int i = 0; i < refs; i++) {
        shiftArr[i + 1] = shiftArr[i];
        shiftArr[i + 1] += (offset[i] >> 3) + 1;
    }
//    
//    printf("arrSize = %lld\n", arrSize);
//    printf("shiftArr[refs] = %lld\n", shiftArr[refs]);
    
    unsigned char * BARR = new unsigned char[ shiftArr[refs] ];
    
    for (long long int i = 0; i < *(shiftArr + refs); i++) {
        *(BARR + i) = 0;
    }
    
    fullPermutationIndexingA9(refs, dim, n, R, f_in, BARR, shiftArr);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados

    
//    for (int i = 0; i < refs; i++) {
//        int bucket_size_bytes = offset[i] / 8 + 1;
//        for (int k = 0; k < bucket_size_bytes; k++) {
//            printf("%d ", BARR[i][k]);
//        }
//        printf("\n");
//    }
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      printf("A9 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

//    long long int len = 0;
//    for (int i = 0; i < refs; i++) {
//
//        len += offset[i] / 8 + 1;
//    }

    printf("A9 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, shiftArr[refs]);
    
    /* ************************************************************************* */
    /* ************************ SECAO DE CONSULTAS ***************************** */
    /* ************************************************************************* */
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

            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);

            fullPermutationSearchingA9(refs, dim, n, q, R, acc, BARR, PQ, knn, shiftArr);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){

          printf("A9 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    delete[] BARR;
    delete[] shiftArr;
    f_in.close();

    return 0;
}

// MSA usando diferenca em unsigned char fixed (A6)
int cMSA_diff(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q) {
    
    int dim, refs, n, num_q;
    int datasetrefs;
    int ignore;
    
    time_t t_start=0;
    clock_t c_start=0;

    

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; // referencias

    /* ************************************************************************* */
    /* ************************ SECAO DE INDEXACAO ***************************** */
    /* ************************************************************************* */
    //vector<bool> *BS = new vector<bool>[refs];
    
    // definindo tamanho do bloco de bits
    int block_size = binaryLength(2 * refs - 1); // tamanho fixo dos blocos de bits
    int bucket_size = (n * block_size) / 8 + 1; // número de elementos de 1 Byte (unsigned char)
    
//    printf("block size = %d\n", block_size);
//    printf("bucket size = %d\n", bucket_size);
    
    unsigned char * * BARR = new unsigned char * [refs];
    
    for (int i = 0; i < refs; i++) {
        
        BARR[i] = new unsigned char[bucket_size];
        
        for (int k = 0; k < bucket_size; k++) {
            BARR[i][k] = 0;
        }
    }
    
    fullPermutationIndexingA6(refs, dim, n, R, f_in, BARR, block_size);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
        
//    for (int i = 0; i < refs; i++) {
//        for (int k = 0; k < bucket_size; k++) {
//            printf("%d ", BARR[i][k]);
//        }
//        printf("\n");
//    }
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      printf("A6 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }


    printf("A6 %d %d %d %d Encoded size (bytes) = %d\n", dim, refs, n, num_q, refs * bucket_size);
    
    /* ************************************************************************* */
    /* ************************ SECAO DE CONSULTAS ***************************** */
    /* ************************************************************************* */
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

            struct Node *acc = NULL;
            priority_queue_Node PQ;
            init_pq_fixed_size(PQ, knn);

            fullPermutationSearchingA6(refs, dim, n, q, R, acc, BARR, PQ, knn, block_size);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){

          printf("A6 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    for (int i = 0; i < refs; i++) {
        delete [] BARR[i];
    }
    delete[] BARR;
    f_in.close();

    return 0;
}

#if SDSL
// algoritmo A10 - cMSA SDSL
int cMSA_sdsl(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/
 
    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    //vector<int> *M = new vector<int>[refs];
    //int *M = new int[n * refs];
    
    //REMOVE
    //unsigned int * M = new unsigned int[refs * n];
    //unsigned int * S = new unsigned int[refs * n];
    vector<uint32_t> L(refs);
    
    ofstream outfile("tmp.sdsl",ios::out | ios::binary);
       
    if(time) time_start(&t_start, &c_start);
    
    
/**********************************************************************
 * //fullPermutationIndexing(refs, dim, n, R, f_in, M);
 **********************************************************************/

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        //printf("    obj %d = ", i);
        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
            //printf("%d ", oi[kk]);
        }
        //printf("\n");

        //createOrderedList(refs, dim, oi, loi, refsR);
        createPositionList(refs, dim, oi, lpi, R);

        for (int j = 0; j < refs; j++) {
            //M[j * n + i] = i * refs + p(loi, j, refs);
            unsigned int value = refs;
            value *= i;
            value += lpi[j];
            
            //FELIPE
            //S[i*refs + lpi[j]] = j;
            //TODO: escrever no disco   - OK
            //TODO: alocar S[refs]      - OK
            
            //S[lpi[j]] = j; --> disk   - OK
            L[lpi[j]] = j;
            
            //M[j * n + i] = value;
        }
        
        for (int i = 0; i < refs; i++) 
          outfile.write((char*) &L[i], sizeof(uint32_t));
    }
    
    outfile.close();
    
    delete[] oi;
    delete[] lpi;

    
    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    
/**********************************************************************/

    wt_sdsl WT;
    construct(WT, "tmp.sdsl", 4);

    //VALIDATE string S e WT
/*
    for (int i = 0; i < n*refs; i++) 
      if(S[i]!=WT[i])
        cout<<S[i]<<" "<<WT[i]<<" ERROR ***************\n";

    unsigned int * Bucket = new unsigned int[refs];
    for (int i = 0; i < refs; i++) {
      Bucket[i] = i*n;
    }
    
    for (int i = 0; i < n*refs; i++) 
      M2[Bucket[S[i]]++] = i;
            
    for (int i = 0; i < n*refs; i++) 
      if(M[i]!=M2[i])
        cout<<M[i]<<" "<<M2[i]<<" ERROR ***************\n";
*/

    //VALIDATE WT e MSA
/*
    for (int i = 0; i < refs; i++) 
      for (int j = 0; j < n; j++) 
        if(M[i*n+j]!=WT.select(j+1, i))
          cout<<M[i*n+j]<<" "<<WT.select(j+1, i)<<" ERROR ***************\n";
*/
/**********************************************************************/
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      //cout << "Indexing:" << endl;
      printf("A10 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    printf("A10 %d %d %d %d Encoded size (bytes) = %ld\n", dim, refs, n, num_q, size_in_bytes(WT));
    
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

            //printf(" ---- PASSOU AQUI 1\n");
            
            fullPermutationSearching_sdsl(refs, dim, n, q, R, acc, NULL, WT, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A10 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    //REMOVE
    //delete[] M;
    f_in.close();

    return 0;
}
#endif

// algoritmo A1 - MSA original
int MSA(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
//    if (r != 0) {
//        if (r > refs) n = n - (r - refs);
//        refs = r;
//    }
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    
//    /* =======================================================================  */
//    /* ======================== LIMITE DE MEMORIA ============================  */
//    long long int totalSize = ( n * refs * 4 ) >> 30; // tamanho do MSA em GB
//    if (totalSize > 10) {
//        printf("A1 num refs = %d num objs = %d = %lld GB excede memória\n", refs, n, totalSize);
//        exit(0);
//    }
//    /* =======================================================================  */
    
    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    //vector<int> *M = new vector<int>[refs];
    //int *M = new int[n * refs];
    
    long long int sizeM = refs;
    sizeM *= n;
    
    unsigned int * M = new unsigned int[sizeM];
//    for (long long int i = 0; i < sizeM; i++) {
//        M[i] = 1;
//    }
//    
//    long long int counter = 0;
//    for (long long int i = 0; i < sizeM; i++) {
//        counter += M[i];
//    }
    
    if(time) time_start(&t_start, &c_start);
    
    fullPermutationIndexing(refs, dim, n, R, f_in, M);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      //cout << "Indexing:" << endl;
      printf("A1 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    long long int ttsize = n;
    ttsize *= refs;
    ttsize *= 4;
    printf("A1 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, ttsize);
    
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

            //printf(" ---- PASSOU AQUI 1\n");
            
            fullPermutationSearching(refs, dim, n, q, R, acc, M, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A1 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    delete[] M;
    f_in.close();

    return 0;
}

// algoritmo A8 - MSA melhorado
int MSA_m(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
//    if (r != 0) {
//        if (r > refs) n = n - (r - refs);
//        refs = r;
//    }
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    
//    /* =======================================================================  */
//    /* ======================== LIMITE DE MEMORIA ============================  */
//    long long int totalSize = ( n * refs * 4 ) >> 30; // tamanho do MSA em GB
//    if (totalSize > 10) {
//        printf("A1 num refs = %d num objs = %d = %lld GB excede memória\n", refs, n, totalSize);
//        exit(0);
//    }
//    /* =======================================================================  */
    
    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    //vector<int> *M = new vector<int>[refs];
    //int *M = new int[n * refs];
    
    long long int sizeM = refs;
    sizeM *= n;
    
    int * M = new int[sizeM];
//    for (long long int i = 0; i < sizeM; i++) {
//        M[i] = 0;
//    }
    
    if(time) time_start(&t_start, &c_start);
    
    fullPermutationIndexing_m(refs, dim, n, R, f_in, M);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      //cout << "Indexing:" << endl;
      printf("A8 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    long long int ttsize = n;
    ttsize *= refs;
    ttsize *= 4;
    printf("A8 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, ttsize);

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

            //printf(" ---- PASSOU AQUI 1\n");
            
            fullPermutationSearching_m(refs, dim, n, q, R, acc, M, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A8 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    delete[] M;
    f_in.close();

    return 0;
}

//sequential scan
int naive(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
//    if (r != 0) {
//        if (r > refs) n = n - (r - refs);
//        refs = r;
//    }
    
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    int *R = new int[dim * refs];
    for (int i = 0; i < dim * refs; i++) f_in >> R[i]; //referencias

    //vector<int> *M = new vector<int>[refs];
    //int *O = new int[n * refs];
    int * * O = new int * [n];
    for (int i = 0; i < n; i++) {
        O[i] = new int[refs];
    }

    if(time) time_start(&t_start, &c_start);

    fullSequentialIndexing(refs, dim, n, R, f_in, O);

    // consumir registros nao utilizados na execução: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;
      //cout << "(n_objs x n_refs) x  32 bits = (" << n << " x " << refs << ") x 32 bits = " << n * refs * 32 << endl;

    if(time){
      //cout << "Indexing:" << endl;
      printf("A0 %d %d %d %d Indexing: ", dim, refs, n, num_q);
      time_stop(t_start, c_start);
    }

    //cout << "Encoded size (bits) = " << n * refs * 32 << endl;
    //cout << "Encoded size (bytes) = " << int(n * refs * 32/8) << endl;
    //cout << "########" <<endl;
    long long int totalSize = n * refs * 4;
    printf("A0 %d %d %d %d Encoded size (bytes) = %lld\n", dim, refs, n, num_q, totalSize);

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


            fullSequentialSearching(refs, dim, n, q, R, acc, O, PQ, knn);
            if(output) write_output(acc, n, f_out, knn, PQ);
            delete[] acc;
        }

        if(output) f_out.close();

        if(time){
          //cout << "Searching:" << endl;
          printf("A0 %d %d %d %d Searching: ", dim, refs, n, num_q);
          time_stop(t_start, c_start);
        }
    }

    delete[] R;
    delete[] O;
    f_in.close();

    return 0;
}

/* cMSA simple-9 (A3) related */
void fullPermutationIndexingSIMPLE9(int refs, int dim, int n, int* refsR, std::fstream& f_in) {

    bool * firstValue = new bool[refs];
    for (int i = 0; i < refs; i++) {
        firstValue[i] = true;
    }
    
    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        //int oi[dim];
        for (int kk = 0; kk < dim; kk++) {
            //oi[kk] = domainD[dim * i + kk]; //objeto da lista
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);
        
//        for (int i = 0; i < dim; i++) printf("%d ", oi[i]);
//        printf("\n");
 
        for (int j = 0; j < refs; j++) {
            //int value = i * refs + p(lpi, j, refs) + 1;
            long long int value = i * refs + lpi[j] + 1;
            
            //printf("value[%d] = %d\n", j, value);

            //if (offset[j] == 0) {//proprio valor
            if (firstValue[j]) {//proprio valor
                firstValue[j] = false;
                lastValues[j] = value;

                CBUFFER.at(j)[CBPTR[j]] = (int) value;
                CBPTR[j]++;
                CBPTR[j] %= CN;
                ACCUM[j]++;
                
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                lastValues[j] = value;

                CBUFFER.at(j)[CBPTR[j]] = diff;
                CBPTR[j]++;
                CBPTR[j] %= CN;
                ACCUM[j]++;

            }
            
            Parameter param = getMP(CBUFFER.at(j), LASTPTR[j]);
            
            if (ACCUM[j] >= param.p) {
                
//                cout << "BUCKET " << j << "\n";
//                int count = 0;
//                for (int kk = 0; kk < CN; kk++) {
//                    if (count == LASTPTR[j]) {
//                        cout << "[ ";
//                    }
//                    cout << CBUFFER.at(j)[kk] << " ";
//                    count++;
//                }
//                cout << "\n";
                
                int code = getCode(CBUFFER.at(j), LASTPTR[j], param.m, param.p);
                SIMP.at(j).push_back(code);
                LASTPTR[j] += param.p;
                LASTPTR[j] %= CN;
                ACCUM[j] = 0;
                
                //cout << code << "\n";
                

            }
        }

    }
    
    for (int j = 0; j < refs; j++) {

//        cout << "BUCKET " << j << "\n";
//        int count = 0;
//        for (int kk = 0; kk < CN; kk++) {
//            if (count == LASTPTR[j]) {
//                cout << "[ ";
//            }
//            cout << CBUFFER.at(j)[kk] << " ";
//            count++;
//        }
//        cout << "\n";
//        cout << ACCUM[j] << endl;
                
        Parameter param = getMP(CBUFFER.at(j), LASTPTR[j]);

        int code = getCode(CBUFFER.at(j), LASTPTR[j], param.m, param.p);
        SIMP.at(j).push_back(code);
        LASTPTR[j] += param.p;
        LASTPTR[j] %= CN;
        ACCUM[j] = 0;
        
        //cout << code << "\n";

    }
    
//    for (int j = 0; j < refs; j++) {
//        cout << "BUCKET " << j << ": SIZE = " << SIMP.at(j).size() << "\n";
//    }
    
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;
    delete[] firstValue;

}

/* cMSA simple-9 com contagem previa (A7) related */
void fullPermutationIndexingSIMPLE9(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * * SIMPlocal) {

    // array de arrays circulares
    int * * CBUFFERlocal = new int * [ refs ];
    
    for (int i = 0; i < refs; i++) {
        CBUFFERlocal[i] = new int[ CN ];
    }
    
    int * CBPTRlocal = new int[ refs ]; // ponteiro para o buffer circular
    int * LASTPTRlocal = new int[ refs ]; 
    int * ACCUMlocal = new int[ refs ];
    
    for (int j = 0; j < refs; j++) {
        CBPTRlocal[j] = LASTPTRlocal[j] = ACCUMlocal[j] = 0;
    }
    
    // array que aponta para a próxima posição disponível no SIMPlocal
    int * indexes = new int[ refs ];
    for (int i = 0; i < refs; i++) {
        indexes[i] = 0;
    }
    
    bool * firstValue = new bool[refs];
    for (int i = 0; i < refs; i++) {
        firstValue[i] = true;
    }
    
    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {

            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);
 
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;


            if (firstValue[j]) {//proprio valor
                
                firstValue[j] = false;
                lastValues[j] = value;

                CBUFFERlocal[j][ CBPTRlocal[j] ] = (int) value;
                CBPTRlocal[j]++;
                CBPTRlocal[j] %= CN;
                ACCUMlocal[j]++;
                
            } else { //um lastValues e offset para cada referencia
                
                int diff = (int) (value - lastValues[j]);
                lastValues[j] = value;

                CBUFFERlocal[j][ CBPTRlocal[j] ] = diff;
                CBPTRlocal[j]++;
                CBPTRlocal[j] %= CN;
                ACCUMlocal[j]++;

            }
            
            Parameter param = getMP(CBUFFERlocal[j], LASTPTRlocal[j]);
            
            if (ACCUMlocal[j] >= param.p) {
                
                int code = getCode(CBUFFERlocal[j], LASTPTRlocal[j], param.m, param.p);
                //SIMP.at(j).push_back(code);
                SIMPlocal[j][ indexes[j] ] = code;
                indexes[j]++;
                LASTPTRlocal[j] += param.p;
                LASTPTRlocal[j] %= CN;
                ACCUMlocal[j] = 0;
                
            }
        }

    }
    
    for (int j = 0; j < refs; j++) {
        
        Parameter param = getMP(CBUFFERlocal[j], LASTPTRlocal[j]);

        int code = getCode(CBUFFERlocal[j], LASTPTRlocal[j], param.m, param.p);
        //SIMP.at(j).push_back(code);
        SIMPlocal[j][ indexes[j] ] = code;
        indexes[j]++;
        LASTPTRlocal[j] += param.p;
        LASTPTRlocal[j] %= CN;
        ACCUMlocal[j] = 0;
        
    }
    
    //printf(" ---------- PASSOU AQUI 1\n");

    //delete[] oi;
    //delete[] lpi;
    //delete[] lastValues;
    //delete[] firstValue;

    //printf(" ---------- PASSOU AQUI 2\n");

//    for (int i = 0; i < refs; i++) {
//        delete [] CBUFFERlocal[i];
//    }
    delete [] CBUFFERlocal;

    delete[] CBPTRlocal;
    delete[] LASTPTRlocal;
    //delete[] ACCUMlocal;

    //printf(" ---------- PASSOU AQUI 3\n");
}


/* cMSA simple-9 com contagem previa (A7) related */
void fullPermutationIndexingSIMPLE9counter(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * COUNTERlocal) {

    int * * CBUFFERlocal = new int * [ refs ]; // array de arrays circulares
    
    for (int i = 0; i < refs; i++) {
        CBUFFERlocal[i] = new int[ CN ];
        for (int j = 0; j < CN; j++) {
            CBUFFERlocal[i][j] = 0;
        }
    }
    
    int * CBPTRlocal = new int[ refs ]; // ponteiro para o buffer circular
    int * LASTPTRlocal = new int[ refs ]; 
    int * ACCUMlocal = new int[ refs ];
    
    for (int j = 0; j < refs; j++) {
        CBPTRlocal[j] = LASTPTRlocal[j] = ACCUMlocal[j] = 0;
    }
    
    bool * firstValue = new bool[refs];
    for (int i = 0; i < refs; i++) {
        firstValue[i] = true;
    }
    
    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);
        
//        for (int i = 0; i < dim; i++) printf("%d ", oi[i]);
//        printf("\n");
 
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;
            
            //printf("value[%d] = %d\n", j, value);

            if (firstValue[j]) {//proprio valor
                
                firstValue[j] = false;
                lastValues[j] = value;

                CBUFFERlocal[j][ CBPTRlocal[j] ] = (int) value;
                CBPTRlocal[j]++; // incrementa ponteiro do buffer circular
                CBPTRlocal[j] %= CN;
                ACCUMlocal[j]++; // incrementa posicao absoluta
                
            } else { //um lastValues e offset para cada referencia
                
                int diff = (int) (value - lastValues[j]);
                lastValues[j] = value;

                CBUFFERlocal[j][ CBPTRlocal[j] ] = diff;
                CBPTRlocal[j]++; // incrementa ponteiro do buffer circular
                CBPTRlocal[j] %= CN;
                ACCUMlocal[j]++; // incrementa posicao absoluta

            }

            Parameter param = getMP(CBUFFERlocal[j], LASTPTRlocal[j]);
            
            //printf("ACCUMlocal[%d] = %d | param.p = %d\n", j, ACCUMlocal[j], param.p);
            if (ACCUMlocal[j] >= param.p) {

                //int code = getCode(CBUFFERlocal[j], LASTPTRlocal[j], param.m, param.p);
                //SIMP.at(j).push_back(code);
                COUNTERlocal[j]++;
                LASTPTRlocal[j] += param.p;
                LASTPTRlocal[j] %= CN;
                ACCUMlocal[j] = 0;
            }
        }

    }
    
    for (int j = 0; j < refs; j++) {
                
        Parameter param = getMP(CBUFFERlocal[j], LASTPTRlocal[j]);

        //int code = getCode(CBUFFERlocal[j], LASTPTRlocal[j], param.m, param.p);
        //SIMP.at(j).push_back(code);
        COUNTERlocal[j]++;
        LASTPTRlocal[j] += param.p;
        LASTPTRlocal[j] %= CN;
        ACCUMlocal[j] = 0;

    }

    delete[] oi;
    delete[] lpi;
    delete[] lastValues;
    delete[] firstValue;

    for (int i = 0; i < refs; i++) {
        delete [] CBUFFERlocal[i];
    }
    delete [] CBUFFERlocal;
    
    delete [] CBPTRlocal;
    delete [] LASTPTRlocal;
    delete [] ACCUMlocal;
}

int getCode(int * x, int k, int m, int p) {

    int c = 0;

    while (p * (m + 1) <= 28) {
        m++;
    }

    while (p > 0) {
        c = c * (1 << m) + x[k % CN];
        k++;
        p--;
    }

    c += (1 << 28) * m;

    return c;
}

/* cMSA simple-9 (A3) related */
Parameter getMP(int * x, int k) {
    
    int m, p;

    m = binaryLength(x[k]);
    p = 1;

    while ((p + 1) * max(m, binaryLength(x[(k + p) % CN])) <= 28) {
        m = max(m, binaryLength(x[(k + p) % CN]));
        p++;
    }

    struct Parameter tmp = {m, p};
    return tmp;
}

int binaryLength(int xk) {
    return floor(log2(xk)) + 1;
}

int deltaLength(int n) {
    int blen = binaryLength(n);
    return blen + 2 * floor( log2(blen) );
}

/* cMSA simple-9 (A3) related */
void fullPermutationSearchingSIMPLE9(int refs, int dim, int n, int * q, int * refsR, priority_queue_Node &PQ, int knn) {

//    cout << "**************" << endl;
//    for (int i = 0; i < refs; i++) {
//        cout << "BUCKET " << i << " SIZE = " << SIMP.at(i).size() << endl;;
//    }
    
    int *lq = new int[refs];
    createOrderedList(refs, dim, q, lq, refsR);
    
    /**********************************/
    /* ESTRUTURAS SIMPLE-9 */
    INT_ID = new int[refs];
    INT_NEXT_POS = new int[refs];
    int * MSA_k2 = new int[refs];
    
    for (int j = 0; j < refs; j++) {
        INT_ID[j] = INT_NEXT_POS[j] = MSA_k2[j] = 0;
    }
    /**********************************/


    for (int oid = 0; oid < n; oid++) {
        
        double Acc2 = 0.0;
        
        for( int j = 0; j < refs; j++) {

          int refPosQ = j;
          int bucket = lq[j];
          
          
          int tmp2 = retrieve(bucket, j);
          MSA_k2[j] += tmp2;
          
          int refPos2 = MSA_k2[j] % refs;
          
          Acc2 += abs(refPosQ - refPos2);
        }

        //printf("oid = %d | Acc2 = %f\n", oid, Acc2);
        struct Node tmp = {oid, Acc2};
        
        push_pq_fixed_size(PQ, tmp, knn);
        //acc[oid].d = Acc;
    }

    
    delete[] lq;
}

/* cMSA simple-9 (A3) related */
// decodifica um valor do MSA
// a posição deste valor é armazenado em INT_NEXT_POS
int retrieve(int bucket, int j) {

    int current_32bits = SIMP.at(bucket).at(INT_ID[j]);

    int m = ( (unsigned int) current_32bits ) >> 28;
    int p = 28 / m;
    int mask = (1 << m) - 1;

    int output = 0;
    int adjust = 0;
    
    // corrije o valor para que o primeiro objeto seja zero
    // talvez seja melhor transferir essa correção para o 'print'
    if (INT_ID[j] == 0 && INT_NEXT_POS[j] == 0) {
        adjust = -1;
    }

    // ignora valores zero contidos no ínicio do INT
    // o que ocorre em alguns casos
    while (output == 0) {
        output = ( (unsigned int) current_32bits ) >> ((p - 1 - INT_NEXT_POS[j]) * m);
        output &= mask;
        INT_NEXT_POS[j]++;
    }

    if (INT_NEXT_POS[j] >= p) {
        INT_ID[j]++;
        INT_NEXT_POS[j] = 0;
    }

    return output + adjust;
}

/* cMSA simple-9 contagem previa (A7) related */
void fullPermutationSearchingSIMPLE9(int refs, int dim, int n, int * q, int * refsR, priority_queue_Node &PQ, int knn, int * * SIMPlocal) {
    
    int *lq = new int[refs];
    createOrderedList(refs, dim, q, lq, refsR);
    
    /**********************************/
    /* ESTRUTURAS SIMPLE-9 */
    int * INT_ID_local = new int[refs];
    int * INT_NEXT_POS_local = new int[refs];
    int * MSA_k2 = new int[refs];
    
    for (int j = 0; j < refs; j++) {
        INT_ID_local[j] = INT_NEXT_POS_local[j] = MSA_k2[j] = 0;
    }
    /**********************************/

    for (int oid = 0; oid < n; oid++) {
        
        double Acc2 = 0.0;
        
        for( int j = 0; j < refs; j++) {

          int refPosQ = j;
          int bucket = lq[j];
          
          
          
          int tmp2 = retrieve(bucket, j, SIMPlocal, INT_ID_local, INT_NEXT_POS_local);
          MSA_k2[j] += tmp2;
          
          int refPos2 = MSA_k2[j] % refs;
          
          Acc2 += abs(refPosQ - refPos2);
        }

        //printf("oid = %d | Acc2 = %f\n", oid, Acc2);
        struct Node tmp = {oid, Acc2};
        
        push_pq_fixed_size(PQ, tmp, knn);

    }

    
    delete[] lq;
    
    delete [] INT_ID_local;
    delete [] INT_NEXT_POS_local;
    delete [] MSA_k2;
}

/* cMSA simple-9 contagem previa (A7) related */
// decodifica um valor do MSA
// a posição deste valor é armazenado em INT_NEXT_POS
int retrieve(int bucket, int j, int * * SIMPlocal, int * INT_ID_local, int * INT_NEXT_POS_local) {

    int current_32bits = SIMPlocal[ bucket ][ INT_ID_local[j] ];

    int m = ( (unsigned int) current_32bits ) >> 28;
    int p = 28 / m;
    int mask = (1 << m) - 1;

    int output = 0;
    int adjust = 0;
    
    // corrije o valor para que o primeiro objeto seja zero
    // talvez seja melhor transferir essa correção para o 'print'
    if (INT_ID_local[j] == 0 && INT_NEXT_POS_local[j] == 0) {
        adjust = -1;
    }

    // ignora valores zero contidos no ínicio do INT
    // o que ocorre em alguns casos
    while (output == 0) {
        output = ( (unsigned int) current_32bits ) >> ((p - 1 - INT_NEXT_POS_local[j]) * m);
        output &= mask;
        INT_NEXT_POS_local[j]++;
    }

    if (INT_NEXT_POS_local[j] >= p) {
        INT_ID_local[j]++;
        INT_NEXT_POS_local[j] = 0;
    }

    return output + adjust;
}

#if BOOST
/* cMSA delta [boost] (A4) related */
void fullPermutationIndexingBoost(int refs, int dim, int n, int* refsR, std::fstream& f_in, boost::dynamic_bitset<> * DBS) {


    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    // avaliação do desempenho
    int maxDiff = 0;
    int minDiff = 1 << 30;
    long long int summ = 0;
    // ************************
    
    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;


            if (i == 0) {//proprio valor
                lastValues[j] = value;

                deltaEncodeBoost((int) value, j, DBS);
                
            } else { //um lastValues e offset para cada referencia
                
                int diff = (int) (value - lastValues[j]);
                
                if (diff > maxDiff) {
                    maxDiff = diff;
                }
                if (diff < minDiff) {
                    minDiff = diff;
                }
                summ += diff;
                
                lastValues[j] = value;

                deltaEncodeBoost(diff, j, DBS);
            }
        }

    }
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

    double avg = summ / ( (n - 1) * refs );
    
    //printf("Analise das diferencas: Caso A2 dim = %d refs = %d n = %d average diff = %0.1f min diff = %d max diff = %d\n", dim, refs, n, avg, minDiff, maxDiff);
    
}
#endif

/* cMSA delta [vector] (A2) related */
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, vector<bool> *BS) {

    //bs = new BitSet[n];
    //offset = new int[n];
    //lastValues = new int[n];
    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    // avaliação do desempenho
    int maxDiff = 0;
    int minDiff = 1 << 30;
    long long int summ = 0;
    // ************************
    
    #ifdef DEBUG_EXACT_SEARCH
    cout << endl << "First 10 objects:" << endl;
    #endif
    for (int i = 0; i < n; i++) {

        //int oi[dim];
        for (int kk = 0; kk < dim; kk++) {
            //oi[kk] = domainD[dim * i + kk]; //objeto da lista
            f_in >> oi[kk];
            #ifdef DEBUG_EXACT_SEARCH
            if (i < 10) cout << oi[kk] << " ";
            #endif
        }
        #ifdef DEBUG_EXACT_SEARCH
        if (i < 10) cout << endl;
        #endif

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {
            //int value = i * refs + p(lpi, j, refs) + 1;
            long long int value = i * refs + lpi[j] + 1;

            //if (offset[j] == 0) {//proprio valor
            if (i == 0) {//proprio valor
                lastValues[j] = value;
                //offset[j] += deltaEncode(value, j, BS);
                deltaEncode((int) value, j, BS);
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                
                if (diff > maxDiff) {
                    maxDiff = diff;
                }
                if (diff < minDiff) {
                    minDiff = diff;
                }
                summ += diff;
                
                lastValues[j] = value;
                //offset[j] += deltaEncode(diff, j, BS);
                deltaEncode(diff, j, BS);
            }
        }

    }
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

    double avg = summ / ( (n - 1) * refs );
    
    //printf("Analise das diferencas: Caso A2 dim = %d refs = %d n = %d average diff = %0.1f min diff = %d max diff = %d\n", dim, refs, n, avg, minDiff, maxDiff);
    
}

/* cMSA delta [u char] (A5) related */
// funcao apenas para contagem do numero de bits necessarios
// inicializar array de char
void fullPermutationIndexingCounter(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * offset) {

    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;

            if (i == 0) {//proprio valor
                lastValues[j] = value;
                
                //deltaEncode(value, j, BS);
                
                offset[j] = deltaLength((int) value); // adiciona tamanho do valor codificado
                
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                
                lastValues[j] = value;

                //deltaEncode(diff, j, BS);
                
                offset[j] += deltaLength(diff); // adiciona tamanho da diferenca codificada
            }
        }

    }
    
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

}

/* cMSA delta [u char] (A5) related */
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char * * BARR) {

    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    int * offset = new int[refs]; // retorno do registro da posição do ultimo bit inserido
    
    for (int i = 0; i < refs; i++) {
        offset[i] = 0;
    }
    
    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;

            if (i == 0) {//proprio valor
                lastValues[j] = value;
                
                //printf("bkt %d, obj %d = %d\n", j, i, value);
                
                deltaEncode((int) value, j, BARR, offset);
                
                //offset[j] = deltaLength(value);
                
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                
                lastValues[j] = value;

                //printf("bkt %d, obj %d = %d\n", j, i, diff);
                
                deltaEncode(diff, j, BARR, offset);
                
                //offset[j] += deltaLength(diff);
            }
        }

    }
    
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

}

/* cMSA delta [u char] v3 (A9) related */
void fullPermutationIndexingA9(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char *BARR, long long int * shiftArr) {

    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    int * offset = new int[refs]; // retorno do registro da posição do ultimo bit inserido
    
    for (int i = 0; i < refs; i++) {
        offset[i] = 0;
    }
    
    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;

            if (i == 0) {//proprio valor
                lastValues[j] = value;
                
                //printf("bkt %d, obj %d = %d\n", j, i, value);
                
                deltaEncodeA9((int) value, j, BARR, offset, shiftArr);
                
                //offset[j] = deltaLength(value);
                
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                
                lastValues[j] = value;

                //printf("bkt %d, obj %d = %d\n", j, i, diff);
                
                deltaEncodeA9(diff, j, BARR, offset, shiftArr);
                
                //offset[j] += deltaLength(diff);
            }
        }

    }
    
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

}

/* cMSA diff [u char] (A6) related */
void fullPermutationIndexingA6(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned char * * BARR, int block_size) {

    long long int *lastValues = new long long int[refs];

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto
    
    int * offset = new int[refs]; // retorno do registro da posição do ultimo bit inserido
    
    for (int i = 0; i < refs; i++) {
        offset[i] = 0;
    }
    
    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createPositionList(refs, dim, oi, lpi, refsR);

        
        for (int j = 0; j < refs; j++) {

            long long int value = i * refs + lpi[j] + 1;

            if (i == 0) {//proprio valor
                lastValues[j] = value;
                
                deltaEncodeA6((int) value, j, BARR, offset, block_size);
                
                //printf("bkt %d, obj %d = %d\n", j, i, value);
                
                //offset[j] = deltaLength(value);
                
            } else { //um lastValues e offset para cada referencia
                int diff = (int) (value - lastValues[j]);
                
                lastValues[j] = value;

                deltaEncodeA6(diff, j, BARR, offset, block_size);
                
                //printf("bkt %d, obj %d = %d\n", j, i, diff);
                
                //offset[j] += deltaLength(diff);
            }
        }

    }
    
    delete[] oi;
    delete[] lpi;
    delete[] lastValues;

}

/* original MSA (A1) related */
void fullPermutationIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, unsigned int * M) {

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        //printf("    obj %d = ", i);
        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
            //printf("%d ", oi[kk]);
        }
        //printf("\n");

        //createOrderedList(refs, dim, oi, loi, refsR);
        createPositionList(refs, dim, oi, lpi, refsR);

        for (int j = 0; j < refs; j++) {
            //M[j * n + i] = i * refs + p(loi, j, refs);
            unsigned int value = refs;
            value *= i;
            value += lpi[j];
            M[j * n + i] = value;
        }

    }
    delete[] oi;
    delete[] lpi;

}

/* original MSA melhorado (A8) related */
void fullPermutationIndexing_m(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * M) {

    int *lpi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        //printf("    obj %d = ", i);
        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
            //printf("%d ", oi[kk]);
        }
        //printf("\n");

        //createOrderedList(refs, dim, oi, loi, refsR);
        createPositionList(refs, dim, oi, lpi, refsR);

        for (int j = 0; j < refs; j++) {
            //M[j * n + i] = i * refs + p(loi, j, refs);
            //M[i][j] = i * refs + lpi[j];
            long long int index = refs;
            index *= i;
            index += j;
            M[index] = lpi[j];
        }

    }
    delete[] oi;
    delete[] lpi;

}

/* original Naive (A0) related */
void fullSequentialIndexing(int refs, int dim, int n, int* refsR, std::fstream& f_in, int * * O) {

    int *loi = new int[refs];
    int *oi = new int[dim]; //objeto

    for (int i = 0; i < n; i++) {

        for (int kk = 0; kk < dim; kk++) {
            f_in >> oi[kk];
        }

        createOrderedList(refs, dim, oi, loi, refsR);

        for (int j = 0; j < refs; j++) {
            //O[i * refs + j] = loi[j];
            O[i][j] = loi[j];
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

void createOrderedList(int refs, int dim, int* oi, int* loi, int* R) {

    struct Node *output = new struct Node[refs];
    double d, diff;

    for (int i = 0; i < refs; i++) {

        d = 0;
        for (int kk = 0; kk < dim; kk++) {

            diff = oi[kk] - R[dim * i + kk];
            d += diff * diff;
        }

        struct Node tmp = {i, d};
        output[i] = tmp;
    }

    // efetua a ordenaÃ§Ã£o
    std::sort(output, output + refs, &node_sorter);

    for (int i = 0; i < refs; i++) {
        loi[i] = output[i].i;
    }

    delete[] output;
}

void createPositionList(int refs, int dim, int* oi, int* lpi, int* R) {

    struct Node *output = new struct Node[refs];
    double d, diff;

    for (int i = 0; i < refs; i++) {

        d = 0;
        for (int kk = 0; kk < dim; kk++) {
            
            diff = oi[kk] - R[dim * i + kk];
            d += diff * diff;
        }

        struct Node tmp = {i, d};
        output[i] = tmp;
    }

    // efetua a ordenaÃ§Ã£o
    std::sort(output, output + refs, &node_sorter);

    for (int i = 0; i < refs; i++) {
        lpi[output[i].i] = i;
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

#if BOOST
/* cMSA delta [boost] (A4) related */
void fullPermutationSearchingBoost(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, boost::dynamic_bitset<> * DBS, priority_queue_Node &PQ, int knn) {

    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    
    int *MSA_k = new int[refs];
    int *Index = new int[refs];
    for (int j = 0; j < refs; j++) Index[j] = MSA_k[j] = 0;

    for (int oid = 0; oid < n; oid++) {
      double Acc = 0.0;
      for (int j = 0; j < refs; j++) {

        int refPosQ = j; //p(loi, loi[j], refs);//?
        int bucket = loi[j];


        Decoded tmp = deltaDecodeDiffBoost(bucket, Index[j], DBS);

        MSA_k[j] += tmp.value;
        Index[j] = tmp.position;

        int refPos = MSA_k[j] % refs;
        Acc += abs(refPosQ - refPos);
      }

      struct Node tmp = {oid, Acc};
      push_pq_fixed_size(PQ, tmp, knn);
      
    }

    delete[] MSA_k;
    delete[] Index;

    delete[] loi;
}
#endif

/* cMSA delta [u char] (A5) related */
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * * BARR, priority_queue_Node &PQ, int knn) {

    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    
    int *MSA_k = new int[refs];
    int *Index = new int[refs];
    for (int j = 0; j < refs; j++) Index[j] = MSA_k[j] = 0;

    for (int oid = 0; oid < n; oid++) {
      double Acc = 0.0;
      for (int j = 0; j < refs; j++) {

        int refPosQ = j; //p(loi, loi[j], refs);//?
        int bucket = loi[j];


        Decoded tmp = deltaDecodeDiff(bucket, Index[j], BARR);

        //printf("bkt %d, obj %d = %d\n", j, oid, tmp.value);
        
        MSA_k[j] += tmp.value;
        Index[j] = tmp.position;

        int refPos = MSA_k[j] % refs;
        Acc += abs(refPosQ - refPos);
      }

      struct Node tmp = {oid, Acc};
      push_pq_fixed_size(PQ, tmp, knn);
      
    }

    delete[] MSA_k;
    delete[] Index;

    delete[] loi;
}

/* cMSA delta [u char] v3 (A9) related */
void fullPermutationSearchingA9(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * BARR, priority_queue_Node &PQ, int knn, long long int * shiftArr) {

    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    
    int *MSA_k = new int[refs];
    int *Index = new int[refs];
    for (int j = 0; j < refs; j++) *(Index + j) = *(MSA_k + j) = 0;

    for (int oid = 0; oid < n; oid++) {
      double Acc = 0.0;
      for (int j = 0; j < refs; j++) {

        int refPosQ = j; //p(loi, loi[j], refs);//?
        int bucket = *(loi + j);


        Decoded tmp = deltaDecodeDiffA9(bucket, *(Index + j), BARR, shiftArr);

        //printf("bkt %d, obj %d = %d\n", j, oid, tmp.value);
        
        *(MSA_k + j) += tmp.value;
        *(Index + j) = tmp.position;

        int refPos = *(MSA_k + j) % refs;
        Acc += abs(refPosQ - refPos);
      }

      struct Node tmp = {oid, Acc};
      push_pq_fixed_size(PQ, tmp, knn);
      
    }

    delete[] MSA_k;
    delete[] Index;

    delete[] loi;
}

/* cMSA diff [u char] (A6) related */
void fullPermutationSearchingA6(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned char * * BARR, priority_queue_Node &PQ, int knn, int block_size) {

    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    
    int *MSA_k = new int[refs];
    int *Index = new int[refs];
    for (int j = 0; j < refs; j++) Index[j] = MSA_k[j] = 0;

    for (int oid = 0; oid < n; oid++) {
      double Acc = 0.0;
      for (int j = 0; j < refs; j++) {

        int refPosQ = j; //p(loi, loi[j], refs);//?
        int bucket = loi[j];

        Decoded tmp = deltaDecodeDiffA6(bucket, Index[j], BARR, block_size);

        //printf("bkt %d, obj %d = %d\n", bucket, oid, tmp.value);
        
        MSA_k[j] += tmp.value;
        Index[j] = tmp.position;

        int refPos = MSA_k[j] % refs;
        Acc += abs(refPosQ - refPos);
      }

      struct Node tmp = {oid, Acc};
      push_pq_fixed_size(PQ, tmp, knn);
      
    }

    delete[] MSA_k;
    delete[] Index;

    delete[] loi;
}

/* cMSA delta [vector] (A2) related */
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, vector<bool> *BS, priority_queue_Node &PQ, int knn) {

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

        int refPosQ = j; //p(loi, loi[j], refs);//?
        int bucket = loi[j];

        //int k = loi[j] * n + oid;

        //Decoded tmp = deltaDecodeDiff(bucket, Index[j], offset[bucket], BS);
        Decoded tmp = deltaDecodeDiff(bucket, Index[j], BS);
        //cout<<tmp.value<<" "<<tmp.position<<endl;
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

#if SDSL
/* original MSA (A10) related */
void fullPermutationSearching_sdsl(int refs, int dim, int n, int * q, int * refsR, struct Node *acc,  unsigned int * M, wt_sdsl &WT, priority_queue_Node &PQ, int knn){

    int *lq = new int[refs];
    createOrderedList(refs, dim, q, lq, refsR);
    
    /*
    for (int oid = 0; oid < n; oid++) {
        double Acc = 0.0; 
        for( int j = 0; j < refs; j++) {

          int refPosQ = j;
          long long int k = loi[j];
          k *= n;
          k += oid;
          //int refPos = M[k] % refs;
          
          int refPos = WT.select(oid+1, loi[j]) % refs;
          
          //if(M[k]!=WT.select(oid+1, loi[j]))
            //cout<<M[k]<<" "<<WT.select(oid+1, loi[j])<<" ERROR ***************\n";
          
          //acc[oid].d += abs(refPosQ - refPos);
          Acc += abs(refPosQ - refPos);
        }

        struct Node tmp = {oid, Acc};
        push_pq_fixed_size(PQ, tmp, knn);
        //acc[oid].d = Acc;
    }
    */
    
    int *inv_lq = new int[refs];
    for(int i=0; i<refs; i++)
      inv_lq[lq[i]] = i;
    
    double Acc = 0.0;
    for (int j = 0; j < n*refs; j++) {
       
          
      //int refPos = WT.select(oid+1, lq[j]) % refs;
      //int refPos = WT.select(oid+1, lq[j]);
      int refPos = WT[j];
      int idx_refPos = j % refs;
      
      //
      int refPosQ = inv_lq[refPos];
      /*
      if(M[k]!=WT.select(oid+1, lq[j]))
        cout<<M[k]<<" "<<WT.select(oid+1, lq[j])<<" ERROR ***************\n";
      */
      
      //acc[oid].d += abs(refPosQ - refPos);
      
      Acc += abs(refPosQ - idx_refPos);
      
      int oid = j / refs;
      
      //cout << Acc << "\t" << oid << endl;
      
      if(j % refs == refs-1){
        struct Node tmp = {oid, Acc};
        push_pq_fixed_size(PQ, tmp, knn);
        //acc[oid].d = Acc;
        Acc = 0.0;
      }
    }

    delete[] lq;
}
#endif

/* original MSA (A1) related */
void fullPermutationSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, unsigned int * M, priority_queue_Node &PQ, int knn) {

//    for (int i = 0; i < refs; i++) {
//        for (int j = 0; j < n; j++) {
//            printf("%d ", M[i][j]);
//        }
//        printf("\n");
//    }
    
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
          long long int k = loi[j];
          k *= n;
          k += oid;
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

/* original MSA melhorado (A8) related */
void fullPermutationSearching_m(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int * M, priority_queue_Node &PQ, int knn) {

//    for (int i = 0; i < refs; i++) {
//        for (int j = 0; j < n; j++) {
//            printf("%d ", M[i][j]);
//        }
//        printf("\n");
//    }
    
    //int_t t1 = time_start();
    /*
    for (int j = 0; j < n; j++) {
        struct Node tmp = {j, 0.0};
        acc[j] = tmp;
    }
    */
    int *lq = new int[refs];
    createOrderedList(refs, dim, q, lq, refsR);

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
          //int k = loi[j] * n + oid;
          //printf("k = %d   |  M[ %d, %d ] = \n", k, k / n, k % n);
          long long int index = refs;
          //index *= k % n;
          index *= oid;
          //index += k / n;
          index += lq[j];
          int refPos = M[index]; // % refs;

          //acc[oid].d += abs(refPosQ - refPos);
          Acc += abs(refPosQ - refPos);
        }

        struct Node tmp = {oid, Acc};
        push_pq_fixed_size(PQ, tmp, knn);
        //acc[oid].d = Acc;
    }

    delete[] lq;
}

/* Naive (A0) related */
void fullSequentialSearching(int refs, int dim, int n, int * q, int * refsR, struct Node *acc, int * * O, priority_queue_Node &PQ, int knn){
    
    int *loi = new int[refs];
    createOrderedList(refs, dim, q, loi, refsR);

    for (int oid = 0; oid < n; oid++) {
        double Acc = 0.0;
        for( int j = 0; j < refs; j++) {

          int refPosQ = j;
          //int refPos = p(O + oid * refs, loi[j], refs);
          int refPos = p(O[oid], loi[j], refs);

          Acc += abs(refPosQ - refPos);
        }

        struct Node tmp = {oid, Acc};
        push_pq_fixed_size(PQ, tmp, knn);
    }

    delete[] loi;
}

//keep PQ with maximum knn elements
void push_pq_fixed_size(priority_queue_Node &PQ, Node tmp, int knn){
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
  //printf("########\n");

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
    //cout << out<<" "<<bin<< "("<<out+bin<< ")\t";
    //bit_len(x);

    return out + bin;
}

string deltaEncode(int x) {

    string bin = toBinaryString(x);
    string out = gammaEncode(bin.length());
    
    out += bin.substr(1);

    return out;
}

int deltaEncodeInt(int x) {

    int y = bit_len(x);
    x &= ~mask[y-1];

    int l = bit_len(y)-1;    
    y = y << (y-1);
    y |= x;
    
    l += bit_len(y);

    //add bit 1 on (l+1)-th bit
    y = y|mask[l];

    //cout<<toBinaryString(y)<<endl;

    return y;
}

#if BOOST
/* cMSA delta [boost] (A4) related */
int deltaEncodeBoost(int x, int index, boost::dynamic_bitset<> * DBS) {

    int output = deltaEncodeInt(x);
    int len = bit_len(output) - 1;

    for (int i = len - 1; i >= 0; i--) {

        DBS[index].push_back( ( output & mask[i] ) >> i );
    }

    return len;
}
#endif

/* cMSA delta [vector] (A2) related */
int deltaEncode(int x, int index, vector<bool> *BS) {

    /*
    string output = deltaEncode(x);

    for (long unsigned i = 0; i < output.length(); i++) {
        BS[index].push_back(output[i]=='1'?1:0);
    }
    return offset_value + output.length();
    */

    int output = deltaEncodeInt(x);
    int len = bit_len(output)-1;

    /*
    cout<<x<<"\t";
    cout<<toBinaryString(x)<<"\t";
    cout<<toBinaryString(output)<<endl;
    */
    
    for (int i = len-1; i >= 0; i--) {
        //cout<<toBinaryString((output&mask[i])>>i)<<" ";
        BS[index].push_back((output&mask[i])>>i);
    }
    //cout<<endl;
    return len;
}

/* cMSA delta [u char] (A5) related */
int deltaEncode(int x, int index, unsigned char * * BARR, int * offset) {

    int output = deltaEncodeInt(x);
    int len = bit_len(output) - 1;

    for (int i = len - 1; i >= 0; i--) {

        //BS[index].push_back( ( output & mask[i] ) >> i );
        //BARR[index][ offset[index] ] = ( output & mask[i] ) >> i;  //  ??
        
        if ( (output >> i) & 1 ) {
            BARR[index][ offset[index] / 8 ] |= mask2[offset[index] % 8];
        }
        
        offset[index]++;
    }

    return len;
}

/* cMSA delta [u char] v3 (A9) related */
int deltaEncodeA9(int x, int index, unsigned char * BARR, int * offset, long long int * shiftArr) {

    int output = deltaEncodeInt(x);
    int len = bit_len(output) - 1;

    for (int i = len - 1; i >= 0; i--) {

        //BS[index].push_back( ( output & mask[i] ) >> i );
        //BARR[index][ offset[index] ] = ( output & mask[i] ) >> i;  //  ??
        
        if ( (output >> i) & 1 ) {
            //BARR[index][ offset[index] / 8 ] |= mask2[offset[index] % 8];
            long long int shift = *(shiftArr + index) + ( *(offset + index) >> 3);
            //BARR[ shift ] |= mask2[offset[index] % 8];
            *(BARR + shift ) |= *(mask2 + (*(offset + index) & 7) );
        }
        
        *(offset + index) += 1;
    }

    return len;
}

/* cMSA diff [u char] (A6) related */
int deltaEncodeA6(int x, int index, unsigned char * * BARR, int * offset, int block_size) {

    //printf("%d = ", x);
    
    for (int i = block_size - 1; i >= 0; i--) {

        if ( (x >> i) & 1 ) {
            BARR[index][ offset[index] / 8 ] |= mask2[offset[index] % 8];
        }
        
        //printf("%d", (x >> i) & 1);
        
        offset[index]++;
    }
    //printf("\n");

    return block_size;
}

#if BOOST
/* cMSA delta [boost] (A4) related */
Decoded deltaDecodeDiffBoost(int bsindex, int index, boost::dynamic_bitset<> * DBS) {

    int pos = index;
    
    int i=0;
    while(DBS[bsindex][index]!=1){
        
        index++;
        i++;
    }
    
    int l = 0;
    while(i >= 0){

      l |= (DBS[bsindex][index]) << i;
      index++;
      i--;
    }
    
    int y = 0;
    y |= 1<<(l-1);
      
    l--;
    while(l > 0){

      y |= (DBS[bsindex][index]) << (l - 1);
      index++;
      l--;
    }

    if(pos==0) y--;
    struct Decoded tmp = {y,index};
    return tmp;

}
#endif

/* cMSA delta [u char] (A5) related */
Decoded deltaDecodeDiff(int bsindex, int index, unsigned char * * BARR) {

    int pos = index;
    
    int i = 0;
    while ( ( (BARR[bsindex][index / 8] >> (7 - (index % 8))) & 1 ) == 0){
        
        index++;
        i++;
    }
    
    int l = 0;
    while (i >= 0){

      l |= ( (BARR[bsindex][index / 8] >> (7 - (index % 8))) & 1 ) << i;
      index++;
      i--;
    }
    
    int y = 0;
    y |= 1 << (l - 1);
      
    l--;
    while (l > 0){

      y |= ( (BARR[bsindex][index / 8] >> (7 - (index % 8))) & 1 ) << (l - 1);
      index++;
      l--;
    }

    if (pos == 0) y--;
    struct Decoded tmp = {y, index};
    return tmp;

}

/* cMSA delta [u char] v3 (A9) related */
Decoded deltaDecodeDiffA9(int bsindex, int index, unsigned char * BARR, long long int * shiftArr) {

    int pos = index;
    
    int i = 0;
    //long long int shift = (long long int) bsindex * max_bucket;
    long long int shift = *(shiftArr + bsindex);
    
    while ( !( ( *(BARR + (shift + (index >> 3))) >> (7 ^ (index & 7))) & 1 ) ){
        
        index++;
        i++;
    }
    
    int l = 0;
    while (i >= 0){

      l |= ( ( *(BARR + (shift + (index >> 3))) >> (7 ^ (index & 7))) & 1 ) << i;
      index++;
      i--;
    }
    
    int y = 0;
    y |= 1 << (l - 1);
      
    l--;
    while (l > 0){

      y |= ( ( *(BARR + (shift + (index >> 3))) >> (7 ^ (index & 7))) & 1 ) << (l - 1);
      index++;
      l--;
    }

    if (pos == 0) y--;
    struct Decoded tmp = {y, index};
    return tmp;

}

/* cMSA diff [u char] (A6) related */
Decoded deltaDecodeDiffA6(int bsindex, int index, unsigned char * * BARR, int block_size) {

    int pos = index;
    int bit = 0;

    int y = 0;
    for (int i = block_size - 1; i >= 0; i--) {
        bit = (BARR[bsindex][index / 8] >> (7 - (index % 8))) & 1;
        //printf("%d", bit);
        y |= (bit << i);
        index++;
    }
    
    //printf(" = %d\n", y);
    
    if (pos == 0) y--;
    struct Decoded tmp = {y, index};
    return tmp;

}

/* cMSA delta [vector] (A2) related */
Decoded deltaDecodeDiff(int bsindex, int index,  vector<bool> *BS) {
/*
    int offset_value = BS[bsindex].size();
    
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

*/
    int pos = index;
    
    int i=0;
    while(BS[bsindex][index]!=1){
        //cout<<BS[bsindex][index]<<" ";
        index++;
        i++;
    }
    //cout<<"| ";
    
    int l = 0;
    while(i >= 0){
      //cout<<BS[bsindex][index]<<" ";
      l |= (BS[bsindex][index])<<i;
      index++;
      i--;
    }
    
    //cout<<"["<<l<<"] | ";
    
    int y = 0;
    y |= 1<<(l-1);
    
    //cout<<"("<<toBinaryString(y)<<") | ";
    
    l--;
    while(l > 0){
      //cout<<BS[bsindex][index]<<" ";
      y |= (BS[bsindex][index])<<(l-1);
      index++;
      l--;
    }
    
    //cout<<"\t# "<<y<<" #\t"<<toBinaryString(y)<<"\t("<<index<<")"<<endl;
    
    if(pos==0) y--;//???
    struct Decoded tmp = {y,index};
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

/* para teste de codificação delta. Retorna a string do código */
string eliasDeltaEncode(int num) {
    
    string code = "";
    int len = 1 + floor( log2(num) );
    int lengthOfLen = floor( log2(len) );
    
    for (int i = lengthOfLen; i > 0; i--) {
        code += "0";
    }
    for (int i = lengthOfLen; i >= 0; i--) {
        code += std::to_string( (len >> i) & 1 );
    }
    for (int i = len - 2; i >= 0; i--) {
        code += std::to_string( (num >> i) & 1 );
    }
    
    return code;
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

// Used for EXACT SEARCH
float distance(int *o1, int *o2, int dim) {
    float d = 0;
    #ifdef MANHATTAN
    for (int i = 0; i < dim; i++) d += fabs(o1[i] - o2[i]);
    #endif
    #ifdef EUCLIDEAN
    for (int i = 0; i < dim; i++) 
        d += (o1[i] - o2[i]) * (o1[i] - o2[i]);
    d = sqrt(d);
    #endif
    return d;
}

// Used for EXACT SEARCH: red black tree to index pairs of < distance, offset >
class result {
  private:
    typedef multimap<float, int> mapType; // multimap allows duplicate keys, maps ignores duplicate keys
    mapType tree;
    mapType::iterator iter;
    float maxkey;
  public:

    result() { maxkey = -1; }
    
    ~result() { tree.clear(); }

    void add(float key, int value) { tree.insert(pair<float, int>(key, value)); if (key > maxkey) maxkey = key; }

    void remove(float key) {
        iter = tree.find(key);
        if (iter != tree.end()) {
            tree.erase(iter);
        }
    }

    void removelast() { 
        iter = tree.end();
        --iter;
        tree.erase(iter);
        iter = tree.end();
        --iter;
        maxkey = getkey();
    }
    
    void clear() { tree.clear(); }

    void prior() { --iter; }

    void next() { ++iter; }

    bool eof() { if (iter == tree.end()) return true; return false; }

    bool bof() { if (iter == tree.begin()) return true; return false; }

    float getkey() { return iter->first; }

    int getvalue() { return iter->second; }
    
    float getmaxkey() { return maxkey; } 

    void begin() { iter = tree.begin(); }

    void end() { iter = tree.end(); }

    int find(float key) {
        iter = tree.find(key);
        if (iter != tree.end()) {
            return iter->second;
        }
        else {
            return -1;
        }
    }

    int count() { return tree.size(); }
};

// Used for EXACT SEARCH: sequential scan
int exact(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q){

    time_t t_start=0;
    clock_t c_start=0;
    int ignorevalue;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    //if(r!=0 and r<refs) refs = r;
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    /********************/
    int datasetrefs;
    if (r != 0) {
        if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

        datasetrefs = refs;
        refs = r;

        // let's ignore the first (datasetrefs - refs) references
        int ignore;
        for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
    }
    /********************/

    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    int datasetn = n;
    if (num_o != 0 and num_o < n) n = num_o;

    /********************/
    
    if(time) time_start(&t_start, &c_start);
        
    // read references
    for (int i = 0; i < dim * refs; i++) f_in >> ignorevalue; //referencias

    // Read objects
    // goal is to ignore objects, in order to be able read the queries. Later we will read the objects again
    for (int i = 0; i < n; i++) {
        for (int kk = 0; kk < dim; kk++) {
            f_in >> ignorevalue;
        }
    }

    // Read non-used objects: datasetn - n (datasetn é igual ao n do arquivo de dados pois n = num_o)
    int ignore;
    for (int i = 0; i < dim * (datasetn - n); i++) f_in >> ignore; // registros não utilizados

    #ifdef DEBUG_EXACT_SEARCH
    cout << endl << "First 10 queries:" << endl;
    #endif
    // Read queries
    int **queries = new int*[num_q];
    for (int i = 0; i < num_q; i++) {
        queries[i] = new int[dim];
        for (int j = 0; j < dim; j++) { 
            f_in >> queries[i][j]; 
            #ifdef DEBUG_EXACT_SEARCH
            if (i < 10) cout << queries[i][j] << " ";
            #endif
        }
        #ifdef DEBUG_EXACT_SEARCH
        if (i < 10) cout << endl;
        #endif
    }

    // Goal was to read queries: let's close the input file
    f_in.close();
    
    if(verbose)
        cout << "dim = " << dim << "; refs = " << refs << "; n = " << n  << "; q = " << num_q << endl;

    if(time){
      cout << "Read objects:" << endl;
      time_stop(t_start, c_start);
    }
    
    if(knn>0){//knn queries

        // Goal now it to read the objects
        std::fstream f_in(s_in, std::ios_base::in);
        if(!f_in.is_open()) exit(EXIT_FAILURE);

        int dim, refs, n, num_q;

        f_in >> dim; //dimensoes
        f_in >> refs; //referencias (pivos)
        //if(r!=0 and r<refs) refs = r;
        f_in >> n; //objetos
        f_in >> num_q; //consultas

        /********************/
        int datasetrefs;
        if (r != 0) {
            if (r > refs) { cout << "cannot run with this number of references (value is greater than the number of references in the datafile" << endl; exit(0); };

            datasetrefs = refs;
            refs = r;

            // let's ignore the first (datasetrefs - refs) references
            int ignore;
            for (int i = 0; i < dim * (datasetrefs - refs); i++) f_in >> ignore; //referencias
        }
        /********************/

        if (sel_q != 0) {
            if (sel_q > num_q) n = n - (sel_q - num_q);
            num_q = sel_q;
        }
        
        int datasetn = n;
        if (num_o != 0 and num_o < n) n = num_o;

        /********************/        
        
        // read references
        for (int i = 0; i < dim * refs; i++) f_in >> ignorevalue; //referencias

        result *results = new result[num_q];

        std::fstream f_out;
        if(output){
          f_out.open(s_out, std::ios_base::out);
          if(!f_out.is_open()) exit(EXIT_FAILURE);
          if(verbose) cout << s_out <<endl;
        }

        if(time) time_start(&t_start, &c_start);

        float d, max;

        #ifdef DEBUG_EXACT_SEARCH
        cout << endl << "First 10 objects:" << endl;
        #endif

        // Read objects: for each object, consider it to be included in the result of each query
        int *obj = new int[dim];
        for (int i = 0; i < n; i++) {
            // read 1 object
            for (int kk = 0; kk < dim; kk++) {
                f_in >> obj[kk];
                #ifdef DEBUG_EXACT_SEARCH
                if (i < 10) cout << obj[kk] << " ";
                #endif
            }
            #ifdef DEBUG_EXACT_SEARCH
            if (i < 10) cout << endl;
            #endif

            // for each query
            for (int j = 0; j < num_q; j++) {
                d = distance(queries[j], obj, dim);
                // add if count < k
                if (results[j].count() < knn) {
                    results[j].add(d, i);
                }
                // otherwise, add if distance is smaller than maxkey and remove last
                else {
                    max = results[j].getmaxkey();
                    if (d < max) {
                        results[j].add(d, i);
                        results[j].removelast();
                    }
                }
            }
        }
        delete[]obj;
        // for each query
        for (int i = 0; i < num_q; i++) {
            if(output) write_output(f_out, &(results[i]));
        }

        if(output) f_out.close();

        f_in.close();

        if(time){
          cout << "Searching:" << endl;
          time_stop(t_start, c_start);
        }
    }

    // clean memory
    for (int i = 0; i < num_q; i++)
        delete[]queries[i];
    delete[]queries;

    return 0;
}

// Used for EXACT SEARCH
int write_output(std::fstream& f_out, result *r){
    r->begin();
    int key;
    while (!r->eof()) {
        key = r->getvalue();
        f_out << key << " ";
        r->next();
    }
    f_out << endl;

    return 0;
}
