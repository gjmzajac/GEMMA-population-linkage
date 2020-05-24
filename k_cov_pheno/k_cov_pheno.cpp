#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "io.h"
#include "mathfunc.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "gsl/gsl_vector.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_blas.h"

#include "gzstream.h"

using namespace std;

//TODO: figure out how to compile this
/* Compilation: 
g++ -Wall -Weffc++ -O3 -std=gnu++11 -DWITH_LAPACK -m64 -static \
	k_cov_pheno.cpp \
	../src/io.o \
	../src/mathfunc.o \
	../src/gzstream.o \
	../src/eigenlib.o \
	../src/lapack.o \
	-I../src \
	-lgsl -lgslcblas -pthread -lz -lsqlite3 -ldl -llapack -lgfortran \
	-latlas /usr/lib/libblas/libblas.a -Wl,--allow-multiple-definition \
	-lquadmath -o k_cov_pheno

/* Usage:
./k_cov_pheno \
	<ids_file> \
	<kin_file> \
	<prop_variance_from_kin> \
	<out_file>
*/


int main(int argc, char *argv[]) {
	for (int i=1; i<argc; i++){
		cout << argv[i] << endl;
	}
	
	map<string, int> mapID2num; //must be made from file containing ids
	
	igzstream idfile (argv[1], igzstream::in);
	
	int n = 0;
	string id;
	while (getline(idfile, id)) {
// cout << id << " " << n << endl;
		mapID2num[id] = n++;
	}
	
	vector<int> indicator_idv (n,1); //n-length vector of 1s

	long int randseed=-1;
	gsl_rng_env_setup();                
	const gsl_rng_type * gslType = gsl_rng_mt19937;                                               
	// if (randseed<0)
	// {
		time_t rawtime;
		time (&rawtime);
		tm * ptm = gmtime (&rawtime);
		
		randseed = (unsigned) (ptm->tm_hour%24*3600+ptm->tm_min*60+ptm->tm_sec);
	// }
	gsl_rng *gsl_r = gsl_rng_alloc(gslType); 
	gsl_rng_set(gsl_r, randseed);

	gsl_vector * x = gsl_vector_alloc (n);
	gsl_vector * y = gsl_vector_alloc (n);

	gsl_matrix * K = gsl_matrix_alloc (n, n);

	bool error = false;
	// ReadFile_king (argv[2], indicator_idv, mapID2num, 0, error, K);
	ReadFile_kin(argv[2], indicator_idv, mapID2num, 1, error, K);

// FILE * out_f = fopen(argv[4], "w");
// gsl_vector_fprintf(out_f, y, "%f");
// cout << endl;

// gsl_matrix_fprintf(out_f, K, "%f");
// cout << endl;

	// CenterMatrix (K);

// gsl_matrix_fprintf(out_f, K, "%f");
// cout << endl;

	ScaleMatrix (K);
	
// gsl_matrix_fprintf(out_f, K, "%f");
// cout << endl;
	
	gsl_linalg_cholesky_decomp(K);

// gsl_matrix_fprintf(out_f, K, "%f");
// cout << endl;

	// get the lower triangle only
	for (int i=0; i<n; i++) {
		for (int j=i+1; j<n; j++) {
			gsl_matrix_set(K, i, j, 0);
		}
	}

	double p = atof(argv[3]);
// gsl_matrix_fprintf(out_f, K, "%f");
// cout << endl;
	for (int i=4; i<argc; i++){
		for (int j=0; j<n; j++){
			gsl_vector_set(y, j, gsl_ran_gaussian(gsl_r, 1));
		}
		
		gsl_vector_memcpy(x, y);
// gsl_vector_fprintf(out_f, y, "%f");
// cout << endl;

		// gsl_blas_dgemv(CblasNoTrans, 0.2, K, x, 0.8, y);
		gsl_blas_dgemv(CblasNoTrans, p, K, x, 1-p, y);
		
		FILE * out_f = fopen(argv[i], "w");
		gsl_vector_fprintf(out_f, y, "%f");
		fclose(out_f);
	}
	gsl_matrix_free(K);
	gsl_vector_free(y);
	// int gsl_vector_fprintf(FILE * stream, const gsl_vector * v, const char * format);

}
// gsl_vector * mu
// gsl_vector * y
// gsl_matrix * K
// gsl_matrix * L //idenity matrix
// int gsl_matrix_scale(K, 0.2)
// int gsl_matrix_scale(L, 0.8)
// int gsl_matrix_add(L, K)
// gsl_linalg_cholesky_decomp1(L)
// int gsl_ran_multivariate_gaussian(const gsl_rng * r, mu, L, y)
