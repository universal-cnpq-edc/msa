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

int **getqueries(string s_in, int *dim, int *refs, int *n, int *num_q);
int **getresults(string exactfile_in, int num_q, int k);
int discoverk(string s_in);
int **getobjects(string s_in);
void compareanwers(int **exact_ids, int **approx_ids, int num_q, int k);

int main(int argc, char** argv) {

    if (argc != 4) {
        printf("This program compares the output between a set of exact and approximate searches.\n");
        printf("Usage: this program receives 3 file names\n\n");
        printf("       ./compare datafile exactresult approximate \n");
        return 0;
    }

    string datafile_in = argv[1];
    string exactfile_in = argv[2];
    string approxfile_in = argv[3];
    string file_out = datafile_in.substr(0, datafile_in.rfind('.', datafile_in.length()) ) + ".compare";

    int dim, refs, n, num_q;
   
    int **queries = getqueries(datafile_in, &dim, &refs, &n, &num_q);

    int k = discoverk(exactfile_in);
    int **exact_ids = getresults(exactfile_in, num_q, k);
    int **approx_ids = getresults(approxfile_in, num_q, k);
    
    // % of correct answers of the approximate query compared to the exact query
    compareanwers(exact_ids, approx_ids, num_q, k);
    
    int **objects = getobjects(datafile_in);

    float d, maxdist;
    float avgmaxdist = 0;
    int avg_position_farthest = 0;

    // for each query
    for (int i = 0; i < num_q; i++) {
        // encontrar a distancia r do k-esimo obj na resposta aproximada
        maxdist = 0;
        for (int j = 0; j < k; j++) {
            int tmppos = approx_ids[i][j];
            d = distance(queries[i], objects[ approx_ids[i][j] ], dim);
            if (d > maxdist)
                maxdist = d;
        }
       
        // fazer consulta por abrangência exata com raio r para computar quantos elementos precisa
        int count = 0;
        int foundequal = 0;
        for (int j = 0; j < n; j++) {
            d = distance(queries[i], objects[j], dim);
            // it is necessary to find only 1 with maxdist distance
            if (foundequal) {
                if (d < maxdist)
                    count++;
            }
            else {
                if (d <= maxdist)
                    count++;
                if (d == maxdist)
                    foundequal = 1;
            }
        }
        //printf("maxdist=%.1f -- the farthest obj found by the approximate is the %d-th nearest neighbor\n", maxdist, count);
        avgmaxdist += maxdist;
        avg_position_farthest += count;
    }
    // compute average
    avgmaxdist /= num_q;
    avg_position_farthest /= num_q;
    printf("average maxdist %.1f -- the farthest obj found by the approximate %d-NN is on avg the %d-th nearest neighbor\n", avgmaxdist, k, avg_position_farthest);
    
    return 0;
}

void compareanwers(int **exact_ids, int **approx_ids, int num_q, int k) {
    // % of correct answers of the approximate query compared to the exact query
    
    // for each query
    int correct_count = 0;
    int *correct_counts = new int[num_q];
    float *percs  = new float[num_q];
    for (int i = 0; i < num_q; i++) {
        correct_counts[i] = 0;
        // for each obj in the exact query
        for (int j = 0; j < k; j++) {
            // check if the obj is in the approximate result
            for (int m = 0; m < k; m++) {
                if (exact_ids[i][j] == approx_ids[i][m]) {
                    correct_count++;
                    correct_counts[i]++;
                }
            }
        }
        percs[i] = correct_counts[i] / (float) k * 100.0;
        //printf("\n[%d] %.1f %% correct", i, percs[i]);
    }
    float perc = correct_count / (float)(num_q * k) * 100.0;
    float squared_sum = 0, sttdev;
    for (int i = 0; i < num_q; i++)
        squared_sum += (perc - percs[i]) * (perc - percs[i]);
    sttdev = sqrt(squared_sum / num_q);
    printf("avg and stddev correct answers of the approximate query compared to the exact query: avg %.1f%% stddev %f%%\n", perc, sttdev);

    return;    
}

int **getobjects(string s_in) {

    int ignorevalue;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    int dim, refs, n, num_q;

    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    
    // read references
    for (int i = 0; i < dim * refs; i++) f_in >> ignorevalue; //referencias

    // read objects
    int **objs = new int*[n];
    for (int i = 0; i < n; i++) {
        objs[i] = new int[dim];
        for (int j = 0; j < dim; j++) { 
            f_in >> objs[i][j]; 
        }
    }

    f_in.close();
    return objs;
}

int discoverk(string s_in) {
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);
    
    char line[10000];
    f_in.getline(line,10000);
    //printf("%s\n", line);

    int tokens = 0;
    char *p = strtok (line," ");
    while (p != NULL) {
        //printf ("%s\n",p);
        p = strtok (NULL, " ");
        tokens++;
    }    
    f_in.close();
    return tokens;
}

int **getresults(string s_in, int num_q, int k) {
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);
   
    int **results = new int*[num_q];
    for (int i = 0; i < num_q; i++) {
        results[i] = new int[k];
        for (int j = 0; j < k; j++)
            f_in >> results[i][j]; 
    }
    
    f_in.close();

    //return queries;
    return results;
}

//sequential scan
int **getqueries(string s_in, int *dim, int *refs, int *n, int *num_q) {

    int ignorevalue;

    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) exit(EXIT_FAILURE);

    //int dim, refs, n, num_q;

    f_in >> *dim; //dimensoes
    f_in >> *refs; //referencias (pivos)
    f_in >> *n; //objetos
    f_in >> *num_q; //consultas
    
    // read references
    for (int i = 0; i < *dim * *refs; i++) f_in >> ignorevalue; //referencias

    // read objects
    for (int i = 0; i < *n; i++) for (int kk = 0; kk < *dim; kk++) f_in >> ignorevalue;

    // read queries
    int **queries = new int*[*num_q];
    for (int i = 0; i < *num_q; i++) {
        queries[i] = new int[*dim];
        for (int j = 0; j < *dim; j++) { 
            f_in >> queries[i][j]; 
        }
    }

    f_in.close();
    
    cout << "dim = " << *dim << "; refs = " << *refs << "; n = " << *n  << "; q = " << *num_q << endl;

    return queries;
}
