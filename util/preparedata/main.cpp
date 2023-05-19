#include <cstdlib>
#include <cstring>
#include <math.h>
#include <iostream>
#include <fstream>

#include <arboretum/stBasicObjects.h>
#include <arboretum/stBasicMetricEvaluators.h>
typedef stBasicArrayObject <float, int> tBasicArrayObject;
typedef stBasicEuclideanMetricEvaluator < tBasicArrayObject > tBasicEuclideanMetricEvaluator;

using namespace std;

typedef struct Reference { int ID; int position; tBasicArrayObject *obj; } reference;

reference * GetSortedReferences(int size, int numrefs) {
    reference *refs = new reference[numrefs];

    int position, found;
    for (int i = 0; i < numrefs; i++) {
        // let's select a position that was not previously selected
        do {
            found = 0;
            position = rand()%size;
            // search vector ids from positions zero to i-1
            for (int j = 0; j < i; j++)
                if (position == refs[j].ID)
                    found = 1;
        } while (found);
        refs[i].obj = NULL;
        refs[i].ID = position;
        refs[i].position = i; // in this function, we will sort based on IDs, but before writing to final file, we will sort by the position (to keep the random sorting of the IDs, not their order in the original file)
    }
    
    // sort based on IDs

    // we are able to write better code. So why n^2 sorting?
    int aux1, aux2;
    for (int i = 0; i < numrefs-1; i++) {
        for (int j = i+1; j < numrefs; j++) {
            if (refs[i].ID > refs[j].ID) {
                aux1 = refs[i].ID;
                aux2 = refs[i].position;

                refs[i].ID =       refs[j].ID;
                refs[i].position = refs[j].position;

                refs[j].ID       = aux1;
                refs[j].position = aux2;
            }    
        }
    }
    return refs;
}

void SortedReferencesBasedOnSortOrder(reference *refs, int numrefs) {
    // sort based on sort order instead of ID

    // we are able to write better code. So why n^2 sorting?
    int aux1, aux2;
    tBasicArrayObject *aux3;
    for (int i = 0; i < numrefs-1; i++) {
        for (int j = i+1; j < numrefs; j++) {
            if (refs[i].position > refs[j].position) {
                aux1 = refs[i].ID;
                aux2 = refs[i].position;
                aux3 = refs[i].obj;

                refs[i].ID       = refs[j].ID;
                refs[i].position = refs[j].position;
                refs[i].obj      = refs[j].obj;

                refs[j].ID       = aux1;
                refs[j].position = aux2;
                refs[j].obj      = aux3;
            }    
        }
    }
}


//tBasicArrayObject *FindPivot(int embedding_id, tBasicArrayObject **vantage_objs, int numobjects, tBasicArrayObject **objects);
//tBasicArrayObject **GetReferences(tBasicArrayObject **objs, int size, int numrefs);
//tBasicArrayObject **GetQueries(tBasicArrayObject **objs, int size, int numq);

int main(int argc, char** argv) {

    srand (time(NULL));

    if (argc != 3) {
        printf("Usage: ./preparedata inputfile outputfile \n\n");
        printf("       input file: \n");
        printf("          dim\n");
        printf("          numrefs\n");
        printf("          numlines\n");
        printf("          numqueries\n");
        printf("          id_present\n");
        printf("          numlines records\n");
        return 0;
    }
    string s_in = argv[1];
    string s_out = argv[2];

    if (s_in == s_out) {
         cout << endl << "Input and output file names should not be the same (" << s_in << ")." << endl << endl; exit(0);
    }

    // read input
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) { cout << "Could not read input file " << s_in << endl << endl; exit(EXIT_FAILURE); }

    // write output
    std::fstream f_out;
    f_out.open(s_out, std::ios_base::out);  
    if(!f_out.is_open()) { cout << "Could not write output file " << s_out << endl << endl; exit(EXIT_FAILURE); }

    // write output
    std::fstream f_out_temp_instances;
    f_out_temp_instances.open("temp_instances_" + s_out, std::ios_base::out);  
    if(!f_out_temp_instances.is_open()) { cout << "Could not write output file " << "temp_instances_" << s_out << endl << endl; exit(EXIT_FAILURE); }
    
    int dim, num_refs, n, num_queries, idpresent;
    f_in >> dim;       //dimensoes
    f_in >> num_refs;  //referencias (pivos)
    f_in >> n;         //num instances
    f_in >> num_queries; //consultas
    f_in >> idpresent; // 0 if ID is absent or 1 if present

    // write dataset properties    
    f_out << dim << " ";
    f_out << num_refs << " ";
    f_out << n << " ";
    f_out << num_queries << endl;
    
    reference *references = GetSortedReferences(n, num_refs);
    reference *queries = GetSortedReferences(n, num_queries);
    
    cout << "Reading input..." << endl;
    int aux; 
    float *tuple = new float[dim];
    int referenceposition = 0, queryposition = 0;
    // for each line
    for (int i = 0; i < n; i++) {
        // read all features
        if (idpresent)
            f_in >> aux; // read the ID
        for (int j = 0; j < dim; j++)
            f_in >> tuple[j];
        tBasicArrayObject *obj = new tBasicArrayObject(dim, tuple);
        obj->SetOID(i);
        
        if (i == references[referenceposition].ID) {
            references[referenceposition].obj = obj->Clone();
            referenceposition++;
        }
        
        if (i == queries[queryposition].ID) {
            queries[queryposition].obj = obj->Clone();
            queryposition++;
        }

        // write instance (count=n)
        for (int j = 0; j < dim; j++) {
            f_out_temp_instances << tuple[j]; 
            if (j < dim-1) f_out_temp_instances << " ";
        }
        f_out_temp_instances << endl;
        
        delete obj;
    }
    
    // sort references based on sorting order (instead of ID order);
    SortedReferencesBasedOnSortOrder(references, num_refs);
    
    //f_out << "# inicio referencias" << endl;
    
    // write reference objects (count=refs)
    cout << "Writing references..." << endl;
    for (int i = 0; i < num_refs; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << references[i].obj->Get(j); 
            if (j < dim-1) f_out << " ";
        }
        f_out << endl;
    }

    //f_out << "# fim referencias" << endl;

    //f_out << "# inicio instancias" << endl;
    
    f_out_temp_instances.close();
    std::fstream f_in_temp_instances("temp_instances_" + s_out, std::ios_base::in);
    if(!f_in_temp_instances.is_open()) { cout << "Could not read input file " << "temp_instances_" + s_out << endl << endl; exit(EXIT_FAILURE); }

    float fvalue;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < dim; j++) {
            f_in_temp_instances >> fvalue;
            f_out << fvalue;
            if (j < dim-1) f_out << " ";
        }
        f_out << endl;
    }

    //f_out << "# fim instancias" << endl;

    // sort queries based on sorting order (instead of ID order);
    SortedReferencesBasedOnSortOrder(queries, num_queries);
    
    //f_out << "# inicio queries" << endl;
    
    // write query objects (count=num_queries)
    cout << "Writing queries..." << endl;
    for (int i = 0; i < num_queries; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << queries[i].obj->Get(j); 
            if (j < dim-1) f_out << " ";
        }
        f_out << endl;
    }

    //f_out << "# fim queries" << endl;
    
    f_in.close();
    f_out.close();
    f_in_temp_instances.close();
    
    //cout << "Wrote " << refs << " references, " << n << " objects, and " << num_q << " queries." << endl << endl;

    delete[]tuple;
    
    return 0;

/*
    
    cout << "Computing references..." << endl;
    tBasicArrayObject **references = GetReferences(data, n, refs);
    tBasicArrayObject **queries = GetQueries(data, n, num_q);

    // write dataset properties    
    f_out << dim << " ";
    f_out << refs << " ";
    f_out << n << " ";
    f_out << num_q << endl;
        
    // write reference objects (count=refs)
    cout << "Writing references..." << endl;
    for (int i = 0; i < refs; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << references[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }
    // write instances (count=n)
    cout << "Writing objects..." << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << data[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }
    // write queries (count=num_q)
    cout << "Writing queries..." << endl;
    for (int i = 0; i < num_q; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << queries[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }

    f_out.close();
    
    cout << "Wrote " << refs << " references, " << n << " objects, and " << num_q << " queries." << endl << endl;

    delete[]tuple;
    for (int i = 0; i < n; i++)
        delete data[i];
    delete[]data;
    delete[]queries;
    delete[]references;
    return 0;
    
    
    /*
     // Version 1: loads data into memory

    srand (time(NULL));

    if (argc != 3) {
        printf("Usage: ./preparedata inputfile outputfile \n\n");
        printf("       input file: \n");
        printf("          dim\n");
        printf("          numrefs\n");
        printf("          numlines\n");
        printf("          numqueries\n");
        printf("          id_present\n");
        printf("          numlines records\n");
        return 0;
    }
    string s_in = argv[1];
    string s_out = argv[2];

    if (s_in == s_out) {
         cout << endl << "Input and output file names should not be the same (" << s_in << ")." << endl << endl; exit(0);
    }

    // read input
    std::fstream f_in(s_in, std::ios_base::in);
    if(!f_in.is_open()) { cout << "Could not read input file " << s_in << endl << endl; exit(EXIT_FAILURE); }

    // write output
    std::fstream f_out;
    f_out.open(s_out, std::ios_base::out);  
    if(!f_out.is_open()) { cout << "Could not write output file " << s_out << endl << endl; exit(EXIT_FAILURE); }

    int dim, refs, n, num_q, idpresent;
    f_in >> dim; //dimensoes
    f_in >> refs; //referencias (pivos)
    f_in >> n; //objetos
    f_in >> num_q; //consultas
    f_in >> idpresent; // 0 if ID is absent or 1 if present
    
    cout << "Reading input..." << endl;
    int aux; 
    float *tuple = new float[dim];
    tBasicArrayObject **data = new tBasicArrayObject*[n];
    // for each line
    for (int i = 0; i < n; i++) {
        // read all features
        if (idpresent)
            f_in >> aux; // read the ID
        for (int j = 0; j < dim; j++)
            f_in >> tuple[j];
        data[i] = new tBasicArrayObject(dim, tuple);
        data[i]->SetOID(i);
    }
    cout << "Computing references..." << endl;
    tBasicArrayObject **references = GetReferences(data, n, refs);
    tBasicArrayObject **queries = GetQueries(data, n, num_q);

    // write dataset properties    
    f_out << dim << " ";
    f_out << refs << " ";
    f_out << n << " ";
    f_out << num_q << endl;
        
    // write reference objects (count=refs), objects (count=n) and queries (count=num_q)
    cout << "Writing output..." << endl;
    for (int i = 0; i < refs; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << references[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << data[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }
    for (int i = 0; i < num_q; i++) {
        for (int j = 0; j < dim; j++) {
            f_out << queries[i]->Get(j); if (j<dim-1) f_out << " ";
        }
        f_out << endl;
    }

    f_out.close();
    
    cout << "Wrote " << refs << " references, " << n << " objects, and " << num_q << " queries." << endl << endl;

    delete[]tuple;
    for (int i = 0; i < n; i++)
        delete data[i];
    delete[]data;
    delete[]queries;
    delete[]references;
    return 0;
    */
    
}


tBasicArrayObject **GetQueries(tBasicArrayObject **objs, int size, int numq) {
    tBasicArrayObject **queries = new tBasicArrayObject*[numq];
    int *ids = new int[numq];
    int position, found;
    for (int i = 0; i < numq; i++) {
        // let's select a position that was not previously selected
        do {
            found = 0;
            position = rand()%size;
            // search vector ids from positions zero to i-1
            for (int j = 0; j < i; j++)
                if (position == ids[j])
                    found = 1;
        } while (found);
        ids[i] = position;
 	queries[i] = objs[position];
    }
    return queries;
}

/**
* Find the next pivot.
* Based on the description of Maximum of sum of distances (MSD) available at
* A fast pivot-based indexing algorithm for metric spaces.
* Pattern Recognition Letters 32(11): 1511-1516 (2011)
* http://dx.doi.org/10.1016/j.patrec.2011.04.016
*
* @param embedding_id position of the pivot that will be computed by function (first pivot is position zero)
* @param vantage_objs array that points to the pivots computed so far
* @param numobjects dataset number of objects
* @param objects dataset (array of objects)
* @return pivot object
*/
tBasicArrayObject *FindPivot(int embedding_id, tBasicArrayObject **vantage_objs, int numobjects, tBasicArrayObject **objects) {

    tBasicEuclideanMetricEvaluator *me = new tBasicEuclideanMetricEvaluator();

    tBasicArrayObject *result = NULL;

     // first vantage object
    if (embedding_id == 0) {
        unsigned int position = rand()%numobjects;
        tBasicArrayObject *obj = objects[position];

        // let's find the farthest object from obj, then find the farthest from it and set as the first vantage object
        double tmp, d = 0;
        tBasicArrayObject *tmpobj = NULL;
        for (int i = 0; i < numobjects; i++) {
            tBasicArrayObject *obj_i = objects[i];
            tmp = me->GetDistance(obj, obj_i);
            if (tmp > d) {
                d = tmp;
                tmpobj = obj_i;
                position = i+1;
            }
        }
        result = tmpobj;
    }
    // let's find the other vantage_objs_size-1 objects
    else {
        double tmp, d = 0;
        tBasicArrayObject *tmpobj = NULL;
        // let's find the farthest object from all vantage
        // objects found so far (k objects found so far)
        // new check: it must be different from the objects already in the list
        for (int i = 0; i < numobjects; i++) {
            tBasicArrayObject *obj_i = objects[i];
            int tmp_oid, oid_i = obj_i->GetOID();

            // check if it is not already selected
            bool selected = false;
            for (int j = 0; j < embedding_id; j++) {
                tmp_oid = vantage_objs[j]->GetOID();
                if (tmp_oid == oid_i)
                    selected = true;
            }

            if (!selected) {
                tmp = 0;
                for (int j = 0; j < embedding_id; j++) {
                    tmp += me->GetDistance(vantage_objs[j], obj_i); //  vantage_objs[j]->Distance(obj_i);
                }
                if (tmp > d) {
                    d = tmp;
                    tmpobj = obj_i;
                }
            }
        }
        result = tmpobj;
    }
    return result->Clone();
}

tBasicArrayObject **GetReferences(tBasicArrayObject **objs, int size, int numrefs) {
    tBasicArrayObject **refs = new tBasicArrayObject*[numrefs];
    for (int piv = 0; piv < numrefs; piv++) {
 	refs[piv] = FindPivot(piv, refs, size, objs);
        refs[piv]->SetOID(piv); // global pivot IDs
    }
    return refs;    
}
