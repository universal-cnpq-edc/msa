#ifndef DEBUG
  #define DEBUG 0
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
#include "../../external/malloc_count/malloc_count.h"

#if _WIN32
    #include <windows.h>
    typedef __int64  int_t;
#else
    typedef int64_t  int_t;
#endif


#include <cstring>

#include <bitset>
#include <algorithm>

//#define MANHATTAN
#define EUCLIDEAN
    
using namespace std;

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

// red black tree to index pairs of < distance, offset >
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

void time_start(time_t *t_time, clock_t *c_clock);
double time_stop(time_t t_time, clock_t c_clock);

int write_output(std::fstream& f_out, result *r);

int exact(string s_in, string s_out, int output, int verbose, int time, int knn, int r, int num_o, int sel_q);

void usage(char *name){
    puts("Available options:");
    puts("\t-A a\talgorithm to use (default -1)");
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
    int ALG=-1;//Algorithm

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
            cout<<"## exact search ##"<<endl;//no compression
            exact(s_in, s_out, output, verbose, time, knn, r, num_o, sel_q);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
    }
    return 0;
}

//sequential scan
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
    if (r != 0) {
        if (r > refs) n = n - (r - refs);
        refs = r;
    }
    
    if (sel_q != 0) {
        if (sel_q > num_q) n = n - (sel_q - num_q);
        num_q = sel_q;
    }
    
    if (num_o != 0 and num_o < n) n = num_o;
    /********************/

    if(time) time_start(&t_start, &c_start);
        
    // read references
    for (int i = 0; i < dim * refs; i++) f_in >> ignorevalue; //referencias

    // Read objects
    // goal is to ignore objects, in order to be able read the references. Later we will read the objects again
    for (int i = 0; i < n; i++) {
        for (int kk = 0; kk < dim; kk++) {
            f_in >> ignorevalue;
        }
    }

    // Read queries
    int **queries = new int*[num_q];
    for (int i = 0; i < num_q; i++) {
        queries[i] = new int[dim];
        for (int j = 0; j < dim; j++) { 
            f_in >> queries[i][j]; 
        }
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

        // Read objects: for each object, consider it to be included in the result of each query
        int *obj = new int[dim];
        for (int i = 0; i < n; i++) {
            // read 1 object
            for (int kk = 0; kk < dim; kk++) {
                f_in >> obj[kk];
            }
            
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
