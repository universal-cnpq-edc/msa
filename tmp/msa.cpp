#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

class Index {
  vector<vector<double>> objects;
  vector<vector<double>> references;

  vector<int> MSA; //  proposto por Mohamed & Marchand-Maillet
  int n, N;

  public:
  Index(vector<vector<double>> base, vector<vector<double>> refs) {
    objects = base;
    references = refs;

    n = references.size();
    N = objects.size();

    vector<int> m(n * N);
    MSA = m;
    build_MSA();
  }

  void build_MSA() {
    for (int i = 0; i < objects.size(); i++ ){
      vector<int> sorted_references = sort_by_distance(objects[i]);

      // for  r \in L_{o_i}
      // where  "rj .id × N deﬁnes the start of the associated bucket id"
      // (oi .id × n) + P (Loi , rj ) deﬁnes the global index of rj in the string S
      // MSA[(r_j.id * N) + o_i.id] = (o_i.id * n) + P(L_{o_i}, r_j);
      for (int j = 0; j < sorted_references.size(); j++) {
        int r_j_id = sorted_references[j];//  + n; // FIXME -- verificar se necessario usar +n para identificar unicamente o id da referencia com simbolo em alfabeto unificado entre os numeros dos objects e das references
        MSA[ (r_j_id * N) + i ] = (i * n)  + j;
      }
    }
  }

  void print_MSA() {
	  for (int i = 0; i < n; i++) {
		  cout <<" buk_r"<< i<<": \n";
		  for (int j = 0; j < N; j++) {
			  cout<<MSA[j+ N*i]<<" ";
		  }
		  cout <<"\n";
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

  // Algorithm 2 Full permutation searching
  // TODO DEBUG este!
  vector<int> knn(vector<double> q, int k) {
    vector<int> A(N, 0);

    vector<int> Lq = sort_by_distance(q);

//    cout << "Lq == ";
//    for (int l = 0; l < Lq.size(); l++) 
//	    cout << Lq[l] << " ";
//    cout <<"\n";

    for (int j = 0; j < Lq.size(); j++) { 
      //cout << "Vai processar a "<<j<<"-esima referencia mais próxima: "<<Lq[j]<<"\n";
      for (int k = Lq[j] * N ; k < (Lq[j] * N) + N; k++) {
        int O_id = (MSA[k] - (MSA[k] % n))/n;
        int RefPos = (MSA[k] % n) ; // no paper está +1 mas não faz sentido pq j começa em 0...e talvez nem faça diferença
	//cout << "Comparando "<< RefPos << " com "<< j<<"\n";
        A[O_id] = A[O_id] +  (int) abs(j -RefPos);
      }
    }

    vector<pair<int, int>> dists(A.size());
    for (int l = 0; l < A.size(); l++) 
	    dists[l] = make_pair(A[l],l);

    sort(dists.begin(), dists.end());

    vector<int> nn;
    for (int i = 0; i < k && i < A.size(); i++) {
      nn.push_back(dists[i].second);
    }
    //
    // considerar o empate -- se fosse range-query seria mais fácil
    while ((k < dists.size()-1) &&
           (dists[k-1].first == dists[k].first)) {
      nn.push_back(dists[k].second);
      k++;
    }
    return(nn);
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

  //idx.print_MSA();

}
