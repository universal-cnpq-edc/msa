#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

class Index {
  vector<vector<double>> objects;
  vector<vector<double>> references;

  vector<vector<int>> L; // i-th vector in L contains list of references ordered by distance to i-th object in objects

  public:
  Index(vector<vector<double>> base, vector<vector<double>> refs) {
    objects = base;
    references = refs;

    build_L();
  }

  void build_L() {
    for (int i = 0; i < objects.size(); i++ ){
//      cout << "sorting object "<< i<<": \n";
      vector<int> sorted_references = sort_by_distance(objects[i]);
//      cout << i << " : ";
//      for (int i = 0; i < sorted_references.size() -1; i++) {
//        cout << sorted_references[i] << ", ";
//      }
//      cout << sorted_references[sorted_references.size()-1] <<"\n";
      L.push_back(sorted_references);
    }
  }

  void print_L() {
	  for (int o_i = 0; o_i < objects.size(); o_i++) {
		  cout << "L_o_"<< o_i<< " == "<<o_i<<": ";
		  for (int l = 0; l < L[o_i].size(); l++) {
			  cout << "r_"<<L[o_i][l] << " ";
		  }
		  cout << "\n";
	  }
  }

  vector<int> sort_by_distance(vector<double> o) {

    vector<pair<double, int>> dists(references.size());

    for (int j = 0; j < references.size(); j++) {
      double dist = distance(o, references[j]);
      dists[j] = make_pair(dist, j);
    }

    sort(dists.begin(), dists.end());

    vector<int> ret(references.size());
    for (int i = 0; i < dists.size(); i++) {
      ret[i] = dists[i].second;
    }

    return (ret);
  }

  double distance(vector<double> o, vector<double> r) {
    int D = o.size();
    double dist = 0;
    for (int i = 0; i < D; i++) {
      double tmp = o[i]-r[i];
      dist += tmp*tmp;
    }
    return sqrt(dist);
  }

  int SFD(vector<double> q, int oi) {
    int s = 0;

    vector<int> Lq = sort_by_distance(q);

    for (int r = 0; r < references.size(); r++) {

      int P_oi_r = -1;
      for (int i = 0; i < L[oi].size(); i++) { 
        if (L[oi][i] == r) {
          P_oi_r = i;// position in L_oi where r is
          break;
        }
      }

      int P_Lq = -1;
      for (int j = 0; j < Lq.size(); j++) { 
        if (Lq[j] == r) {
          P_Lq = j; // position in L_q where r is
          break;
        }
      }
      
      s += (int) abs(P_oi_r - P_Lq);
    }

    return s;
  }
  
  vector<int> knn(vector<double> q, int k) {

    vector<pair<int,double>> dists(objects.size());

    for (int i = 0; i < objects.size(); i++) {
      dists[i] = make_pair(SFD(q, i), i);
      //cout << "SFD(q, "<<i<<") == "<<dists[i].first<<"\n";
    }

    sort(dists.begin(), dists.end());

    vector<int> nn;
    for (int i = 0; i < k && i < dists.size(); i++) {
      nn.push_back(dists[i].second);
    }

    // considerar o empate -- se fosse range-query seria mais fácil
    while ((k < dists.size()-1) &&
           (dists[k-1].first == dists[k].first)) {
      nn.push_back(dists[k].second);
      k++;
    }

    return nn;
  }
};

int main() { 

  int L, N, n, Q;
  cin >> L >> N >> n >> Q;
  
  vector<vector<double>> base(N);

  for (int i = 0; i < N; i++) { // leitura da base de objetos
     vector<double> o(L);
     for (int l = 0; l < L; l++) 
       cin >> o[l];

     base[i] = o;
  }

  vector<vector<double>> refs(n);
  for (int j = 0; j < n; j++) { // leitura dos objetos de referência
     vector<double> r(L);
     for (int l = 0; l < L; l++) 
       cin >> r[l];

     refs[j] = r;
  }

  Index idx(base, refs);

  for (int qi = 0; qi < Q; qi++) { // leitura dos objetos de consulta
     vector<double> q(L);                           
     for (int l = 0; l < L; l++) 
       cin >> q[l];

     int k; // from K-nn
     cin >> k;

//     cout << "Return knn:\n";
     vector<int> nn = idx.knn(q, k);
     for (int i = 0; i < nn.size(); i++) {
        cout << nn[i] << " ";
     }
     cout << "\n";
  }
   //  idx.print_L();

}
