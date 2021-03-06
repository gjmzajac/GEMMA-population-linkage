/*
	Genome-wide Efficient Mixed Model Association (GEMMA)
    Copyright (C) 2011  Xiang Zhou

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <ctime>
#include <cmath>

#include "gsl/gsl_vector.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_blas.h"
#include "gsl/gsl_eigen.h"
#include "gsl/gsl_cdf.h"

#include "lapack.h"  //for functions EigenDecomp

#ifdef FORCE_FLOAT
#include "io_float.h"   //for function ReadFile_kin
#include "gemma_float.h"
#include "vc_float.h"
#include "lm_float.h"  //for LM class
#include "bslmm_float.h"  //for BSLMM class
#include "bslmmdap_float.h"  //for BSLMMDAP class
#include "ldr_float.h"  //for LDR class
#include "lmm_float.h"  //for LMM class, and functions CalcLambda, CalcPve, CalcVgVe
#include "mvlmm_float.h"  //for MVLMM class
#include "prdt_float.h"	//for PRDT class
#include "varcov_float.h"  //for MVLMM class
#include "mathfunc_float.h"	//for a few functions
#else
#include "io.h"
#include "gemma.h"
#include "vc.h"
#include "lm.h"
#include "bslmm.h"
#include "bslmmdap.h"
#include "ldr.h"
#include "lmm.h"
#include "mvlmm.h"
#include "prdt.h"
#include "varcov.h"
#include "mathfunc.h"
#endif


using namespace std;



GEMMA::GEMMA(void):
version("0.96"), date("05/17/2017"), year("2017")
{}

void GEMMA::PrintHeader (void)
{
	cout<<endl;
	cout<<"*********************************************************"<<endl;
	cout<<"  Genome-wide Efficient Mixed Model Association (GEMMA)  "<<endl;
	cout<<"  Version "<<version<<", "<<date<<"                              "<<endl;
	cout<<"  Visit http://www.xzlab.org/software.html For Updates   "<<endl;
	cout<<"  (C) "<<year<<" Xiang Zhou                                   "<<endl;
	cout<<"  GNU General Public License                             "<<endl;
	cout<<"  For Help, Type ./gemma -h                              "<<endl;
	cout<<"*********************************************************"<<endl;
	cout<<endl;

	return;
}


void GEMMA::PrintLicense (void)
{
	cout<<endl;
	cout<<"The Software Is Distributed Under GNU General Public License, But May Also Require The Following Notifications."<<endl;
	cout<<endl;

	cout<<"Including Lapack Routines In The Software May Require The Following Notification:"<<endl;
	cout<<"Copyright (c) 1992-2010 The University of Tennessee and The University of Tennessee Research Foundation.  All rights reserved."<<endl;
	cout<<"Copyright (c) 2000-2010 The University of California Berkeley. All rights reserved."<<endl;
	cout<<"Copyright (c) 2006-2010 The University of Colorado Denver.  All rights reserved."<<endl;
	cout<<endl;

	cout<<"$COPYRIGHT$"<<endl;
	cout<<"Additional copyrights may follow"<<endl;
	cout<<"$HEADER$"<<endl;
	cout<<"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:"<<endl;
	cout<<"- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer."<<endl;
	cout<<"- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer listed in this license in the documentation and/or other materials provided with the distribution."<<endl;
	cout<<"- Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission."<<endl;
	cout<<"The copyright holders provide no reassurances that the source code provided does not infringe any patent, copyright, or any other "
		<<"intellectual property rights of third parties.  The copyright holders disclaim any liability to any recipient for claims brought against "
		<<"recipient by any third party for infringement of that parties intellectual property rights. "<<endl;
	cout<<"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
		<<"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT "
		<<"OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT "
		<<"LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY "
		<<"THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE "
		<<"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."<<endl;
	cout<<endl;



	return;
}



void GEMMA::PrintHelp(size_t option)
{
	if (option==0) {
		cout<<endl;
		cout<<" GEMMA version "<<version<<", released on "<<date<<endl;
		cout<<" implemented by Xiang Zhou"<<endl;
		cout<<endl;
		cout<<" type ./gemma -h [num] for detailed helps"<<endl;
		cout<<" options: " << endl;
		cout<<" 1: quick guide"<<endl;
		cout<<" 2: file I/O related"<<endl;
		cout<<" 3: SNP QC"<<endl;
		cout<<" 4: calculate relatedness matrix"<<endl;
		cout<<" 5: perform eigen decomposition"<<endl;
		cout<<" 6: perform variance component estimation"<<endl;
		cout<<" 7: fit a linear model"<<endl;
		cout<<" 8: fit a linear mixed model"<<endl;
		cout<<" 9: fit a multivariate linear mixed model"<<endl;
		cout<<" 10: fit a Bayesian sparse linear mixed model"<<endl;
		cout<<" 11: obtain predicted values"<<endl;
		cout<<" 12: calculate snp variance covariance"<<endl;
		cout<<" 13: note"<<endl;
		cout<<endl;
	}

	if (option==1) {
		cout<<" QUICK GUIDE" << endl;
		cout<<" to generate a relatedness matrix: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -gk [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -gk [num] -o [prefix]"<<endl;
		cout<<" to generate the S matrix: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -gs -o [prefix]"<<endl;
		cout<<"         ./gemma -p [filename] -g [filename] -gs -o [prefix]"<<endl;
		cout<<"         ./gemma -bfile [prefix] -cat [filename] -gs -o [prefix]"<<endl;
		cout<<"         ./gemma -p [filename] -g [filename] -cat [filename] -gs -o [prefix]"<<endl;
		cout<<"         ./gemma -bfile [prefix] -sample [num] -gs -o [prefix]"<<endl;
		cout<<"         ./gemma -p [filename] -g [filename] -sample [num] -gs -o [prefix]"<<endl;
		cout<<" to generate the q vector: "<<endl;
		cout<<"         ./gemma -beta [filename] -gq -o [prefix]"<<endl;
		cout<<"         ./gemma -beta [filename] -cat [filename] -gq -o [prefix]"<<endl;
		cout<<" to generate the ldsc weigthts: "<<endl;
		cout<<"         ./gemma -beta [filename] -gw -o [prefix]"<<endl;
		cout<<"         ./gemma -beta [filename] -cat [filename] -gw -o [prefix]"<<endl;
		cout<<" to perform eigen decomposition of the relatedness matrix: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -k [filename] -eigen -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -k [filename] -eigen -o [prefix]"<<endl;
		cout<<" to estimate variance components: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -k [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -p [filename] -k [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -bfile [prefix] -mk [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -p [filename] -mk [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -beta [filename] -cor [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -beta [filename] -cor [filename] -cat [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         options for the above two commands: -crt -windowbp [num]"<<endl;
		cout<<"         ./gemma -mq [filename] -ms [filename] -mv [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         or with summary statistics, replace bfile with mbfile, or g or mg; vc=1 for HE weights and vc=2 for LDSC weights"<<endl;
		cout<<"         ./gemma -beta [filename] -bfile [filename] -cat [filename] -wsnp [filename] -wcat [filename] -vc [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -beta [filename] -bfile [filename] -cat [filename] -wsnp [filename] -wcat [filename] -ci [num] -o [prefix]"<<endl;
		cout<<" to fit a linear mixed model: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -a [filename] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<" to fit a linear mixed model to test g by e effects: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -gxe [filename] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -a [filename] -gxe [filename] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<" to fit a univariate linear mixed model with different residual weights for different individuals: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -weight [filename] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -a [filename] -weight [filename] -k [filename] -lmm [num] -o [prefix]"<<endl;
		cout<<" to fit a multivariate linear mixed model: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -k [filename] -lmm [num] -n [num1] [num2] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -a [filename] -k [filename] -lmm [num] -n [num1] [num2] -o [prefix]"<<endl;
		cout<<" to fit a Bayesian sparse linear mixed model: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -bslmm [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -a [filename] -bslmm [num] -o [prefix]"<<endl;
		cout<<" to obtain predicted values: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -epm [filename] -emu [filename] -ebv [filename] -k [filename] -predict [num] -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -epm [filename] -emu [filename] -ebv [filename] -k [filename] -predict [num] -o [prefix]"<<endl;
		cout<<" to calculate correlations between SNPs: "<<endl;
		cout<<"         ./gemma -bfile [prefix] -calccor -o [prefix]"<<endl;
		cout<<"         ./gemma -g [filename] -p [filename] -calccor -o [prefix]"<<endl;
		cout<<endl;
	}

	if (option==2) {
		cout<<" FILE I/O RELATED OPTIONS" << endl;
		cout<<" -bfile    [prefix]       "<<" specify input PLINK binary ped file prefix."<<endl;
		cout<<"          requires: *.fam, *.bim and *.bed files"<<endl;
		cout<<"          missing value: -9"<<endl;
		cout<<" -g        [filename]     "<<" specify input BIMBAM mean genotype file name"<<endl;
		cout<<"          format: rs#1, allele0, allele1, genotype for individual 1, genotype for individual 2, ..."<<endl;
		cout<<"                  rs#2, allele0, allele1, genotype for individual 1, genotype for individual 2, ..."<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -p        [filename]     "<<" specify input BIMBAM phenotype file name"<<endl;
		cout<<"          format: phenotype for individual 1"<<endl;
		cout<<"                  phenotype for individual 2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -a        [filename]     "<<" specify input BIMBAM SNP annotation file name (optional)"<<endl;
		cout<<"          format: rs#1, base_position, chr_number"<<endl;
		cout<<"                  rs#2, base_position, chr_number"<<endl;
		cout<<"                  ..."<<endl;
		// WJA added
		cout<<" -oxford    [prefix]       "<<" specify input Oxford genotype bgen file prefix."<<endl;
		cout<<"          requires: *.bgen, *.sample files"<<endl;

		cout<<" -gxe      [filename]     "<<" specify input file that contains a column of environmental factor for g by e tests"<<endl;
		cout<<"          format: variable for individual 1"<<endl;
		cout<<"                  variable for individual 2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -widv   [filename]     "<<" specify input file that contains a column of residual weights"<<endl;
		cout<<"          format: variable for individual 1"<<endl;
		cout<<"                  variable for individual 2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -k        [filename]     "<<" specify input kinship/relatedness matrix file name"<<endl;
		cout<<" -mk       [filename]     "<<" specify input file which contains a list of kinship/relatedness matrices"<<endl;
		cout<<" -u        [filename]     "<<" specify input file containing the eigen vectors of the kinship/relatedness matrix"<<endl;
		cout<<" -d        [filename]     "<<" specify input file containing the eigen values of the kinship/relatedness matrix"<<endl;
		cout<<" -c        [filename]     "<<" specify input covariates file name (optional)"<<endl;
		cout<<" -cat      [filename]     "<<" specify input category file name (optional), which contains rs cat1 cat2 ..."<<endl;
		cout<<" -beta     [filename]     "<<" specify input beta file name (optional), which contains rs beta se_beta n_total (or n_mis and n_obs) estimates from a lm model"<<endl;
		cout<<" -cor      [filename]     "<<" specify input correlation file name (optional), which contains rs window_size correlations from snps"<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<"          note: the intercept (a column of 1s) may need to be included"<<endl;
		cout<<" -epm      [filename]     "<<" specify input estimated parameter file name"<<endl;
		cout<<" -en [n1] [n2] [n3] [n4]  "<<" specify values for the input estimated parameter file (with a header)"<<endl;
		cout<<"          options: n1: rs column number"<<endl;
		cout<<"                   n2: estimated alpha column number (0 to ignore)"<<endl;
		cout<<"                   n3: estimated beta column number (0 to ignore)"<<endl;
		cout<<"                   n4: estimated gamma column number (0 to ignore)"<<endl;
		cout<<"          default: 2 4 5 6 if -ebv is not specified; 2 0 5 6 if -ebv is specified"<<endl;
		cout<<" -ebv      [filename]     "<<" specify input estimated random effect (breeding value) file name"<<endl;
		cout<<"          format: value for individual 1"<<endl;
		cout<<"                  value for individual 2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -emu      [filename]     "<<" specify input log file name containing estimated mean"<<endl;
		cout<<" -mu       [num]          "<<" specify input estimated mean value"<<endl;
		cout<<" -gene     [filename]     "<<" specify input gene expression file name"<<endl;
		cout<<"          format: header"<<endl;
		cout<<"                  gene1, count for individual 1, count for individual 2, ..."<<endl;
		cout<<"                  gene2, count for individual 1, count for individual 2, ..."<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: not allowed"<<endl;
		cout<<" -r        [filename]     "<<" specify input total read count file name"<<endl;
		cout<<"          format: total read count for individual 1"<<endl;
		cout<<"                  total read count for individual 2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -snps     [filename]     "<<" specify input snps file name to only analyze a certain set of snps"<<endl;
		cout<<"          format: rs#1"<<endl;
		cout<<"                  rs#2"<<endl;
		cout<<"                  ..."<<endl;
		cout<<"          missing value: NA"<<endl;
		cout<<" -silence                 "<<" silent terminal display"<<endl;
		cout<<" -km       [num]          "<<" specify input kinship/relatedness file type (default 1)."<<endl;
		cout<<"          options: 1: \"n by n matrix\" format"<<endl;
		cout<<"                   2: \"id  id  value\" format"<<endl;
		cout<<" -n        [num]          "<<" specify phenotype column in the phenotype/*.fam file (optional; default 1)"<<endl;
		cout<<" -pace     [num]          "<<" specify terminal display update pace (default 100000 SNPs or 100000 iterations)."<<endl;
		cout<<" -outdir   [path]         "<<" specify output directory path (default \"./output/\")"<<endl;
		cout<<" -o        [prefix]       "<<" specify output file prefix (default \"result\")"<<endl;
		cout<<"          output: prefix.cXX.txt or prefix.sXX.txt from kinship/relatedness matrix estimation"<<endl;
		cout<<"          output: prefix.assoc.txt and prefix.log.txt form association tests"<<endl;
		cout<<endl;
	}

	if (option==3) {
		cout<<" SNP QC OPTIONS" << endl;
		cout<<" -miss     [num]          "<<" specify missingness threshold (default 0.05)" << endl;
		cout<<" -maf      [num]          "<<" specify minor allele frequency threshold (default 0.01)" << endl;
		cout<<" -hwe      [num]          "<<" specify HWE test p value threshold (default 0; no test)" << endl;
		cout<<" -r2       [num]          "<<" specify r-squared threshold (default 0.9999)" << endl;
		cout<<" -notsnp                  "<<" minor allele frequency cutoff is not used" << endl;
		cout<<endl;
	}

	if (option==4) {
		cout<<" RELATEDNESS MATRIX CALCULATION OPTIONS" << endl;
		cout<<" -gk       [num]          "<<" specify which type of kinship/relatedness matrix to generate (default 1)" << endl;
		cout<<"          options: 1: centered XX^T/p"<<endl;
		cout<<"                   2: standardized XX^T/p"<<endl;
		cout<<"          note: non-polymorphic SNPs are excluded "<<endl;
		cout<<endl;
	}

	if (option==5) {
		cout<<" EIGEN-DECOMPOSITION OPTIONS" << endl;
		cout<<" -eigen                   "<<" specify to perform eigen decomposition of the loaded relatedness matrix" << endl;
		cout<<endl;
	}

	if (option==6) {
		cout<<" VARIANCE COMPONENT ESTIMATION OPTIONS" << endl;
		cout<<" -vc                      "<<" specify to perform variance component estimation for the loaded relatedness matrix/matrices" << endl;
		cout<<"          options (with kinship file):   1: HE regression (default)"<<endl;
		cout<<"                                         2: REML"<<endl;
		cout<<"          options (with beta/cor files): 1: Centered genotypes (default)"<<endl;
		cout<<"                                         2: Standardized genotypes"<<endl;
		cout<<"                                         -crt -windowbp [num]"<<" specify the window size based on bp (default 1000000; 1Mb)"<<endl;
		cout<<"                                         -crt -windowcm [num]"<<" specify the window size based on cm (default 0)"<<endl;
		cout<<"                                         -crt -windowns [num]"<<" specify the window size based on number of snps (default 0)"<<endl;
		cout<<endl;
	}

	if (option==7) {
		cout<<" LINEAR MODEL OPTIONS" << endl;
		cout<<" -lm       [num]         "<<" specify analysis options (default 1)."<<endl;
		cout<<"          options: 1: Wald test"<<endl;
		cout<<"                   2: Likelihood ratio test"<<endl;
		cout<<"                   3: Score test"<<endl;
		cout<<"                   4: 1-3"<<endl;
		cout<<endl;
	}

	if (option==8) {
		cout<<" LINEAR MIXED MODEL OPTIONS" << endl;
		cout<<" -lmm      [num]         "<<" specify analysis options (default 1)."<<endl;
		cout<<"          options: 1: Wald test"<<endl;
		cout<<"                   2: Likelihood ratio test"<<endl;
		cout<<"                   3: Score test"<<endl;
		cout<<"                   4: 1-3"<<endl;
		cout<<"                   5: Parameter estimation in the null model only"<<endl;
		cout<<" -lmin     [num]          "<<" specify minimal value for lambda (default 1e-5)" << endl;
		cout<<" -lmax     [num]          "<<" specify maximum value for lambda (default 1e+5)" << endl;
		cout<<" -region   [num]          "<<" specify the number of regions used to evaluate lambda (default 10)" << endl;
		cout<<endl;
	}

	if (option==9) {
		cout<<" MULTIVARIATE LINEAR MIXED MODEL OPTIONS" << endl;
		cout<<" -pnr				     "<<" specify the pvalue threshold to use the Newton-Raphson's method (default 0.001)"<<endl;
		cout<<" -emi				     "<<" specify the maximum number of iterations for the PX-EM method in the null (default 10000)"<<endl;
		cout<<" -nri				     "<<" specify the maximum number of iterations for the Newton-Raphson's method in the null (default 100)"<<endl;
		cout<<" -emp				     "<<" specify the precision for the PX-EM method in the null (default 0.0001)"<<endl;
		cout<<" -nrp				     "<<" specify the precision for the Newton-Raphson's method in the null (default 0.0001)"<<endl;
		cout<<" -crt				     "<<" specify to output corrected pvalues for these pvalues that are below the -pnr threshold"<<endl;
		cout<<endl;
	}

	if (option==10) {
		cout<<" MULTI-LOCUS ANALYSIS OPTIONS" << endl;
		cout<<" -bslmm	  [num]			 "<<" specify analysis options (default 1)."<<endl;
		cout<<"          options: 1: BSLMM"<<endl;
		cout<<"                   2: standard ridge regression/GBLUP (no mcmc)"<<endl;
		cout<<"                   3: probit BSLMM (requires 0/1 phenotypes)"<<endl;
		cout<<"                   4: BSLMM with DAP for Hyper Parameter Estimation"<<endl;
		cout<<"                   5: BSLMM with DAP for Fine Mapping"<<endl;

		cout<<" -ldr	  [num]			 "<<" specify analysis options (default 1)."<<endl;
		cout<<"          options: 1: LDR"<<endl;

		cout<<"   MCMC OPTIONS" << endl;
		cout<<"   Prior" << endl;
		cout<<" -hmin     [num]          "<<" specify minimum value for h (default 0)" << endl;
		cout<<" -hmax     [num]          "<<" specify maximum value for h (default 1)" << endl;
		cout<<" -rmin     [num]          "<<" specify minimum value for rho (default 0)" << endl;
		cout<<" -rmax     [num]          "<<" specify maximum value for rho (default 1)" << endl;
		cout<<" -pmin     [num]          "<<" specify minimum value for log10(pi) (default log10(1/p), where p is the number of analyzed SNPs )" << endl;
		cout<<" -pmax     [num]          "<<" specify maximum value for log10(pi) (default log10(1) )" << endl;
		cout<<" -smin     [num]          "<<" specify minimum value for |gamma| (default 0)" << endl;
		cout<<" -smax     [num]          "<<" specify maximum value for |gamma| (default 300)" << endl;

		cout<<"   Proposal" << endl;
		cout<<" -gmean    [num]          "<<" specify the mean for the geometric distribution (default: 2000)" << endl;
		cout<<" -hscale   [num]          "<<" specify the step size scale for the proposal distribution of h (value between 0 and 1, default min(10/sqrt(n),1) )" << endl;
		cout<<" -rscale   [num]          "<<" specify the step size scale for the proposal distribution of rho (value between 0 and 1, default min(10/sqrt(n),1) )" << endl;
		cout<<" -pscale   [num]          "<<" specify the step size scale for the proposal distribution of log10(pi) (value between 0 and 1, default min(5/sqrt(n),1) )" << endl;

		cout<<"   Others" << endl;
		cout<<" -w        [num]          "<<" specify burn-in steps (default 100,000)" << endl;
		cout<<" -s        [num]          "<<" specify sampling steps (default 1,000,000)" << endl;
		cout<<" -rpace    [num]          "<<" specify recording pace, record one state in every [num] steps (default 10)" << endl;
		cout<<" -wpace    [num]          "<<" specify writing pace, write values down in every [num] recorded steps (default 1000)" << endl;
		cout<<" -seed     [num]          "<<" specify random seed (a random seed is generated by default)" << endl;
		cout<<" -mh       [num]          "<<" specify number of MH steps in each iteration (default 10)" << endl;
		cout<<"          requires: 0/1 phenotypes and -bslmm 3 option"<<endl;
		cout<<endl;
	}

	if (option==11) {
		cout<<" PREDICTION OPTIONS" << endl;
		cout<<" -predict  [num]			 "<<" specify prediction options (default 1)."<<endl;
		cout<<"          options: 1: predict for individuals with missing phenotypes"<<endl;
		cout<<"                   2: predict for individuals with missing phenotypes, and convert the predicted values to probability scale. Use only for files fitted with -bslmm 3 option"<<endl;
		cout<<endl;
	}

	if (option==12) {
		cout<<" CALC CORRELATION OPTIONS" << endl;
		cout<<" -calccor       			 "<<endl;
		cout<<" -windowbp       [num]            "<<" specify the window size based on bp (default 1000000; 1Mb)" << endl;
		cout<<" -windowcm       [num]            "<<" specify the window size based on cm (default 0; not used)" << endl;
		cout<<" -windowns       [num]            "<<" specify the window size based on number of snps (default 0; not used)" << endl;
		cout<<endl;
	}

	if (option==13) {
		cout<<" NOTE"<<endl;
		cout<<" 1. Only individuals with non-missing phenotoypes and covariates will be analyzed."<<endl;
		cout<<" 2. Missing genotoypes will be repalced with the mean genotype of that SNP."<<endl;
		cout<<" 3. For lmm analysis, memory should be large enough to hold the relatedness matrix and to perform eigen decomposition."<<endl;
		cout<<" 4. For multivariate lmm analysis, use a large -pnr for each snp will increase computation time dramatically."<<endl;
		cout<<" 5. For bslmm analysis, in addition to 3, memory should be large enough to hold the whole genotype matrix."<<endl;
		cout<<endl;
	}

	return;
}

//options
//gk: 21-22
//gs: 25-26
//gq: 27-28
//eigen: 31-32
//lmm: 1-5
//bslmm: 11-15
//predict: 41-43
//lm: 51
//vc: 61
//ci: 66-67
//calccor: 71
//gw: 72

void GEMMA::Assign(int argc, char ** argv, PARAM &cPar)
{
	string str;

	for(int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-bfile")==0 || strcmp(argv[i], "--bfile")==0 || strcmp(argv[i], "-b")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_bfile=str;
		}
		else if (strcmp(argv[i], "-mbfile")==0 || strcmp(argv[i], "--mbfile")==0 || strcmp(argv[i], "-mb")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mbfile=str;
		}
		else if (strcmp(argv[i], "-silence")==0) {
			cPar.mode_silence=true;
		}
		else if (strcmp(argv[i], "-g")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_geno=str;
		}
		else if (strcmp(argv[i], "-mg")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mgeno=str;
		}
		else if (strcmp(argv[i], "-p")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_pheno=str;
		}
		else if (strcmp(argv[i], "-a")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_anno=str;
		}
		// WJA added
		else if (strcmp(argv[i], "-oxford")==0 || strcmp(argv[i], "--oxford")==0 || strcmp(argv[i], "-x")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_oxford=str;
		}
		else if (strcmp(argv[i], "-gxe")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_gxe=str;
		}
		else if (strcmp(argv[i], "-widv")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_weight=str;
		}
		else if (strcmp(argv[i], "-wsnp")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_wsnp=str;
		}
		else if (strcmp(argv[i], "-wcat")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_wcat=str;
		}
		else if (strcmp(argv[i], "-k")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_kin=str;
		}
		else if (strcmp(argv[i], "-mk")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mk=str;
		}
		else if (strcmp(argv[i], "-u")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_ku=str;
		}
		else if (strcmp(argv[i], "-d")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_kd=str;
		}
		else if (strcmp(argv[i], "-c")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_cvt=str;
		}
		else if (strcmp(argv[i], "-cat")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_cat=str;
		}
		else if (strcmp(argv[i], "-mcat")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mcat=str;
		}
		else if (strcmp(argv[i], "-catc")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_catc=str;
		}
		else if (strcmp(argv[i], "-mcatc")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mcatc=str;
		}
		else if (strcmp(argv[i], "-beta")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_beta=str;
		}
		else if (strcmp(argv[i], "-bf")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_bf=str;
		}
		else if (strcmp(argv[i], "-hyp")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_hyp=str;
		}
		else if (strcmp(argv[i], "-cor")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_cor=str;
		}
		else if (strcmp(argv[i], "-study")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_study=str;
		}
		else if (strcmp(argv[i], "-ref")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_ref=str;
		}
		else if (strcmp(argv[i], "-mstudy")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mstudy=str;
		}
		else if (strcmp(argv[i], "-mref")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_mref=str;
		}
		else if (strcmp(argv[i], "-epm")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_epm=str;
		}
		else if (strcmp(argv[i], "-en")==0) {
			while (argv[i+1] != NULL && argv[i+1][0] != '-') {
				++i;
				str.clear();
				str.assign(argv[i]);
				cPar.est_column.push_back(atoi(str.c_str()));
			}
		}
		else if (strcmp(argv[i], "-ebv")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_ebv=str;
		}
		else if (strcmp(argv[i], "-emu")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_log=str;
		}
		else if (strcmp(argv[i], "-mu")==0) {
			if(argv[i+1] == NULL) {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.pheno_mean=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-gene")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_gene=str;
		}
		else if (strcmp(argv[i], "-r")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_read=str;
		}
		else if (strcmp(argv[i], "-snps")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_snps=str;
		}
		else if (strcmp(argv[i], "-km")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.k_mode=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-n")==0) {
			(cPar.p_column).clear();
			while (argv[i+1] != NULL && argv[i+1][0] != '-') {
				++i;
				str.clear();
				str.assign(argv[i]);
				(cPar.p_column).push_back(atoi(str.c_str()));
			}
		}
		else if (strcmp(argv[i], "-pace")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.d_pace=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-outdir")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.path_out=str;
		}
		else if (strcmp(argv[i], "-o")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_out=str;
		}
		else if (strcmp(argv[i], "-miss")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.miss_level=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-maf")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			if (cPar.maf_level!=-1) {cPar.maf_level=atof(str.c_str());}
		}
		else if (strcmp(argv[i], "-hwe")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.hwe_level=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-r2")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.r2_level=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-notsnp")==0) {
			cPar.maf_level=-1;
		}
		else if (strcmp(argv[i], "-gk")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=21; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=20+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-gs")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=25; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=24+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-gq")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=27; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=26+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-gw")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=72; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=71+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-sample")==0) {
		  if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.ni_subsample=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-eigen")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=31; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=30+atoi(str.c_str());
		}
        else if (strcmp(argv[i], "-calccor")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=71; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=70+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-vc")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=61; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=60+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-ci")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=66; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=65+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-pve")==0) {
		  double s=0;
		  while (argv[i+1] != NULL && (argv[i+1][0] != '-' || !isalpha(argv[i+1][1]) ) ) {
			  ++i;
			  str.clear();
			  str.assign(argv[i]);
			  cPar.v_pve.push_back(atof(str.c_str()));
			  s+=atof(str.c_str());
			}
			if (s==1) {
			  cout<<"summation of pve equals one."<<endl;
			}
		}
		else if (strcmp(argv[i], "-blocks")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.n_block=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-noconstrain")==0) {
			cPar.noconstrain=true;
		}
		else if (strcmp(argv[i], "-lm")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=51; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=50+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-fa")==0 || strcmp(argv[i], "-lmm")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=1; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-lmin")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.l_min=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-lmax")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.l_max=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-region")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.n_region=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-pnr")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.p_nr=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-emi")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.em_iter=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-nri")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.nr_iter=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-emp")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.em_prec=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-nrp")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.nr_prec=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-crt")==0) {
			cPar.crt=1;
		}
		else if (strcmp(argv[i], "-bslmm")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=11; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=10+atoi(str.c_str());
		}
		/*
		else if (strcmp(argv[i], "-ldr")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=14; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=13+atoi(str.c_str());
		}
		*/
		else if (strcmp(argv[i], "-hmin")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.h_min=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-hmax")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.h_max=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-rmin")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.rho_min=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-rmax")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.rho_max=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-pmin")==0) {
			if(argv[i+1] == NULL) {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.logp_min=atof(str.c_str())*log(10.0);
		}
		else if (strcmp(argv[i], "-pmax")==0) {
			if(argv[i+1] == NULL) {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.logp_max=atof(str.c_str())*log(10.0);
		}
		else if (strcmp(argv[i], "-smin")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.s_min=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-smax")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.s_max=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-gmean")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.geo_mean=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-hscale")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.h_scale=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-rscale")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.rho_scale=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-pscale")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.logp_scale=atof(str.c_str())*log(10.0);
		}
		else if (strcmp(argv[i], "-w")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.w_step=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-s")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.s_step=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-rpace")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.r_pace=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-wpace")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.w_pace=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-seed")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.randseed=atol(str.c_str());
		}
		else if (strcmp(argv[i], "-mh")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.n_mh=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-predict")==0) {
			if (cPar.a_mode!=0) {cPar.error=true; cout<<"error! only one of -gk -gs -eigen -vc -lm -lmm -bslmm -predict -calccor options is allowed."<<endl; break;}
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {cPar.a_mode=41; continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.a_mode=40+atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-windowcm")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.window_cm=atof(str.c_str());
		}
		else if (strcmp(argv[i], "-windowbp")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.window_bp=atoi(str.c_str());
		}
		else if (strcmp(argv[i], "-windowns")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.window_ns=atoi(str.c_str());
		}
//GJMZ Added: code to read king files
		else if (strcmp(argv[i], "-king")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_king=str;
		}
		else if (strcmp(argv[i], "-seg")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_seg=str;
		}
		else if (strcmp(argv[i], "-segments")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.file_segments=str;
		}
		else if (strcmp(argv[i], "-max-link-tests")==0) {
			if(argv[i+1] == NULL || argv[i+1][0] == '-') {continue;}
			++i;
			str.clear();
			str.assign(argv[i]);
			cPar.m=stoi(str);
		}
		else if (strcmp(argv[i], "-combine-mats")==0) {
			cPar.combine_mats=true;
		}
		else {cout<<"error! unrecognized option: "<<argv[i]<<endl; cPar.error=true; continue;}
	}

	//change prediction mode to 43, if the epm file is not provided
	if (cPar.a_mode==41 && cPar.file_epm.empty()) {cPar.a_mode=43;}

	return;
}



void GEMMA::BatchRun (PARAM &cPar)
{
	clock_t time_begin, time_start;
	time_begin=clock();

	//Read Files
	cout<<"Reading Files ... "<<endl;
	cPar.ReadFiles();
	if (cPar.error==true) {cout<<"error! fail to read files. "<<endl; return;}
	cPar.CheckData();
	if (cPar.error==true) {cout<<"error! fail to check data. "<<endl; return;}

	//Prediction for bslmm
	if (cPar.a_mode==41 || cPar.a_mode==42) {
		gsl_vector *y_prdt;

		y_prdt=gsl_vector_alloc (cPar.ni_total-cPar.ni_test);

		//set to zero
		gsl_vector_set_zero (y_prdt);

		PRDT cPRDT;
		cPRDT.CopyFromParam(cPar);

		//add breeding value if needed
		if (!cPar.file_kin.empty() && !cPar.file_ebv.empty()) {
			cout<<"Adding Breeding Values ... "<<endl;

			gsl_matrix *G=gsl_matrix_alloc (cPar.ni_total, cPar.ni_total);
			gsl_vector *u_hat=gsl_vector_alloc (cPar.ni_test);

			//read kinship matrix and set u_hat
			vector<int> indicator_all;
			size_t c_bv=0;
			for (size_t i=0; i<cPar.indicator_idv.size(); i++) {
				indicator_all.push_back(1);
				if (cPar.indicator_bv[i]==1) {gsl_vector_set(u_hat, c_bv, cPar.vec_bv[i]); c_bv++;}
			}

			ReadFile_kin (cPar.file_kin, indicator_all, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

			//read u
			cPRDT.AddBV(G, u_hat, y_prdt);

			gsl_matrix_free(G);
			gsl_vector_free(u_hat);
		}

		//add beta
		if (!cPar.file_bfile.empty()) {
			cPRDT.AnalyzePlink (y_prdt);
		}
		else {
			cPRDT.AnalyzeBimbam (y_prdt);
		}

		//add mu
		gsl_vector_add_constant(y_prdt, cPar.pheno_mean);

		//convert y to probability if needed
		if (cPar.a_mode==42) {
			double d;
			for (size_t i=0; i<y_prdt->size; i++) {
				d=gsl_vector_get(y_prdt, i);
				d=gsl_cdf_gaussian_P(d, 1.0);
				gsl_vector_set(y_prdt, i, d);
			}
		}


		cPRDT.CopyToParam(cPar);

		cPRDT.WriteFiles(y_prdt);

		gsl_vector_free(y_prdt);
	}


	//Prediction with kinship matrix only; for one or more phenotypes
	if (cPar.a_mode==43) {
		//first, use individuals with full phenotypes to obtain estimates of Vg and Ve
		gsl_matrix *Y=gsl_matrix_alloc (cPar.ni_test, cPar.n_ph);
		gsl_matrix *W=gsl_matrix_alloc (Y->size1, cPar.n_cvt);
		gsl_matrix *G=gsl_matrix_alloc (Y->size1, Y->size1);
		gsl_matrix *U=gsl_matrix_alloc (Y->size1, Y->size1);
		gsl_matrix *UtW=gsl_matrix_alloc (Y->size1, W->size2);
		gsl_matrix *UtY=gsl_matrix_alloc (Y->size1, Y->size2);
		gsl_vector *eval=gsl_vector_alloc (Y->size1);

		gsl_matrix *Y_full=gsl_matrix_alloc (cPar.ni_cvt, cPar.n_ph);
		gsl_matrix *W_full=gsl_matrix_alloc (Y_full->size1, cPar.n_cvt);
		//set covariates matrix W and phenotype matrix Y
		//an intercept should be included in W,
		cPar.CopyCvtPhen (W, Y, 0);
		cPar.CopyCvtPhen (W_full, Y_full, 1);

		gsl_matrix *Y_hat=gsl_matrix_alloc (Y_full->size1, cPar.n_ph);
		gsl_matrix *G_full=gsl_matrix_alloc (Y_full->size1, Y_full->size1);
		gsl_matrix *H_full=gsl_matrix_alloc (Y_full->size1*Y_hat->size2, Y_full->size1*Y_hat->size2);

		//read relatedness matrix G, and matrix G_full
		ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
		if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}
		ReadFile_kin (cPar.file_kin, cPar.indicator_cvt, cPar.mapID2num, cPar.k_mode, cPar.error, G_full);
		if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

		//center matrix G
		CenterMatrix (G);
		CenterMatrix (G_full);

		//eigen-decomposition and calculate trace_G
		cout<<"Start Eigen-Decomposition..."<<endl;
		time_start=clock();
		cPar.trace_G=EigenDecomp (G, U, eval, 0);
		cPar.trace_G=0.0;
		for (size_t i=0; i<eval->size; i++) {
			if (gsl_vector_get (eval, i)<1e-10) {gsl_vector_set (eval, i, 0);}
			cPar.trace_G+=gsl_vector_get (eval, i);
		}
		cPar.trace_G/=(double)eval->size;
		cPar.time_eigen=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

		//calculate UtW and Uty
		CalcUtX (U, W, UtW);
		CalcUtX (U, Y, UtY);

		//calculate variance component and beta estimates
		//and then obtain predicted values
		if (cPar.n_ph==1) {
			gsl_vector *beta=gsl_vector_alloc (W->size2);
			gsl_vector *se_beta=gsl_vector_alloc (W->size2);

			double lambda, logl, vg, ve;
			gsl_vector_view UtY_col=gsl_matrix_column (UtY, 0);

			//obtain estimates
			CalcLambda ('R', eval, UtW, &UtY_col.vector, cPar.l_min, cPar.l_max, cPar.n_region, lambda, logl);
			CalcLmmVgVeBeta (eval, UtW, &UtY_col.vector, lambda, vg, ve, beta, se_beta);

			cout<<"REMLE estimate for vg in the null model = "<<vg<<endl;
			cout<<"REMLE estimate for ve in the null model = "<<ve<<endl;
			cPar.vg_remle_null=vg; cPar.ve_remle_null=ve;

			//obtain Y_hat from fixed effects
			gsl_vector_view Yhat_col=gsl_matrix_column (Y_hat, 0);
			gsl_blas_dgemv (CblasNoTrans, 1.0, W_full, beta, 0.0, &Yhat_col.vector);

			//obtain H
			gsl_matrix_set_identity (H_full);
			gsl_matrix_scale (H_full, ve);
			gsl_matrix_scale (G_full, vg);
			gsl_matrix_add (H_full, G_full);

			//free matrices
			gsl_vector_free(beta);
			gsl_vector_free(se_beta);
		} else {
			gsl_matrix *Vg=gsl_matrix_alloc (cPar.n_ph, cPar.n_ph);
			gsl_matrix *Ve=gsl_matrix_alloc (cPar.n_ph, cPar.n_ph);
			gsl_matrix *B=gsl_matrix_alloc (cPar.n_ph, W->size2);
			gsl_matrix *se_B=gsl_matrix_alloc (cPar.n_ph, W->size2);

			//obtain estimates
			CalcMvLmmVgVeBeta (eval, UtW, UtY, cPar.em_iter, cPar.nr_iter, cPar.em_prec, cPar.nr_prec, cPar.l_min, cPar.l_max, cPar.n_region, Vg, Ve, B, se_B);

			cout<<"REMLE estimate for Vg in the null model: "<<endl;
			for (size_t i=0; i<Vg->size1; i++) {
				for (size_t j=0; j<=i; j++) {
					cout<<gsl_matrix_get(Vg, i, j)<<"\t";
				}
				cout<<endl;
			}
			cout<<"REMLE estimate for Ve in the null model: "<<endl;
			for (size_t i=0; i<Ve->size1; i++) {
				for (size_t j=0; j<=i; j++) {
					cout<<gsl_matrix_get(Ve, i, j)<<"\t";
				}
				cout<<endl;
			}
			cPar.Vg_remle_null.clear();
			cPar.Ve_remle_null.clear();
			for (size_t i=0; i<Vg->size1; i++) {
				for (size_t j=i; j<Vg->size2; j++) {
					cPar.Vg_remle_null.push_back(gsl_matrix_get (Vg, i, j) );
					cPar.Ve_remle_null.push_back(gsl_matrix_get (Ve, i, j) );
				}
			}

			//obtain Y_hat from fixed effects
			gsl_blas_dgemm (CblasNoTrans, CblasTrans, 1.0, W_full, B, 0.0, Y_hat);

			//obtain H
			KroneckerSym(G_full, Vg, H_full);
			for (size_t i=0; i<G_full->size1; i++) {
				gsl_matrix_view H_sub=gsl_matrix_submatrix (H_full, i*Ve->size1, i*Ve->size2, Ve->size1, Ve->size2);
				gsl_matrix_add (&H_sub.matrix, Ve);
			}

			//free matrices
			gsl_matrix_free (Vg);
			gsl_matrix_free (Ve);
			gsl_matrix_free (B);
			gsl_matrix_free (se_B);
		}

		PRDT cPRDT;

		cPRDT.CopyFromParam(cPar);

		cout<<"Predicting Missing Phentypes ... "<<endl;
		time_start=clock();
		cPRDT.MvnormPrdt(Y_hat, H_full, Y_full);
		cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

		cPRDT.WriteFiles(Y_full);

		gsl_matrix_free(Y);
		gsl_matrix_free(W);
		gsl_matrix_free(G);
		gsl_matrix_free(U);
		gsl_matrix_free(UtW);
		gsl_matrix_free(UtY);
		gsl_vector_free(eval);

		gsl_matrix_free(Y_full);
		gsl_matrix_free(Y_hat);
		gsl_matrix_free(W_full);
		gsl_matrix_free(G_full);
		gsl_matrix_free(H_full);
	}


	//Generate Kinship matrix
	if (cPar.a_mode==21 || cPar.a_mode==22) {
		cout<<"Calculating Relatedness Matrix ... "<<endl;

		gsl_matrix *G=gsl_matrix_alloc (cPar.ni_total, cPar.ni_total);

		time_start=clock();
		cPar.CalcKin (G);
		cPar.time_G=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		if (cPar.error==true) {cout<<"error! fail to calculate relatedness matrix. "<<endl; return;}

		if (cPar.a_mode==21) {
			cPar.WriteMatrix (G, "cXX");
		} else {
			cPar.WriteMatrix (G, "sXX");
		}

		gsl_matrix_free (G);
	}

	//Compute the LDSC weights (not implemented yet)
	if (cPar.a_mode==72) {
		cout<<"Calculating Weights ... "<<endl;

		VARCOV cVarcov;
		cVarcov.CopyFromParam(cPar);

		if (!cPar.file_bfile.empty()) {
		  cVarcov.AnalyzePlink ();
		} else {
		  cVarcov.AnalyzeBimbam ();
		}

		cVarcov.CopyToParam(cPar);
	}


	//Compute the S matrix (and its variance), that is used for variance component estimation using summary statistics
	if (cPar.a_mode==25 || cPar.a_mode==26) {
	  cout<<"Calculating the S Matrix ... "<<endl;

	  gsl_matrix *S=gsl_matrix_alloc (cPar.n_vc*2, cPar.n_vc);
	  gsl_vector *ns=gsl_vector_alloc (cPar.n_vc+1);
	  gsl_matrix_set_zero(S);
	  gsl_vector_set_zero(ns);

	  gsl_matrix_view S_mat=gsl_matrix_submatrix(S, 0, 0, cPar.n_vc, cPar.n_vc);
	  gsl_matrix_view Svar_mat=gsl_matrix_submatrix (S, cPar.n_vc, 0, cPar.n_vc, cPar.n_vc);
	  gsl_vector_view ns_vec=gsl_vector_subvector(ns, 0, cPar.n_vc);

	  gsl_matrix *K=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc*cPar.ni_test);
	  gsl_matrix *A=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc*cPar.ni_test);
	  gsl_matrix_set_zero (K);
	  gsl_matrix_set_zero (A);

	  gsl_vector *y=gsl_vector_alloc (cPar.ni_test);
	  gsl_matrix *W=gsl_matrix_alloc (cPar.ni_test, cPar.n_cvt);

	  cPar.CopyCvtPhen (W, y, 0);

	  set<string> setSnps_beta;
	  map <string, double> mapRS2wA, mapRS2wK;

	  cPar.ObtainWeight(setSnps_beta, mapRS2wK);

	  time_start=clock();
	  cPar.CalcS (mapRS2wA, mapRS2wK, W, A, K, &S_mat.matrix, &Svar_mat.matrix, &ns_vec.vector);
	  cPar.time_G=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	  if (cPar.error==true) {cout<<"error! fail to calculate the S matrix. "<<endl; return;}

	  gsl_vector_set (ns, cPar.n_vc, cPar.ni_test);

	  cPar.WriteMatrix (S, "S");
	  cPar.WriteVector (ns, "size");
	  cPar.WriteVar ("snps");
	  /*
	  cout<<scientific;
	  for (size_t i=0; i<cPar.n_vc; i++) {
            for (size_t j=0; j<cPar.n_vc; j++) {
	      cout<<gsl_matrix_get(S, i, j)<<" ";
            }
            cout<<endl;
	  }

	  for (size_t i=cPar.n_vc; i<cPar.n_vc*2; i++) {
            for (size_t j=0; j<cPar.n_vc; j++) {
	      cout<<gsl_matrix_get(S, i, j)<<" ";
            }
            cout<<endl;
	  }
	  */
	  gsl_matrix_free (S);
	  gsl_vector_free (ns);

	  gsl_matrix_free (A);
	  gsl_matrix_free (K);

	  gsl_vector_free (y);
	  gsl_matrix_free (K);
	}

	//Compute the q vector, that is used for variance component estimation using summary statistics
	if (cPar.a_mode==27 || cPar.a_mode==28) {
	  gsl_matrix *Vq=gsl_matrix_alloc (cPar.n_vc, cPar.n_vc);
	  gsl_vector *q=gsl_vector_alloc (cPar.n_vc);
	  gsl_vector *s=gsl_vector_alloc (cPar.n_vc+1);
	  gsl_vector_set_zero (q);
	  gsl_vector_set_zero (s);

	  gsl_vector_view s_vec=gsl_vector_subvector(s, 0, cPar.n_vc);

	  vector<size_t> vec_cat, vec_ni;
	  vector<double> vec_weight, vec_z2;
	  map<string, double> mapRS2weight;
	  mapRS2weight.clear();

	  time_start=clock();
	  ReadFile_beta (cPar.file_beta, cPar.mapRS2cat, mapRS2weight, vec_cat, vec_ni, vec_weight, vec_z2, cPar.ni_total, cPar.ns_total, cPar.ns_test);
	  cout<<"## number of total individuals = "<<cPar.ni_total<<endl;
	  cout<<"## number of total SNPs = "<<cPar.ns_total<<endl;
	  cout<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  cout<<"## number of variance components = "<<cPar.n_vc<<endl;
	  cout<<"Calculating the q vector ... "<<endl;
	  Calcq (cPar.n_block, vec_cat, vec_ni, vec_weight, vec_z2, Vq, q, &s_vec.vector);
	  cPar.time_G=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

	  if (cPar.error==true) {cout<<"error! fail to calculate the q vector. "<<endl; return;}

	  gsl_vector_set (s, cPar.n_vc, cPar.ni_total);

	  cPar.WriteMatrix (Vq, "Vq");
	  cPar.WriteVector (q, "q");
	  cPar.WriteVector (s, "size");
	  /*
	  for (size_t i=0; i<cPar.n_vc; i++) {
	    cout<<gsl_vector_get(q, i)<<endl;
	  }
	  */
	  gsl_matrix_free (Vq);
	  gsl_vector_free (q);
	  gsl_vector_free (s);
	}


    //Calculate SNP covariance
	if (cPar.a_mode==71) {
	  VARCOV cVarcov;
	  cVarcov.CopyFromParam(cPar);

	  if (!cPar.file_bfile.empty()) {
            cVarcov.AnalyzePlink ();
	  } else {
            cVarcov.AnalyzeBimbam ();
	  }

	  cVarcov.CopyToParam(cPar);
	}


	//LM
	if (cPar.a_mode==51 || cPar.a_mode==52 || cPar.a_mode==53 || cPar.a_mode==54) {  //Fit LM
		gsl_matrix *Y=gsl_matrix_alloc (cPar.ni_test, cPar.n_ph);
		gsl_matrix *W=gsl_matrix_alloc (Y->size1, cPar.n_cvt);

		//set covariates matrix W and phenotype matrix Y
		//an intercept should be included in W,
		cPar.CopyCvtPhen (W, Y, 0);

		//Fit LM or mvLM
		if (cPar.n_ph==1) {
			LM cLm;
			cLm.CopyFromParam(cPar);

			gsl_vector_view Y_col=gsl_matrix_column (Y, 0);

			if (!cPar.file_gene.empty()) {
				cLm.AnalyzeGene (W, &Y_col.vector); //y is the predictor, not the phenotype
			} else if (!cPar.file_bfile.empty()) {
				cLm.AnalyzePlink (W, &Y_col.vector);
			} else if (!cPar.file_oxford.empty()) {
				cLm.Analyzebgen (W, &Y_col.vector);
			} else {
				cLm.AnalyzeBimbam (W, &Y_col.vector);
			}

			cLm.WriteFiles();
			cLm.CopyToParam(cPar);
		}
		/*
		else {
			MVLM cMvlm;
			cMvlm.CopyFromParam(cPar);

			if (!cPar.file_bfile.empty()) {
				cMvlm.AnalyzePlink (W, Y);
			} else {
				cMvlm.AnalyzeBimbam (W, Y);
			}

			cMvlm.WriteFiles();
			cMvlm.CopyToParam(cPar);
		}
		*/
		//release all matrices and vectors
		gsl_matrix_free (Y);
		gsl_matrix_free (W);
	}


	//VC estimation with one or multiple kinship matrices
	//REML approach only
	//if file_kin or file_ku/kd is provided, then a_mode is changed to 5 already, in param.cpp
	//for one phenotype only;
	if (cPar.a_mode==61 || cPar.a_mode==62 || cPar.a_mode==63) {
	  if (!cPar.file_beta.empty() ) {
	    //need to obtain a common set of SNPs between beta file and the genotype file; these are saved in mapRS2wA and mapRS2wK
	    //normalize the weight in mapRS2wK to have an average of one; each element of mapRS2wA is 1
	    //update indicator_snps, so that the numbers are in accordance with mapRS2wK
	    set<string> setSnps_beta;
	    ReadFile_snps_header (cPar.file_beta, setSnps_beta);

	    map <string, double> mapRS2wA, mapRS2wK;
	    cPar.ObtainWeight(setSnps_beta, mapRS2wK);

	    cPar.UpdateSNP (mapRS2wK);

	    //setup matrices and vectors
	    gsl_matrix *S=gsl_matrix_alloc (cPar.n_vc*2, cPar.n_vc);
	    gsl_matrix *Vq=gsl_matrix_alloc (cPar.n_vc, cPar.n_vc);
	    gsl_vector *q=gsl_vector_alloc (cPar.n_vc);
	    gsl_vector *s=gsl_vector_alloc (cPar.n_vc+1);

	    gsl_matrix *K=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc*cPar.ni_test);
	    gsl_matrix *A=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc*cPar.ni_test);

	    gsl_vector *y=gsl_vector_alloc (cPar.ni_test);
	    gsl_matrix *W=gsl_matrix_alloc (cPar.ni_test, cPar.n_cvt);

	    gsl_matrix_set_zero (K);
	    gsl_matrix_set_zero (A);

	    gsl_matrix_set_zero(S);
	    gsl_matrix_set_zero(Vq);
	    gsl_vector_set_zero (q);
	    gsl_vector_set_zero (s);

	    cPar.CopyCvtPhen (W, y, 0);

	    gsl_matrix_view S_mat=gsl_matrix_submatrix(S, 0, 0, cPar.n_vc, cPar.n_vc);
	    gsl_matrix_view Svar_mat=gsl_matrix_submatrix (S, cPar.n_vc, 0, cPar.n_vc, cPar.n_vc);
	    gsl_vector_view s_vec=gsl_vector_subvector(s, 0, cPar.n_vc);

	    vector<size_t> vec_cat, vec_ni;
	    vector<double> vec_weight, vec_z2;

	    //read beta, based on the mapRS2wK
	    ReadFile_beta (cPar.file_beta, cPar.mapRS2cat, mapRS2wK, vec_cat, vec_ni, vec_weight, vec_z2, cPar.ni_study, cPar.ns_study, cPar.ns_test);

	    cout<<"Study Panel: "<<endl;
	    cout<<"## number of total individuals = "<<cPar.ni_study<<endl;
	    cout<<"## number of total SNPs = "<<cPar.ns_study<<endl;
	    cout<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	    cout<<"## number of variance components = "<<cPar.n_vc<<endl;

	    //compute q
	    Calcq (cPar.n_block, vec_cat, vec_ni, vec_weight, vec_z2, Vq, q, &s_vec.vector);

	    //compute S
	    time_start=clock();
	    cPar.CalcS (mapRS2wA, mapRS2wK, W, A, K, &S_mat.matrix, &Svar_mat.matrix, &s_vec.vector);
	    cPar.time_G+=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	    if (cPar.error==true) {cout<<"error! fail to calculate the S matrix. "<<endl; return;}

	    //compute vc estimates
	    CalcVCss(Vq, &S_mat.matrix, &Svar_mat.matrix, q, &s_vec.vector, cPar.ni_study, cPar.v_pve, cPar.v_se_pve, cPar.pve_total, cPar.se_pve_total, cPar.v_sigma2, cPar.v_se_sigma2, cPar.v_enrich, cPar.v_se_enrich);

	    //if LDSC weights, then compute the weights and run the above steps again
	    if (cPar.a_mode==62) {
	      //compute the weights and normalize the weights for A
	      cPar.UpdateWeight (1, mapRS2wK, cPar.ni_study, &s_vec.vector, mapRS2wA);

	      //read beta file again, and update weigths vector
	      ReadFile_beta (cPar.file_beta, cPar.mapRS2cat, mapRS2wA, vec_cat, vec_ni, vec_weight, vec_z2, cPar.ni_study, cPar.ns_total, cPar.ns_test);

	      //compute q
	      Calcq (cPar.n_block, vec_cat, vec_ni, vec_weight, vec_z2, Vq, q, &s_vec.vector);

	      //compute S
	      time_start=clock();
	      cPar.CalcS (mapRS2wA, mapRS2wK, W, A, K, &S_mat.matrix, &Svar_mat.matrix, &s_vec.vector);
	      cPar.time_G+=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	      if (cPar.error==true) {cout<<"error! fail to calculate the S matrix. "<<endl; return;}

	      //compute vc estimates
	      CalcVCss(Vq, &S_mat.matrix, &Svar_mat.matrix, q, &s_vec.vector, cPar.ni_study, cPar.v_pve, cPar.v_se_pve, cPar.pve_total, cPar.se_pve_total, cPar.v_sigma2, cPar.v_se_sigma2, cPar.v_enrich, cPar.v_se_enrich);
	    }

	    gsl_vector_set (s, cPar.n_vc, cPar.ni_test);

	    cPar.WriteMatrix (S, "S");
	    cPar.WriteMatrix (Vq, "Vq");
	    cPar.WriteVector (q, "q");
	    cPar.WriteVector (s, "size");

	    gsl_matrix_free (S);
	    gsl_matrix_free (Vq);
	    gsl_vector_free (q);
	    gsl_vector_free (s);

	    gsl_matrix_free (A);
	    gsl_matrix_free (K);
	    gsl_vector_free (y);
	    gsl_matrix_free (W);
	  } else if (!cPar.file_study.empty() || !cPar.file_mstudy.empty()) {
	    if (!cPar.file_study.empty()) {
	      string sfile=cPar.file_study+".size.txt";
	      CountFileLines (sfile, cPar.n_vc);
	    } else {
	      string file_name;
	      igzstream infile (cPar.file_mstudy.c_str(), igzstream::in);
	      if (!infile) {cout<<"error! fail to open mstudy file: "<<cPar.file_study<<endl; return;}

	      safeGetline(infile, file_name);

	      infile.clear();
	      infile.close();

	      string sfile=file_name+".size.txt";
	      CountFileLines (sfile, cPar.n_vc);
	    }

	    cPar.n_vc=cPar.n_vc-1;

	    gsl_matrix *S=gsl_matrix_alloc (2*cPar.n_vc, cPar.n_vc);
	    gsl_matrix *Vq=gsl_matrix_alloc (cPar.n_vc, cPar.n_vc);
	    //gsl_matrix *V=gsl_matrix_alloc (cPar.n_vc+1, (cPar.n_vc*(cPar.n_vc+1))/2*(cPar.n_vc+1) );
	    //gsl_matrix *Vslope=gsl_matrix_alloc (n_lines+1, (n_lines*(n_lines+1))/2*(n_lines+1) );
	    gsl_vector *q=gsl_vector_alloc (cPar.n_vc);
	    gsl_vector *s_study=gsl_vector_alloc (cPar.n_vc);
	    gsl_vector *s_ref=gsl_vector_alloc (cPar.n_vc);
	    gsl_vector *s=gsl_vector_alloc (cPar.n_vc+1);

	    gsl_matrix_set_zero(S);
	    gsl_matrix_view S_mat=gsl_matrix_submatrix(S, 0, 0, cPar.n_vc, cPar.n_vc);
	    gsl_matrix_view Svar_mat=gsl_matrix_submatrix (S, cPar.n_vc, 0, cPar.n_vc, cPar.n_vc);

	    gsl_matrix_set_zero(Vq);
	    //gsl_matrix_set_zero(V);
	    //gsl_matrix_set_zero(Vslope);
	    gsl_vector_set_zero(q);
	    gsl_vector_set_zero(s_study);
	    gsl_vector_set_zero(s_ref);

	    if (!cPar.file_study.empty()) {
	      ReadFile_study(cPar.file_study, Vq, q, s_study, cPar.ni_study);
	    } else {
	      ReadFile_mstudy(cPar.file_mstudy, Vq, q, s_study, cPar.ni_study);
	    }

	    if (!cPar.file_ref.empty()) {
	      ReadFile_ref(cPar.file_ref, &S_mat.matrix, &Svar_mat.matrix, s_ref, cPar.ni_ref);
	    } else {
	      ReadFile_mref(cPar.file_mref, &S_mat.matrix, &Svar_mat.matrix, s_ref, cPar.ni_ref);
	    }

	    cout<<"## number of variance components = "<<cPar.n_vc<<endl;
	    cout<<"## number of individuals in the sample = "<<cPar.ni_study<<endl;
	    cout<<"## number of individuals in the reference = "<<cPar.ni_ref<<endl;

	    CalcVCss(Vq, &S_mat.matrix, &Svar_mat.matrix, q, s_study, cPar.ni_study, cPar.v_pve, cPar.v_se_pve, cPar.pve_total, cPar.se_pve_total, cPar.v_sigma2, cPar.v_se_sigma2, cPar.v_enrich, cPar.v_se_enrich);

	    gsl_vector_view s_sub=gsl_vector_subvector (s, 0, cPar.n_vc);
	    gsl_vector_memcpy (&s_sub.vector, s_ref);
	    gsl_vector_set (s, cPar.n_vc, cPar.ni_ref);

	    cPar.WriteMatrix (S, "S");
	    cPar.WriteMatrix (Vq, "Vq");
	    cPar.WriteVector (q, "q");
	    cPar.WriteVector (s, "size");

	    gsl_matrix_free (S);
	    gsl_matrix_free (Vq);
	    //gsl_matrix_free (V);
	    //gsl_matrix_free (Vslope);
	    gsl_vector_free (q);
	    gsl_vector_free (s_study);
	    gsl_vector_free (s_ref);
	    gsl_vector_free (s);
	  } else {
		gsl_matrix *Y=gsl_matrix_alloc (cPar.ni_test, cPar.n_ph);
		gsl_matrix *W=gsl_matrix_alloc (Y->size1, cPar.n_cvt);
		gsl_matrix *G=gsl_matrix_alloc (Y->size1, Y->size1*cPar.n_vc );

		//GJMZ Added: map to hold all IBD segments
		// map <int, map <double,Segments> > chrpos2segs;
		// map <int, map <double, unordered_map <int, unordered_map <int, double> > > > chrpos2segs;
		// map <int, map <long int, unordered_map <int, unordered_map <int, double> > > > chrpos2segs;
// 		map <int, map <long int, map <int, map <int, double> > > > chrpos2segs;
		map <int, map <int, FILE *> > chrpos2segs;
		// map <int, map <long int, sqlite3_stmt*> > chrpos2segs;
		
		//declare traceG_new vector
		// vector<double> traceG_new(2,0);
		vector<double> traceG_new(cPar.n_vc,0);

		//declare db handle
		// sqlite3 *db;
		string segs_f_pref = cPar.path_out+"/"+cPar.file_out+".segs.";

		//set covariates matrix W and phenotype matrix Y
		//an intercept should be included in W,
		cPar.CopyCvtPhen (W, Y, 0);

		//GJMZ Added: code to read king segments with multiple kinship files
		if (!(cPar.file_mk).empty() && !(cPar.file_segments).empty()) {
		  ReadFile_mk (cPar.file_mk, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
		  if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

		  //center matrix G, and obtain v_traceG
		  double d=0;
		  (cPar.v_traceG).clear();
		  gsl_matrix_view G_sub;
		  for (size_t i=0; i<cPar.n_vc-1; i++) {
		    // gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
		    G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
		    CenterMatrix (&G_sub.matrix);
		    d=0;
		    for (size_t j=0; j<G->size1; j++) {
		      d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    }
		    d/=(double)G->size1;
		    (cPar.v_traceG).push_back(d);
			d = ScaleMatrix (&G_sub.matrix);
			traceG_new[i] = d;
		  }
		  
		  // submatrix for segments vc
		  size_t i=cPar.n_vc-1;
			G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			
			ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs, segs_f_pref);
			if (cPar.error==true) {cout<<"error! fail to read KING segments file. "<<endl; return;}
					  //center matrix G, and obtain v_traceG
			(cPar.v_traceG).push_back(0);

		}
		//read kinship matrices
		else if (!(cPar.file_mk).empty()) {
		  ReadFile_mk (cPar.file_mk, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
		  if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

		  //center matrix G, and obtain v_traceG
		  double d=0;
		  (cPar.v_traceG).clear();
		  for (size_t i=0; i<cPar.n_vc; i++) {
		    gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
		    CenterMatrix (&G_sub.matrix);
		    d=0;
		    for (size_t j=0; j<G->size1; j++) {
		      d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    }
		    d/=(double)G->size1;
		    (cPar.v_traceG).push_back(d);
		  }
		}
		  //GJMZ Added: code to read king segments with general kin
		  else if (!(cPar.file_kin).empty() && !(cPar.file_segments).empty()){
			// first submatrix for first vc
			size_t i=0;
		    gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);

			ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, &G_sub.matrix);
			if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

			// center matrix G
					  //center matrix G, and obtain v_traceG
		  double d=0;
		  (cPar.v_traceG).clear();
		    CenterMatrix (&G_sub.matrix);
			
		    d=0;
		    for (size_t j=0; j<G->size1; j++) {
		      d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    }
		    d/=(double)G->size1;
		    (cPar.v_traceG).push_back(d);
		  // }
			d = ScaleMatrix (&G_sub.matrix);
			traceG_new[0] = d;

			// second submatrix for second vc
		  i=1;
			G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			
			ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs, segs_f_pref);
			if (cPar.error==true) {cout<<"error! fail to read KING segments file. "<<endl; return;}
					  //center matrix G, and obtain v_traceG
			(cPar.v_traceG).push_back(0);
		} else if (!(cPar.file_kin).empty()) {
			ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

			//center matrix G
			CenterMatrix (G);

			(cPar.v_traceG).clear();
			double d=0;
			for (size_t j=0; j<G->size1; j++) {
			  d+=gsl_matrix_get (G, j, j);
			}
			d/=(double)G->size1;
			(cPar.v_traceG).push_back(d);
		}
		//GJMZ Added: code to read king files
		else if (!(cPar.file_king).empty() && !(cPar.file_segments).empty()){
			// first submatrix for first vc
			size_t i=0;
		    gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);

			ReadFile_king (cPar.file_king, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, &G_sub.matrix);
			if (cPar.error==true) {cout<<"error! fail to read KING kin file. "<<endl; return;}

			// center matrix G
			// CenterMatrix (G);

			// (cPar.v_traceG).clear();
			// double d=0;
			// for (size_t j=0; j<G->size1; j++) {
			  // d+=gsl_matrix_get (G, j, j);
			// }
			// d/=(double)G->size1;
			// (cPar.v_traceG).push_back(d);
					  //center matrix G, and obtain v_traceG
		  double d=0;
		  (cPar.v_traceG).clear();
		  // for (size_t i=0; i<cPar.n_vc; i++) {
		    // gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
		    CenterMatrix (&G_sub.matrix);
			
		    d=0;
		    for (size_t j=0; j<G->size1; j++) {
		      d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    }
		    d/=(double)G->size1;
		    (cPar.v_traceG).push_back(d);
		  // }
			d = ScaleMatrix (&G_sub.matrix);
			traceG_new[0] = d;

			// second submatrix for second vc
		  // size_t i=1;
		  i=1;
		    // gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			
			//open db connection
			// string file_db_str;
			// file_db_str=cPar.path_out+"/"+cPar.file_out;
			// file_db_str+=".segments.db";

			// if( sqlite3_open(file_db_str.c_str(), &db) ){
				// cerr << "Can't open database: " << sqlite3_errmsg(db) <<endl;
				// sqlite3_close(db);
				// return;
			// }
			
			// ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs, db);
			// ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs);
			ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs, segs_f_pref);
			if (cPar.error==true) {cout<<"error! fail to read KING segments file. "<<endl; return;}
					  //center matrix G, and obtain v_traceG
		  // double d=0;
		  // (cPar.v_traceG).clear();
		  // for (size_t i=0; i<cPar.n_vc; i++) {
		    // gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
		    // CenterMatrix (&G_sub.matrix);
		    // d=0;
		    // for (size_t j=0; j<G->size1; j++) {
		      // d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    // }
		    // d/=(double)G->size1;
		    // (cPar.v_traceG).push_back(d);
		  // }
			(cPar.v_traceG).push_back(0);
		}
		else if (!(cPar.file_seg).empty() && !(cPar.file_segments).empty()){
			// first submatrix for first vc
			size_t i=0;
		    gsl_matrix_view G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);

			ReadFile_seg (cPar.file_seg, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, &G_sub.matrix);
			if (cPar.error==true) {cout<<"error! fail to read KING seg file. "<<endl; return;}

			// center matrix G
					  //center matrix G, and obtain v_traceG
		  double d=0;
		  (cPar.v_traceG).clear();
		    CenterMatrix (&G_sub.matrix);
			
		    d=0;
		    for (size_t j=0; j<G->size1; j++) {
		      d+=gsl_matrix_get (&G_sub.matrix, j, j);
		    }
		    d/=(double)G->size1;
		    (cPar.v_traceG).push_back(d);
		  // }
			d = ScaleMatrix (&G_sub.matrix);
			traceG_new[0] = d;

			// second submatrix for second vc
		  i=1;
			G_sub=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			
			ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, &G_sub.matrix, chrpos2segs, segs_f_pref);
			if (cPar.error==true) {cout<<"error! fail to read KING segments file. "<<endl; return;}
					  //center matrix G, and obtain v_traceG
			(cPar.v_traceG).push_back(0);
		}
		else if (!(cPar.file_king).empty()) {
			ReadFile_king (cPar.file_king, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read KING kin file. "<<endl; return;}

			// center matrix G
			CenterMatrix (G);

			(cPar.v_traceG).clear();
			double d=0;
			for (size_t j=0; j<G->size1; j++) {
			  d+=gsl_matrix_get (G, j, j);
			}
			d/=(double)G->size1;
			(cPar.v_traceG).push_back(d);
		} 
		else if (!(cPar.file_seg).empty()) {
			ReadFile_seg (cPar.file_seg, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read KING seg file. "<<endl; return;}

			// center matrix G
			CenterMatrix (G);

			(cPar.v_traceG).clear();
			double d=0;
			for (size_t j=0; j<G->size1; j++) {
			  d+=gsl_matrix_get (G, j, j);
			}
			d/=(double)G->size1;
			(cPar.v_traceG).push_back(d);
		} 
 		else if (!(cPar.file_segments).empty()) {
			// ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, G, chrpos2segs, db);
			// ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, G, chrpos2segs);
			ReadFile_segments (cPar.file_segments, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.snpInfo, cPar.error, G, chrpos2segs, segs_f_pref);
			if (cPar.error==true) {cout<<"error! fail to read KING segments file. "<<endl; return;}
		} 
			/*
			//eigen-decomposition and calculate trace_G
			cout<<"Start Eigen-Decomposition..."<<endl;
			time_start=clock();

			if (cPar.a_mode==31) {
				cPar.trace_G=EigenDecomp (G, U, eval, 1);
			} else {
				cPar.trace_G=EigenDecomp (G, U, eval, 0);
			}

			cPar.trace_G=0.0;
			for (size_t i=0; i<eval->size; i++) {
				if (gsl_vector_get (eval, i)<1e-10) {gsl_vector_set (eval, i, 0);}
				cPar.trace_G+=gsl_vector_get (eval, i);
			}
			cPar.trace_G/=(double)eval->size;

			cPar.time_eigen=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		} else {
			ReadFile_eigenU (cPar.file_ku, cPar.error, U);
			if (cPar.error==true) {cout<<"error! fail to read the U file. "<<endl; return;}

			ReadFile_eigenD (cPar.file_kd, cPar.error, eval);
			if (cPar.error==true) {cout<<"error! fail to read the D file. "<<endl; return;}

			cPar.trace_G=0.0;
			for (size_t i=0; i<eval->size; i++) {
				if (gsl_vector_get(eval, i)<1e-10) {gsl_vector_set(eval, i, 0);}
			  	cPar.trace_G+=gsl_vector_get(eval, i);
			}
			cPar.trace_G/=(double)eval->size;
		}
		*/
		//fit multiple variance components
		if (cPar.n_ph==1) {
		  //		  if (cPar.n_vc==1) {
		    /*
		    //calculate UtW and Uty
		    CalcUtX (U, W, UtW);
		    CalcUtX (U, Y, UtY);

		    gsl_vector_view beta=gsl_matrix_row (B, 0);
		    gsl_vector_view se_beta=gsl_matrix_row (se_B, 0);
		    gsl_vector_view UtY_col=gsl_matrix_column (UtY, 0);

		    CalcLambda ('L', eval, UtW, &UtY_col.vector, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_mle_null, cPar.logl_mle_H0);
		    CalcLmmVgVeBeta (eval, UtW, &UtY_col.vector, cPar.l_mle_null, cPar.vg_mle_null, cPar.ve_mle_null, &beta.vector, &se_beta.vector);

		    cPar.beta_mle_null.clear();
		    cPar.se_beta_mle_null.clear();
		    for (size_t i=0; i<B->size2; i++) {
		      cPar.beta_mle_null.push_back(gsl_matrix_get(B, 0, i) );
		      cPar.se_beta_mle_null.push_back(gsl_matrix_get(se_B, 0, i) );
		    }

		    CalcLambda ('R', eval, UtW, &UtY_col.vector, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_remle_null, cPar.logl_remle_H0);
		    CalcLmmVgVeBeta (eval, UtW, &UtY_col.vector, cPar.l_remle_null, cPar.vg_remle_null, cPar.ve_remle_null, &beta.vector, &se_beta.vector);
		    cPar.beta_remle_null.clear();
		    cPar.se_beta_remle_null.clear();
		    for (size_t i=0; i<B->size2; i++) {
		      cPar.beta_remle_null.push_back(gsl_matrix_get(B, 0, i) );
		      cPar.se_beta_remle_null.push_back(gsl_matrix_get(se_B, 0, i) );
		    }

		    CalcPve (eval, UtW, &UtY_col.vector, cPar.l_remle_null, cPar.trace_G, cPar.pve_null, cPar.pve_se_null);
		    cPar.PrintSummary();

		    //calculate and output residuals
		    if (cPar.a_mode==5) {
		      gsl_vector *Utu_hat=gsl_vector_alloc (Y->size1);
		      gsl_vector *Ute_hat=gsl_vector_alloc (Y->size1);
		      gsl_vector *u_hat=gsl_vector_alloc (Y->size1);
		      gsl_vector *e_hat=gsl_vector_alloc (Y->size1);
		      gsl_vector *y_hat=gsl_vector_alloc (Y->size1);

		      //obtain Utu and Ute
		      gsl_vector_memcpy (y_hat, &UtY_col.vector);
		      gsl_blas_dgemv (CblasNoTrans, -1.0, UtW, &beta.vector, 1.0, y_hat);

		      double d, u, e;
		      for (size_t i=0; i<eval->size; i++) {
			d=gsl_vector_get (eval, i);
			u=cPar.l_remle_null*d/(cPar.l_remle_null*d+1.0)*gsl_vector_get(y_hat, i);
			e=1.0/(cPar.l_remle_null*d+1.0)*gsl_vector_get(y_hat, i);
			gsl_vector_set (Utu_hat, i, u);
			gsl_vector_set (Ute_hat, i, e);
		      }

		      //obtain u and e
		      gsl_blas_dgemv (CblasNoTrans, 1.0, U, Utu_hat, 0.0, u_hat);
		      gsl_blas_dgemv (CblasNoTrans, 1.0, U, Ute_hat, 0.0, e_hat);

		      //output residuals
		      cPar.WriteVector(u_hat, "residU");
		      cPar.WriteVector(e_hat, "residE");

		      gsl_vector_free(u_hat);
		      gsl_vector_free(e_hat);
		      gsl_vector_free(y_hat);
		    }
*/
		  //		  } else {
		    gsl_vector_view Y_col=gsl_matrix_column (Y, 0);
		    VC cVc;
		    // cVc.CopyFromParam(cPar);
		    //GJMZ Added: code to fit VC model at all sites in KING segments.gz file 
			// if (!(cPar.file_king).empty() && !(cPar.file_segments).empty()){
			if (!(cPar.file_segments).empty() && 
				(!(cPar.file_kin).empty() || !(cPar.file_mk).empty() || !(cPar.file_king).empty() || !(cPar.file_seg).empty())){
			  cout << "Analyzing all sites in KING segments file" << endl;
			  
			  //open linkage output file and print the header
			  string file_link_str;
			  file_link_str=cPar.path_out+"/"+cPar.file_out;
			  file_link_str+=".vclink.txt";
			  ofstream outfile_link (file_link_str.c_str(), ofstream::out);
			  if (!outfile_link) {cout<<"error writing file: "<<file_link_str.c_str()<<endl; return;}
			  outfile_link << "chr" << "\t";
			  outfile_link << "pos" << "\t";
			  outfile_link << "sigma2_K" << "\t";
			  outfile_link << "sigma2_IBD" << "\t";
			  outfile_link << "sigma2_e" << "\t";
			  outfile_link << "pve_K" << "\t";
			  outfile_link << "pve_IBD" << "\t";
			  outfile_link << "se_sigma2_IBD" << "\t";
			  outfile_link << "zstat" << "\t";
			  outfile_link << "pvalue" << "\t";
			  outfile_link << "lod" << endl;
			  // outfile_link << "lod" << "\t";
			  // extra diagnostics
			  // outfile_link << "S_11" << "\t";
			  // outfile_link << "S_12" << "\t";
			  // outfile_link << "S_22" << "\t";
			  // outfile_link << "n_IBD" << "\t";
			  // outfile_link << "avg_K_IBD" << endl;
			  
			  // double ibd;
			  // size_t n_id1, n_id2;
			  gsl_matrix *Gtmp=gsl_matrix_alloc (G->size1, G->size2); //change? It is inefficient to copy the whole matrix
			  size_t n_offdiag = 0;
			  // double tot_k_ibd = 0;
			  
	  		  size_t i=cPar.n_vc-1;
			  // gsl_matrix_view G_sub0=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			  // gsl_matrix_view Gtmp_sub0=gsl_matrix_submatrix (Gtmp, 0, i*Gtmp->size1, Gtmp->size1, Gtmp->size1);
			  gsl_matrix_view G_sub0=gsl_matrix_submatrix (G, 0, 0, G->size1, i*G->size1);
			  gsl_matrix_view Gtmp_sub0=gsl_matrix_submatrix (Gtmp, 0, 0, Gtmp->size1, i*Gtmp->size1);
			  gsl_matrix_memcpy(&Gtmp_sub0.matrix, &G_sub0.matrix); //copy the first half of the matrix only once
	  		  
			  // size_t i=1;
			  // i=cPar.n_vc-1;
			  gsl_matrix_view G_sub1=gsl_matrix_submatrix (G, 0, i*G->size1, G->size1, G->size1);
			  gsl_matrix_view Gtmp_sub1=gsl_matrix_submatrix (Gtmp, 0, i*Gtmp->size1, Gtmp->size1, Gtmp->size1);

			  // Standardize y once and reuse it for each iteration
			  gsl_vector *y_scale=gsl_vector_alloc (G->size1);
			  double var_y, var_y_new;

			  gsl_vector_memcpy (y_scale, &Y_col.vector);
			  CenterVector (y_scale);
  
			  var_y=VectorVar (&Y_col.vector);
			  var_y_new=VectorVar (y_scale);

			  StandardizeVector (y_scale);

			  // Reuse Kry[0]
			  gsl_matrix *Kry = NULL;
			  gsl_matrix *yKrKKry = NULL;
			  gsl_vector *q_vec = gsl_vector_alloc (cPar.n_vc);
			  gsl_matrix *S_mat = NULL;
			  // double s_11, s_12, s_22;
			  // int chr;
			  // double StartMB, StopMB;

			  // for(auto const &ent1 : chrpos2segs) {
			  
			  //test only at the desired number of markers
			  size_t tot_m = 0;
			  
			  //string to hold SQL query
			  // string queryString;
			  // string queryString = "SELECT id1, id2, ibd FROM s WHERE chr = ?1 AND pos = ?2";
			  // string queryString = "SELECT id1, id2, ibd FROM s WHERE chrpos = ?1";
			  // sqlite3_stmt* queryStmt = NULL;
			  // if (sqlite3_prepare_v2(db, queryString.c_str(), -1, &queryStmt, NULL) != SQLITE_OK) {
				  // cerr << "Failed to prepare statement" << endl;
				  // return;
			  // }

			  for(auto &ent1 : chrpos2segs)
				tot_m += ent1.second.size();
			  // for(auto &ent1 : chrpos2segs)
				// for(auto &ent2 : ent1.second)
				  // tot_m += ftell(ent2.second) / (3 * sizeof(uint32_t) + sizeof(uint8_t));
// cout << "BUFSIZ: " << BUFSIZ << endl;
// cout << "bytes/line: " << 3 * sizeof(uint32_t) + sizeof(uint8_t) << endl;
// cout << "tot_m: " << tot_m << endl;
			  // size_t i_m = 0;
			  vector<Segments> f_segs;
			  // const int m = cPar.m;
			  // double skip = (double)tot_m / (double)cPar.m;
			  double skip = 1000000 * (double)tot_m / (double)cPar.m;
// cout << "skip: " << skip << endl;
			  long int n_pos = 0;
			  int c_Mb = 0;
			  long int c_pos = 0;
			  for(auto &ent1 : chrpos2segs) {
				// TODO: use prepared statement and binding to get 
				// repeated queries for different position from the same chr table
				// hopefully results in speedup and no more D state when running
				
				// ent1.first is the first key
				// for(auto const &ent2 : ent1.second) {
				for(auto &ent2 : ent1.second) {
				  // ent2.first is the second key
				  // ent2.second is the data

				  // while (ent2.second.get_seg(&n_id1, &n_id2, &ibd)) {
					// Cov_d=gsl_matrix_get(G, n_id1, n_id2);
					// if (Cov_d!=0 && Cov_d!=ibd) {cout<<"error! redundant and unequal terms in the kinship file, for id1 = "<<id1<<" and id2 = "<<id2<<endl;}
					// else {
//debug
// cout << n_id1 << "\t" << n_id2 << "\t" << ibd << endl;
					// gsl_matrix_set(&G_sub1.matrix, n_id1, n_id2, ibd);
					// gsl_matrix_set(&G_sub1.matrix, n_id2, n_id1, ibd);
					// }
				  // }
// 				  for(auto &ent3 : ent2.second) {
// 					for(auto &ent4 : ent3.second) {
// 					  gsl_matrix_set(&G_sub1.matrix, ent3.first, ent4.first, ent4.second);
// 					  gsl_matrix_set(&G_sub1.matrix, ent4.first, ent3.first, ent4.second);
// 					  n_offdiag += (ent4.second > 0 ? 1 : -1);
// 					}
// 				  }

				  size_t array_size = ftell(ent2.second) / (
					3 * sizeof(uint32_t) + sizeof(uint8_t));
// cout << "array_size: " << array_size << endl;
				  f_segs.resize(array_size);
				  rewind(ent2.second);
				  vector<Segments>::iterator it=f_segs.begin();
				  string segs_f_str = segs_f_pref+to_string(ent1.first)+"."+to_string(ent2.first);
				  while(!feof(ent2.second) && it != f_segs.end()){
					fread(&(it->pos), sizeof(uint32_t), 1, ent2.second);
					if (ferror(ent2.second)) {cout<<"error! fail to read file: "<<segs_f_str<<endl; cPar.error=true; return;}
					fread(&(it->id1), sizeof(uint32_t), 1, ent2.second);
					if (ferror(ent2.second)) {cout<<"error! fail to read file: "<<segs_f_str<<endl; cPar.error=true; return;}
					fread(&(it->id2), sizeof(uint32_t), 1, ent2.second);
					if (ferror(ent2.second)) {cout<<"error! fail to read file: "<<segs_f_str<<endl; cPar.error=true; return;}
					fread(&(it->ibd), sizeof(uint8_t), 1, ent2.second);
					if (ferror(ent2.second)) {cout<<"error! fail to read file: "<<segs_f_str<<endl; cPar.error=true; return;}
				    // gsl_matrix_set(&G_sub1.matrix, n_id1, n_id2, ibd);
					// gsl_matrix_set(&G_sub1.matrix, n_id2, n_id1, ibd);
					// n_offdiag += (ibd > 0 ? 1 : -1);
					++it;
				  }
				  fclose(ent2.second);
				  ent2.second = NULL;
				  remove(segs_f_str.c_str());
				  sort(f_segs.begin(), f_segs.end(), comp_segs);
				  
				  for (vector<Segments>::iterator it=f_segs.begin(); it!=f_segs.end(); /*++*/it){
					long int pos = it->pos;
				  	while (pos == it->pos){
				      gsl_matrix_set(&G_sub1.matrix, it->id1, it->id2, it->ibd);
					  gsl_matrix_set(&G_sub1.matrix, it->id2, it->id1, it->ibd);
					  n_offdiag += (it->ibd > 0 ? 1 : -1);
					  // tot_k_ibd += (it->ibd > 0 ? 1 : -1) * gsl_matrix_get(&G_sub0.matrix, it->id1, it->id2);
					  ++it;
					}
				  // }
				  //finalize
				  // sqlite3_finalize(ent2.second);
				  
				  //SELECT * statement
				  // queryString = string("SELECT * FROM chr") + 
					// to_string(ent1.first) + "_" + to_string(ent2.first);
				  // if (sqlite3_prepare_v2(db, queryString.c_str(), -1, &ent2.second, NULL) != SQLITE_OK) {
					// cerr << "Failed to prepare statement" << endl;
					// return;
				  // }
				  //loop
				  // while( sqlite3_step(ent2.second) == SQLITE_ROW)	{
					//grab columns
					// n_id1 = sqlite3_column_int(ent2.second, 0);
					// n_id2 = sqlite3_column_int(ent2.second, 1);
					// ibd = sqlite3_column_double(ent2.second, 2);
					
					//update kinship matrix
				    // gsl_matrix_set(&G_sub1.matrix, n_id1, n_id2, ibd);
					// gsl_matrix_set(&G_sub1.matrix, n_id2, n_id1, ibd);
					// n_offdiag += (ibd > 0 ? 1 : -1);
				  // }
				  // if (	//sqlite3_bind_int(queryStmt, 1, ent1.first) != SQLITE_OK ||
						// sqlite3_bind_int(queryStmt, 1, ent1.first * 1000000000 + ent2.first) != SQLITE_OK	) {
						// cerr << "Failed to bind" << endl;
						// return;
				  // }
				  // while( sqlite3_step(queryStmt) == SQLITE_ROW)	{
					// grab columns
					// n_id1 = sqlite3_column_int(queryStmt, 0);
					// n_id2 = sqlite3_column_int(queryStmt, 1);
					// ibd = sqlite3_column_double(queryStmt, 2);
					
					// update kinship matrix
				    // gsl_matrix_set(&G_sub1.matrix, n_id1, n_id2, ibd);
					// gsl_matrix_set(&G_sub1.matrix, n_id2, n_id1, ibd);
					// n_offdiag += (ibd > 0 ? 1 : -1);
				  // }
				  // sqlite3_clear_bindings(queryStmt);
				  // sqlite3_reset(queryStmt);

				  //finalize
				  // sqlite3_finalize(ent2.second);
				  
					// cout << endl << "Chr:" << ent1.first << "\tPos:" << ent2.first << endl;
					cout << endl << "Chr:" << ent1.first << "\tPos:" << pos << endl;
					// if (fmod(i_m++, skip) >= 1)
					c_pos = 1000000 * (long int)c_Mb + (pos % 1000000);
					if (n_pos > c_pos)
					  cout << "Skipped due to max-link-tests = " << cPar.m << endl;
					else if (n_offdiag == 0) //skip any sites where matrices have no ibd segments
					  cout << "No IBD at this locus. Skipping." << endl;
					else {
					  // for(n_pos; n_pos <= pos; n_pos += skip);
					  n_pos += skip;
//test
// cout << "n_offdiag: " << n_offdiag << endl;
// if (n_offdiag == 0) {
	// cout << "Diagonal matrix" << endl;
// }
//test
					  //G must remain in the original units, so copy to a temp mat
					  //for the centering, etc.
					  gsl_matrix_memcpy(&Gtmp_sub1.matrix, &G_sub1.matrix);
					  // center matrix G
//debug
// if(i == 1){
// string test_fname = to_string(ent1.first) + "_" + to_string(ent2.first) + "_test.txt";
// FILE * test_f = fopen(test_fname.c_str(), "w");
// gsl_matrix_fprintf(test_f, &Gtmp_sub1.matrix, "%f");
// fclose(test_f);
// }
//end debug
					  CenterMatrix (&Gtmp_sub1.matrix);

					  // (cPar.v_traceG).clear();
					  double d=0;
					  for (size_t j=0; j<Gtmp->size1; j++) {
						d+=gsl_matrix_get (&Gtmp_sub1.matrix, j, j);
					  }
					  d/=(double)Gtmp->size1;
					  // (cPar.v_traceG).push_back(d);
					  (cPar.v_traceG).back() = d;

					  d = ScaleMatrix (&Gtmp_sub1.matrix);
					  // traceG_new[1] = d;
					  traceG_new.back() = d;

					  cVc.CopyFromParam(cPar);

					  if (cPar.a_mode==61) {
						// cVc.CalcVChe (Gtmp, W, &Y_col.vector);
						// cVc.Calc2VCheMulti (Gtmp, traceG_new, y_scale, var_y, var_y_new, Kry, q_vec, yKrKKry, S_mat, s_11, s_12, s_22);
						cVc.Calc2VCheMulti (Gtmp, traceG_new, y_scale, var_y, var_y_new, Kry, q_vec, yKrKKry, S_mat);
					  } else if (cPar.a_mode==62) {
						cVc.CalcVCreml (cPar.noconstrain, Gtmp, W, &Y_col.vector);
					  } else {
						cVc.CalcVCacl (Gtmp, W, &Y_col.vector);
					  }
					  //print the output to a file
					  // WriteFile_linkage(cVc); //consider making a new function
					  // double zstat = cVc.v_sigma2[1] / cVc.v_se_sigma2[1];
					  double zstat = cVc.v_sigma2[cPar.n_vc-1] / cVc.v_se_sigma2[cPar.n_vc-1];
					  double pvalue = gsl_cdf_ugaussian_Q(zstat);
					  double lod = zstat <= 0 ? 0 : M_LOG10E * zstat * zstat * 0.5;
					
					  outfile_link << ent1.first << "\t";
					  // outfile_link << ent2.first << "\t";
					  outfile_link << pos << "\t";
					  outfile_link << cVc.v_sigma2[0] << "\t";
					  // outfile_link << cVc.v_sigma2[1] << "\t";
					  // outfile_link << cVc.v_sigma2[2] << "\t";
					  outfile_link << cVc.v_sigma2[cPar.n_vc-1] << "\t";
					  outfile_link << cVc.v_sigma2.back() << "\t";
					  outfile_link << cVc.v_pve[0] << "\t";
					  // outfile_link << cVc.v_pve[1] << "\t";
					  // outfile_link << cVc.v_se_sigma2[1] << "\t";
					  outfile_link << cVc.v_pve[cPar.n_vc-1] << "\t";
					  outfile_link << cVc.v_se_sigma2[cPar.n_vc-1] << "\t";
					  outfile_link << zstat << "\t";
					  outfile_link << pvalue << "\t";
					  outfile_link << lod << endl;
					  // outfile_link << lod << "\t";
					  /*TODO: print out S_mat */
					  // outfile_link << gsl_matrix_get(S_mat, 0, 0) << "\t";
					  // outfile_link << gsl_matrix_get(S_mat, 0, 1) << "\t";
					  // outfile_link << gsl_matrix_get(S_mat, 1, 1) << "\t";
					  // outfile_link << s_11 << "\t";
					  // outfile_link << s_12 << "\t";
					  // outfile_link << s_22 << "\t";
					  /*TODO: print out expected, observed n ibd, avg K of ibd */
					  // outfile_link << n_offdiag << "\t";
					  // outfile_link << (double)tot_k_ibd / n_offdiag << endl;
					  
//debug
// gsl_matrix_fprintf(stdout, S_mat, "%f");
//end debug
					}
				  }
				  c_Mb++;
				}
			  }
			  //close db connection
			  // sqlite3_close(db);
			  // sqlite3_finalize(queryStmt);

			  //free matrices and vectors reused by Calc2VCheMulti
			  gsl_vector_free(y_scale);
			  gsl_matrix_free(Kry);
			  gsl_matrix_free(yKrKKry);
			  gsl_vector_free(q_vec);
			  gsl_matrix_free(S_mat);

			  //close linkage output file
			  outfile_link.clear();
			  outfile_link.close();
			}
			else if (!cPar.file_segments.empty()) {
			  cout << "Analyzing all sites in KING segments file" << endl;
			  
			  // double ibd;
			  // size_t n_id1, n_id2;
			  gsl_matrix *Gtmp=gsl_matrix_alloc (G->size1, G->size1);
			  // int chr;
			  // double StartMB, StopMB;

			  // for(auto const &ent1 : chrpos2segs) {
			  for(auto &ent1 : chrpos2segs) {
				// ent1.first is the first key
				// for(auto const &ent2 : ent1.second) {
				for(auto &ent2 : ent1.second) {
				  // ent2.first is the second key
				  // ent2.second is the data

				  // while (ent2.second.get_seg(&n_id1, &n_id2, &ibd)) {
					// Cov_d=gsl_matrix_get(G, n_id1, n_id2);
					// if (Cov_d!=0 && Cov_d!=ibd) {cout<<"error! redundant and unequal terms in the kinship file, for id1 = "<<id1<<" and id2 = "<<id2<<endl;}
					// else {
					// gsl_matrix_set(G, n_id1, n_id2, ibd);
					// gsl_matrix_set(G, n_id2, n_id1, ibd);
					// }
				  // }
// 				  for(auto &ent3 : ent2.second) {
// 					for(auto &ent4 : ent3.second) {
// 					  gsl_matrix_set(G, ent3.first, ent4.first, ent4.second);
// 					  gsl_matrix_set(G, ent4.first, ent3.first, ent4.second);
// 					}
// 				  }

				  cout << endl << "Chr:" << ent1.first << "\tPos:" << ent2.first << endl;
				  
				  //G must remain in the original units, so copy to a temp mat
				  //for the centering, etc.
				  gsl_matrix_memcpy(Gtmp, G);
				  // center matrix G
				  CenterMatrix (Gtmp);

				  (cPar.v_traceG).clear();
				  double d=0;
				  for (size_t j=0; j<Gtmp->size1; j++) {
					d+=gsl_matrix_get (Gtmp, j, j);
				  }
				  d/=(double)Gtmp->size1;
				  (cPar.v_traceG).push_back(d);

				  cVc.CopyFromParam(cPar);

				  if (cPar.a_mode==61) {
					cVc.CalcVChe (Gtmp, W, &Y_col.vector);
				  } else if (cPar.a_mode==62) {
					cVc.CalcVCreml (cPar.noconstrain, Gtmp, W, &Y_col.vector);
				  } else {
					cVc.CalcVCacl (Gtmp, W, &Y_col.vector);
				  }
				}
			  }
			}
		    else {
				cVc.CopyFromParam(cPar);
			  if (cPar.a_mode==61) {
				cVc.CalcVChe (G, W, &Y_col.vector);
			  } else if (cPar.a_mode==62) {
				cVc.CalcVCreml (cPar.noconstrain, G, W, &Y_col.vector);
			  } else {
				cVc.CalcVCacl (G, W, &Y_col.vector);
			  }
			}
		    cVc.CopyToParam(cPar);
		    //obtain pve from sigma2
		    //obtain se_pve from se_sigma2

		    //}
			//output combined matrix k*sigma2
			//condsider putting in another function
			if (cPar.combine_mats) {
			  // scale each matrix in G by v_sigma2
			  WriteFile_combined_kin (cPar, G);
			  // map<size_t, size_t> mapID2ID;
			  // size_t c=0;
			  // for (size_t i=0; i<cPar.indicator_idv.size(); i++) {
				// if (cPar.indicator_idv[i]==1) {mapID2ID[i]=c; c++;}
			  // }

			  // string id1, id2;
			  // size_t n_id1, n_id2;

			  // double g;
			  // const size_t n1 = G->size1;
			  // const size_t n2 = G->size2;
			  // const size_t n_vc = n2/n1;
			  
			  // string file_Kcmb_str;
			  // file_Kcmb_str=cPar.path_out+"/"+cPar.file_out;
			  // file_Kcmb_str+=".K_combined.gz";
			  // ogzstream outfile_Kcmb (file_Kcmb_str.c_str());
			  
			  // for (map<string, int>::iterator it1 = cPar.mapID2num.begin(); it1!=cPar.mapID2num.end(); ++it1) {
				// if(cPar.indicator_idv[it1->second]==0) {continue;}
				// id1 = it1->first;
				// n_id1 = mapID2ID[it1->second];
				
				// for (map<string, int>::iterator it2 = it1; it2!=cPar.mapID2num.end(); ++it2) {
				  // if (cPar.indicator_idv[it2->second]==0) {continue;}
				  // id2 = it2->first;
				  // n_id2=mapID2ID[it2->second];
				  
				  // g=0;
				  // for (int vc=0; vc < n_vc; vc++) {
					// g += cVc.v_sigma2[vc] * gsl_matrix_get(G, n_id1, n1*vc + n_id2);
				  // }
				  // outfile_Kcmb << id1 << " " << id2 << " " << g << "\n";
				// }
			  // }
			  // outfile_Kcmb.close();
			  // outfile_Kcmb.clear();
			}

		}
	  }

	}


	//compute confidence intervals with additional summary statistics
	//we do not check the sign of z-scores here, but they have to be matched with the genotypes
	if (cPar.a_mode==66 || cPar.a_mode==67) {
	  //read reference file first
	  gsl_matrix *S=gsl_matrix_alloc (cPar.n_vc, cPar.n_vc);
	  gsl_matrix *Svar=gsl_matrix_alloc (cPar.n_vc, cPar.n_vc);
	  gsl_vector *s_ref=gsl_vector_alloc (cPar.n_vc);

	  gsl_matrix_set_zero(S);
	  gsl_matrix_set_zero(Svar);
	  gsl_vector_set_zero(s_ref);

	  if (!cPar.file_ref.empty()) {
	    ReadFile_ref(cPar.file_ref, S, Svar, s_ref, cPar.ni_ref);
	  } else {
	    ReadFile_mref(cPar.file_mref, S, Svar, s_ref, cPar.ni_ref);
	  }

	  //need to obtain a common set of SNPs between beta file and the genotype file; these are saved in mapRS2wA and mapRS2wK
	  //normalize the weight in mapRS2wK to have an average of one; each element of mapRS2wA is 1
	  set<string> setSnps_beta;
	  ReadFile_snps_header (cPar.file_beta, setSnps_beta);

	  //obtain the weights for wA, which contains the SNP weights for SNPs used in the model
	  map <string, double> mapRS2wK;
	  cPar.ObtainWeight(setSnps_beta, mapRS2wK);

	  //set up matrices and vector
	  gsl_matrix *Xz=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc);
	  gsl_matrix *XWz=gsl_matrix_alloc (cPar.ni_test, cPar.n_vc);
	  gsl_matrix *XtXWz=gsl_matrix_alloc (mapRS2wK.size(), cPar.n_vc*cPar.n_vc);
	  gsl_vector *w=gsl_vector_alloc (mapRS2wK.size());
	  gsl_vector *w1=gsl_vector_alloc (mapRS2wK.size());
	  gsl_vector *z=gsl_vector_alloc (mapRS2wK.size());
	  gsl_vector *s_vec=gsl_vector_alloc (cPar.n_vc);

	  vector<size_t> vec_cat, vec_size;
	  vector<double> vec_z;

	  map <string, double> mapRS2z, mapRS2wA;
	  map <string, string> mapRS2A1;
	  string file_str;

	  //update s_vec, the number of snps in each category
	  for (size_t i=0; i<cPar.n_vc; i++) {
	    vec_size.push_back(0);
	  }

	  for (map<string, double>::const_iterator it=mapRS2wK.begin(); it!=mapRS2wK.end(); ++it) {
	    vec_size[cPar.mapRS2cat[it->first]]++;
	  }

	  for (size_t i=0; i<cPar.n_vc; i++) {
	    gsl_vector_set(s_vec, i, vec_size[i]);
	  }

	  //update mapRS2wA using v_pve and s_vec
	  if (cPar.a_mode==66) {
	    for (map<string, double>::const_iterator it=mapRS2wK.begin(); it!=mapRS2wK.end(); ++it) {
	      mapRS2wA[it->first]=1;
	    }
	  } else {
	    cPar.UpdateWeight (0, mapRS2wK, cPar.ni_test, s_vec, mapRS2wA);
	  }

	  //read in z-scores based on allele 0, and save that into a vector
	  ReadFile_beta (cPar.file_beta, mapRS2wA, mapRS2A1, mapRS2z);

	  //update snp indicator, save weights to w, save z-scores to vec_z, save category label to vec_cat
	  //sign of z is determined by matching alleles
	  cPar.UpdateSNPnZ (mapRS2wA, mapRS2A1, mapRS2z, w, z, vec_cat);

	  //compute an n by k matrix of X_iWz
	  cout<<"Calculating Xz ... "<<endl;

	  gsl_matrix_set_zero(Xz);
	  gsl_vector_set_all (w1, 1);

	  if (!cPar.file_bfile.empty() ) {
	    file_str=cPar.file_bfile+".bed";
	    PlinkXwz (file_str, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, vec_cat, w1, z, 0, Xz);
	  } else if (!cPar.file_geno.empty()) {
	    BimbamXwz (cPar.file_geno, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, vec_cat, w1, z, 0, Xz);
	  } else if (!cPar.file_mbfile.empty() ){
	    MFILEXwz (1, cPar.file_mbfile, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, vec_cat, w1, z, Xz);
	  } else if (!cPar.file_mgeno.empty()) {
	    MFILEXwz (0, cPar.file_mgeno, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, vec_cat, w1, z, Xz);
	  }
	  /*
	  cout<<"Xz: "<<endl;
	  for (size_t i=0; i<5; i++) {
	    for (size_t j=0; j<cPar.n_vc; j++) {
	      cout<<gsl_matrix_get (Xz, i, j)<<" ";
	    }
	    cout<<endl;
	  }
	  */
	  if (cPar.a_mode==66) {
	    gsl_matrix_memcpy (XWz, Xz);
	  } else if (cPar.a_mode==67) {
	    cout<<"Calculating XWz ... "<<endl;

	    gsl_matrix_set_zero(XWz);

	    if (!cPar.file_bfile.empty() ) {
	      file_str=cPar.file_bfile+".bed";
	      PlinkXwz (file_str, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, vec_cat, w, z, 0, XWz);
	    } else if (!cPar.file_geno.empty()) {
	      BimbamXwz (cPar.file_geno, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, vec_cat, w, z, 0, XWz);
	    } else if (!cPar.file_mbfile.empty() ){
	      MFILEXwz (1, cPar.file_mbfile, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, vec_cat, w, z, XWz);
	    } else if (!cPar.file_mgeno.empty()) {
	      MFILEXwz (0, cPar.file_mgeno, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, vec_cat, w, z, XWz);
	    }
	  }
	  /*
	  cout<<"XWz: "<<endl;
	  for (size_t i=0; i<5; i++) {
	    cout<<gsl_vector_get (w, i)<<endl;
	    for (size_t j=0; j<cPar.n_vc; j++) {
	      cout<<gsl_matrix_get (XWz, i, j)<<" ";
	    }
	    cout<<endl;
	  }
	  */
	  //compute an p by k matrix of X_j^TWX_iWz
	  cout<<"Calculating XtXWz ... "<<endl;
	  gsl_matrix_set_zero(XtXWz);

	  if (!cPar.file_bfile.empty() ) {
	    file_str=cPar.file_bfile+".bed";
	    PlinkXtXwz (file_str, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, XWz, 0, XtXWz);
	  } else if (!cPar.file_geno.empty()) {
	    BimbamXtXwz (cPar.file_geno, cPar.d_pace, cPar.indicator_idv, cPar.indicator_snp, XWz, 0, XtXWz);
	  } else if (!cPar.file_mbfile.empty() ){
	    MFILEXtXwz (1, cPar.file_mbfile, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, XWz, XtXWz);
	  } else if (!cPar.file_mgeno.empty()) {
	    MFILEXtXwz (0, cPar.file_mgeno, cPar.d_pace, cPar.indicator_idv, cPar.mindicator_snp, XWz, XtXWz);
	  }
	  /*
	  cout<<"XtXWz: "<<endl;
	  for (size_t i=0; i<5; i++) {
	    for (size_t j=0; j<cPar.n_vc; j++) {
	      cout<<gsl_matrix_get (XtXWz, i, j)<<" ";
	    }
	    cout<<endl;
	  }
	  */
	  //compute confidence intervals
	  CalcCIss(Xz, XWz, XtXWz, S, Svar, w, z, s_vec, vec_cat, cPar.v_pve, cPar.v_se_pve, cPar.pve_total, cPar.se_pve_total, cPar.v_sigma2, cPar.v_se_sigma2, cPar.v_enrich, cPar.v_se_enrich);

	  //write files
	  //cPar.WriteMatrix (XWz, "XWz");
	  //cPar.WriteMatrix (XtXWz, "XtXWz");
	  //cPar.WriteVector (w, "w");

	  gsl_matrix_free(S);
	  gsl_matrix_free(Svar);
	  gsl_vector_free(s_ref);

	  gsl_matrix_free(Xz);
	  gsl_matrix_free(XWz);
	  gsl_matrix_free(XtXWz);
	  gsl_vector_free(w);
	  gsl_vector_free(w1);
	  gsl_vector_free(z);
	  gsl_vector_free(s_vec);
	}


	//LMM or mvLMM or Eigen-Decomposition
	if (cPar.a_mode==1 || cPar.a_mode==2 || cPar.a_mode==3 || cPar.a_mode==4 || cPar.a_mode==5 || cPar.a_mode==31) {  //Fit LMM or mvLMM or eigen
		gsl_matrix *Y=gsl_matrix_alloc (cPar.ni_test, cPar.n_ph);
		gsl_matrix *W=gsl_matrix_alloc (Y->size1, cPar.n_cvt);
		gsl_matrix *B=gsl_matrix_alloc (Y->size2, W->size2);	//B is a d by c matrix
		gsl_matrix *se_B=gsl_matrix_alloc (Y->size2, W->size2);
		gsl_matrix *G=gsl_matrix_alloc (Y->size1, Y->size1);
		gsl_matrix *U=gsl_matrix_alloc (Y->size1, Y->size1);
		gsl_matrix *UtW=gsl_matrix_alloc (Y->size1, W->size2);
		gsl_matrix *UtY=gsl_matrix_alloc (Y->size1, Y->size2);
		gsl_vector *eval=gsl_vector_alloc (Y->size1);
		gsl_vector *env=gsl_vector_alloc (Y->size1);
		gsl_vector *weight=gsl_vector_alloc (Y->size1);

		//set covariates matrix W and phenotype matrix Y
		//an intercept should be included in W,
		cPar.CopyCvtPhen (W, Y, 0);
		if (!cPar.file_gxe.empty()) {cPar.CopyGxe (env);}

		//read relatedness matrix G
		if (!(cPar.file_kin).empty()) {
			ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

			//center matrix G
			CenterMatrix (G);

			//is residual weights are provided, then
			if (!cPar.file_weight.empty()) {
			  cPar.CopyWeight (weight);
			  double d, wi, wj;
			  for (size_t i=0; i<G->size1; i++) {
			    wi=gsl_vector_get(weight, i);
			    for (size_t j=i; j<G->size2; j++) {
			      wj=gsl_vector_get(weight, j);
			      d=gsl_matrix_get(G, i, j);
			      if (wi<=0 || wj<=0) {d=0;} else {d/=sqrt(wi*wj);}
			      gsl_matrix_set(G, i, j, d);
			      if (j!=i) {gsl_matrix_set(G, j, i, d);}
			    }
			  }
			}

			//eigen-decomposition and calculate trace_G
			cout<<"Start Eigen-Decomposition..."<<endl;
			time_start=clock();

			if (cPar.a_mode==31) {
				cPar.trace_G=EigenDecomp (G, U, eval, 1);
			} else {
				cPar.trace_G=EigenDecomp (G, U, eval, 0);
			}

			if (!cPar.file_weight.empty()) {
			  double wi;
			  for (size_t i=0; i<U->size1; i++) {
			    wi=gsl_vector_get(weight, i);
			    if (wi<=0) {wi=0;} else {wi=sqrt(wi);}
			    gsl_vector_view Urow=gsl_matrix_row (U, i);
			    gsl_vector_scale (&Urow.vector, wi);
			  }
			}

			cPar.trace_G=0.0;
			for (size_t i=0; i<eval->size; i++) {
				if (gsl_vector_get (eval, i)<1e-10) {gsl_vector_set (eval, i, 0);}
				cPar.trace_G+=gsl_vector_get (eval, i);
			}
			cPar.trace_G/=(double)eval->size;

			cPar.time_eigen=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		} else {
			ReadFile_eigenU (cPar.file_ku, cPar.error, U);
			if (cPar.error==true) {cout<<"error! fail to read the U file. "<<endl; return;}

			ReadFile_eigenD (cPar.file_kd, cPar.error, eval);
			if (cPar.error==true) {cout<<"error! fail to read the D file. "<<endl; return;}

			cPar.trace_G=0.0;
			for (size_t i=0; i<eval->size; i++) {
				if (gsl_vector_get(eval, i)<1e-10) {gsl_vector_set(eval, i, 0);}
			  	cPar.trace_G+=gsl_vector_get(eval, i);
			}
			cPar.trace_G/=(double)eval->size;
		}

		if (cPar.a_mode==31) {
			cPar.WriteMatrix(U, "eigenU");
			cPar.WriteVector(eval, "eigenD");
		} else if (!cPar.file_gene.empty() ) {
			//calculate UtW and Uty
			CalcUtX (U, W, UtW);
			CalcUtX (U, Y, UtY);

			LMM cLmm;
			cLmm.CopyFromParam(cPar);

			gsl_vector_view Y_col=gsl_matrix_column (Y, 0);
			gsl_vector_view UtY_col=gsl_matrix_column (UtY, 0);

			cLmm.AnalyzeGene (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector); //y is the predictor, not the phenotype

			cLmm.WriteFiles();
			cLmm.CopyToParam(cPar);
		} else {
		  //calculate UtW and Uty
		  CalcUtX (U, W, UtW);
		  CalcUtX (U, Y, UtY);

			//calculate REMLE/MLE estimate and pve for univariate model
			if (cPar.n_ph==1) {
				gsl_vector_view beta=gsl_matrix_row (B, 0);
				gsl_vector_view se_beta=gsl_matrix_row (se_B, 0);
				gsl_vector_view UtY_col=gsl_matrix_column (UtY, 0);

				CalcLambda ('L', eval, UtW, &UtY_col.vector, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_mle_null, cPar.logl_mle_H0);
				CalcLmmVgVeBeta (eval, UtW, &UtY_col.vector, cPar.l_mle_null, cPar.vg_mle_null, cPar.ve_mle_null, &beta.vector, &se_beta.vector);

				cPar.beta_mle_null.clear();
				cPar.se_beta_mle_null.clear();
				for (size_t i=0; i<B->size2; i++) {
					cPar.beta_mle_null.push_back(gsl_matrix_get(B, 0, i) );
					cPar.se_beta_mle_null.push_back(gsl_matrix_get(se_B, 0, i) );
				}

				CalcLambda ('R', eval, UtW, &UtY_col.vector, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_remle_null, cPar.logl_remle_H0);
				CalcLmmVgVeBeta (eval, UtW, &UtY_col.vector, cPar.l_remle_null, cPar.vg_remle_null, cPar.ve_remle_null, &beta.vector, &se_beta.vector);
				cPar.beta_remle_null.clear();
				cPar.se_beta_remle_null.clear();
				for (size_t i=0; i<B->size2; i++) {
					cPar.beta_remle_null.push_back(gsl_matrix_get(B, 0, i) );
					cPar.se_beta_remle_null.push_back(gsl_matrix_get(se_B, 0, i) );
				}

				CalcPve (eval, UtW, &UtY_col.vector, cPar.l_remle_null, cPar.trace_G, cPar.pve_null, cPar.pve_se_null);
				cPar.PrintSummary();

				//calculate and output residuals
				if (cPar.a_mode==5) {
					gsl_vector *Utu_hat=gsl_vector_alloc (Y->size1);
					gsl_vector *Ute_hat=gsl_vector_alloc (Y->size1);
					gsl_vector *u_hat=gsl_vector_alloc (Y->size1);
					gsl_vector *e_hat=gsl_vector_alloc (Y->size1);
					gsl_vector *y_hat=gsl_vector_alloc (Y->size1);

					//obtain Utu and Ute
					gsl_vector_memcpy (y_hat, &UtY_col.vector);
					gsl_blas_dgemv (CblasNoTrans, -1.0, UtW, &beta.vector, 1.0, y_hat);

					double d, u, e;
					for (size_t i=0; i<eval->size; i++) {
						d=gsl_vector_get (eval, i);
						u=cPar.l_remle_null*d/(cPar.l_remle_null*d+1.0)*gsl_vector_get(y_hat, i);
						e=1.0/(cPar.l_remle_null*d+1.0)*gsl_vector_get(y_hat, i);
						gsl_vector_set (Utu_hat, i, u);
						gsl_vector_set (Ute_hat, i, e);
					}

					//obtain u and e
					gsl_blas_dgemv (CblasNoTrans, 1.0, U, Utu_hat, 0.0, u_hat);
					gsl_blas_dgemv (CblasNoTrans, 1.0, U, Ute_hat, 0.0, e_hat);

					//output residuals
					cPar.WriteVector(u_hat, "residU");
					cPar.WriteVector(e_hat, "residE");

					gsl_vector_free(u_hat);
					gsl_vector_free(e_hat);
					gsl_vector_free(y_hat);
				}
			}

			//Fit LMM or mvLMM
			if (cPar.a_mode==1 || cPar.a_mode==2 || cPar.a_mode==3 || cPar.a_mode==4) {
				if (cPar.n_ph==1) {
					LMM cLmm;
					cLmm.CopyFromParam(cPar);

					gsl_vector_view Y_col=gsl_matrix_column (Y, 0);
					gsl_vector_view UtY_col=gsl_matrix_column (UtY, 0);

					if (!cPar.file_bfile.empty()) {
					  if (cPar.file_gxe.empty()) {
					    cLmm.AnalyzePlink (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector);
					  } else {
					    cLmm.AnalyzePlinkGXE (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector, env);
					  }
					}
					// WJA added
				       	else if(!cPar.file_oxford.empty()) {
					  cLmm.Analyzebgen (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector);
					}
					else {
					  if (cPar.file_gxe.empty()) {
					    cLmm.AnalyzeBimbam (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector);
					  } else {
					    cLmm.AnalyzeBimbamGXE (U, eval, UtW, &UtY_col.vector, W, &Y_col.vector, env);
					  }
					}

					cLmm.WriteFiles();
					cLmm.CopyToParam(cPar);
				} else {
					MVLMM cMvlmm;
					cMvlmm.CopyFromParam(cPar);

					if (!cPar.file_bfile.empty()) {
					  if (cPar.file_gxe.empty()) {
					    cMvlmm.AnalyzePlink (U, eval, UtW, UtY);
					  } else {
					    cMvlmm.AnalyzePlinkGXE (U, eval, UtW, UtY, env);
					  }
					}
					else if(!cPar.file_oxford.empty())
					{
					    cMvlmm.Analyzebgen (U, eval, UtW, UtY);
					}
					else {
					  if (cPar.file_gxe.empty()) {
					    cMvlmm.AnalyzeBimbam (U, eval, UtW, UtY);
					  } else {
					    cMvlmm.AnalyzeBimbamGXE (U, eval, UtW, UtY, env);
					  }
					}

					cMvlmm.WriteFiles();
					cMvlmm.CopyToParam(cPar);
				}
			}
		}


		//release all matrices and vectors
		gsl_matrix_free (Y);
		gsl_matrix_free (W);
		gsl_matrix_free(B);
		gsl_matrix_free(se_B);
		gsl_matrix_free (G);
		gsl_matrix_free (U);
		gsl_matrix_free (UtW);
		gsl_matrix_free (UtY);
		gsl_vector_free (eval);
		gsl_vector_free (env);
	}


	//BSLMM
	if (cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13) {
		gsl_vector *y=gsl_vector_alloc (cPar.ni_test);
		gsl_matrix *W=gsl_matrix_alloc (y->size, cPar.n_cvt);
		gsl_matrix *G=gsl_matrix_alloc (y->size, y->size);
		gsl_matrix *UtX=gsl_matrix_alloc (y->size, cPar.ns_test);

		//set covariates matrix W and phenotype vector y
		//an intercept should be included in W,
		cPar.CopyCvtPhen (W, y, 0);

		//center y, even for case/control data
		cPar.pheno_mean=CenterVector(y);

		//run bvsr if rho==1
		if (cPar.rho_min==1 && cPar.rho_max==1) {
		  //read genotypes X (not UtX)
		  cPar.ReadGenotypes (UtX, G, false);

		  //perform BSLMM analysis
		  BSLMM cBslmm;
		  cBslmm.CopyFromParam(cPar);
		  time_start=clock();
		  cBslmm.MCMC(UtX, y);
		  cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		  cBslmm.CopyToParam(cPar);
		  //else, if rho!=1
		} else {
		gsl_matrix *U=gsl_matrix_alloc (y->size, y->size);
		gsl_vector *eval=gsl_vector_alloc (y->size);
		gsl_matrix *UtW=gsl_matrix_alloc (y->size, W->size2);
		gsl_vector *Uty=gsl_vector_alloc (y->size);

		//read relatedness matrix G
		if (!(cPar.file_kin).empty()) {
			cPar.ReadGenotypes (UtX, G, false);

			//read relatedness matrix G
			ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
			if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

			//center matrix G
			CenterMatrix (G);
		} else {
			cPar.ReadGenotypes (UtX, G, true);
		}

		//eigen-decomposition and calculate trace_G
		cout<<"Start Eigen-Decomposition..."<<endl;
		time_start=clock();
		cPar.trace_G=EigenDecomp (G, U, eval, 0);
		cPar.trace_G=0.0;
		for (size_t i=0; i<eval->size; i++) {
			if (gsl_vector_get (eval, i)<1e-10) {gsl_vector_set (eval, i, 0);}
			cPar.trace_G+=gsl_vector_get (eval, i);
		}
		cPar.trace_G/=(double)eval->size;
		cPar.time_eigen=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

		//calculate UtW and Uty
		CalcUtX (U, W, UtW);
		CalcUtX (U, y, Uty);

		//calculate REMLE/MLE estimate and pve
		CalcLambda ('L', eval, UtW, Uty, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_mle_null, cPar.logl_mle_H0);
		CalcLambda ('R', eval, UtW, Uty, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_remle_null, cPar.logl_remle_H0);
		CalcPve (eval, UtW, Uty, cPar.l_remle_null, cPar.trace_G, cPar.pve_null, cPar.pve_se_null);

		cPar.PrintSummary();

		//Creat and calcualte UtX, use a large memory
		cout<<"Calculating UtX..."<<endl;
		time_start=clock();
		CalcUtX (U, UtX);
		cPar.time_UtX=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

		//perform BSLMM or BSLMMDAP analysis
		if (cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13) {
		  BSLMM cBslmm;
		  cBslmm.CopyFromParam(cPar);
		  time_start=clock();
		  if (cPar.a_mode==12) {  //ridge regression
		    cBslmm.RidgeR(U, UtX, Uty, eval, cPar.l_remle_null);
		  } else {	//Run MCMC
		    cBslmm.MCMC(U, UtX, Uty, eval, y);
		  }
		  cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		  cBslmm.CopyToParam(cPar);
		} else {
		}

		//release all matrices and vectors
		gsl_matrix_free (G);
		gsl_matrix_free (U);
		gsl_matrix_free (UtW);
		gsl_vector_free (eval);
		gsl_vector_free (Uty);

		}
		gsl_matrix_free (W);
		gsl_vector_free (y);
		gsl_matrix_free (UtX);
	}



	//BSLMM-DAP
	if (cPar.a_mode==14 || cPar.a_mode==15 || cPar.a_mode==16) {
	  if (cPar.a_mode==14) {
	    gsl_vector *y=gsl_vector_alloc (cPar.ni_test);
	    gsl_matrix *W=gsl_matrix_alloc (y->size, cPar.n_cvt);
	    gsl_matrix *G=gsl_matrix_alloc (y->size, y->size);
	    gsl_matrix *UtX=gsl_matrix_alloc (y->size, cPar.ns_test);

	    //set covariates matrix W and phenotype vector y
	    //an intercept should be included in W,
	    cPar.CopyCvtPhen (W, y, 0);

	    //center y, even for case/control data
	    cPar.pheno_mean=CenterVector(y);

	    //run bvsr if rho==1
	    if (cPar.rho_min==1 && cPar.rho_max==1) {
	      //read genotypes X (not UtX)
	      cPar.ReadGenotypes (UtX, G, false);

	      //perform BSLMM analysis
	      BSLMM cBslmm;
	      cBslmm.CopyFromParam(cPar);
	      time_start=clock();
	      cBslmm.MCMC(UtX, y);
	      cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	      cBslmm.CopyToParam(cPar);
	      //else, if rho!=1
	    } else {
	      gsl_matrix *U=gsl_matrix_alloc (y->size, y->size);
	      gsl_vector *eval=gsl_vector_alloc (y->size);
	      gsl_matrix *UtW=gsl_matrix_alloc (y->size, W->size2);
	      gsl_vector *Uty=gsl_vector_alloc (y->size);

	      //read relatedness matrix G
	      if (!(cPar.file_kin).empty()) {
		cPar.ReadGenotypes (UtX, G, false);

		//read relatedness matrix G
		ReadFile_kin (cPar.file_kin, cPar.indicator_idv, cPar.mapID2num, cPar.k_mode, cPar.error, G);
		if (cPar.error==true) {cout<<"error! fail to read kinship/relatedness file. "<<endl; return;}

		//center matrix G
		CenterMatrix (G);
	      } else {
		cPar.ReadGenotypes (UtX, G, true);
	      }

	      //eigen-decomposition and calculate trace_G
	      cout<<"Start Eigen-Decomposition..."<<endl;
	      time_start=clock();
	      cPar.trace_G=EigenDecomp (G, U, eval, 0);
	      cPar.trace_G=0.0;
	      for (size_t i=0; i<eval->size; i++) {
		if (gsl_vector_get (eval, i)<1e-10) {gsl_vector_set (eval, i, 0);}
		cPar.trace_G+=gsl_vector_get (eval, i);
	      }
	      cPar.trace_G/=(double)eval->size;
	      cPar.time_eigen=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

	      //calculate UtW and Uty
	      CalcUtX (U, W, UtW);
	      CalcUtX (U, y, Uty);

	      //calculate REMLE/MLE estimate and pve
	      CalcLambda ('L', eval, UtW, Uty, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_mle_null, cPar.logl_mle_H0);
	      CalcLambda ('R', eval, UtW, Uty, cPar.l_min, cPar.l_max, cPar.n_region, cPar.l_remle_null, cPar.logl_remle_H0);
	      CalcPve (eval, UtW, Uty, cPar.l_remle_null, cPar.trace_G, cPar.pve_null, cPar.pve_se_null);

	      cPar.PrintSummary();

	      //Creat and calcualte UtX, use a large memory
	      cout<<"Calculating UtX..."<<endl;
	      time_start=clock();
	      CalcUtX (U, UtX);
	      cPar.time_UtX=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);

	      //perform analysis; assume X and y are already centered
	      BSLMMDAP cBslmmDap;
	      cBslmmDap.CopyFromParam(cPar);
	      time_start=clock();
	      cBslmmDap.DAP_CalcBF (U, UtX, Uty, eval, y);
	      cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	      cBslmmDap.CopyToParam(cPar);

	      //release all matrices and vectors
	      gsl_matrix_free (G);
	      gsl_matrix_free (U);
	      gsl_matrix_free (UtW);
	      gsl_vector_free (eval);
	      gsl_vector_free (Uty);
	    }

	    gsl_matrix_free (W);
	    gsl_vector_free (y);
	    gsl_matrix_free (UtX);
	  } else if (cPar.a_mode==15) {
	    //perform EM algorithm and estimate parameters
	    vector<string> vec_rs;
	    vector<double> vec_sa2, vec_sb2, wab;
	    vector<vector<vector<double> > > BF;

	    //read hyp and bf files (functions defined in BSLMMDAP)
	    ReadFile_hyb (cPar.file_hyp, vec_sa2, vec_sb2, wab);
	    ReadFile_bf (cPar.file_bf, vec_rs, BF);

	    cPar.ns_test=vec_rs.size();
	    if (wab.size()!=BF[0][0].size()) {cout<<"error! hyp and bf files dimension do not match"<<endl;}

	    //load annotations
	    gsl_matrix *Ac;
	    gsl_matrix_int *Ad;
	    gsl_vector_int *dlevel;
	    size_t kc, kd;
	    if (!cPar.file_cat.empty()) {
	      ReadFile_cat (cPar.file_cat, vec_rs, Ac, Ad, dlevel, kc, kd);
	    } else {
	      kc=0; kd=0;
	    }
	    
	    cout<<"## number of blocks = "<<BF.size()<<endl;
	    cout<<"## number of analyzed SNPs = "<<vec_rs.size()<<endl;
	    cout<<"## grid size for hyperparameters = "<<wab.size()<<endl;
	    cout<<"## number of continuous annotations = "<<kc<<endl;
	    cout<<"## number of discrete annotations = "<<kd<<endl;

	    //DAP_EstimateHyper (const size_t kc, const size_t kd, const vector<string> &vec_rs, const vector<double> &vec_sa2, const vector<double> &vec_sb2, const vector<double> &wab, const vector<vector<vector<double> > > &BF, gsl_matrix *Ac, gsl_matrix_int *Ad, gsl_vector_int *dlevel);

	    //perform analysis
	    BSLMMDAP cBslmmDap;
	    cBslmmDap.CopyFromParam(cPar);
	    time_start=clock();
	    cBslmmDap.DAP_EstimateHyper (kc, kd, vec_rs, vec_sa2, vec_sb2, wab, BF, Ac, Ad, dlevel);
	    cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
	    cBslmmDap.CopyToParam(cPar);

	    gsl_matrix_free(Ac);
	    gsl_matrix_int_free(Ad);
	    gsl_vector_int_free(dlevel);
	  } else {
	    //
	  }

	}




	/*
	//LDR (change 14 to 16?)
	if (cPar.a_mode==14) {
		gsl_vector *y=gsl_vector_alloc (cPar.ni_test);
		gsl_matrix *W=gsl_matrix_alloc (y->size, cPar.n_cvt);
		gsl_matrix *G=gsl_matrix_alloc (1, 1);
		vector<vector<unsigned char> > Xt;

        	//set covariates matrix W and phenotype vector y
		//an intercept is included in W
		cPar.CopyCvtPhen (W, y, 0);

		//read in genotype matrix X
		cPar.ReadGenotypes (Xt, G, false);

		LDR cLdr;
		cLdr.CopyFromParam(cPar);
		time_start=clock();

		cLdr.VB(Xt, W, y);

		cPar.time_opt=(clock()-time_start)/(double(CLOCKS_PER_SEC)*60.0);
		cLdr.CopyToParam(cPar);

		gsl_vector_free (y);
		gsl_matrix_free (W);
		gsl_matrix_free (G);
	}
	*/

	cPar.time_total=(clock()-time_begin)/(double(CLOCKS_PER_SEC)*60.0);

	return;
}




void GEMMA::WriteLog (int argc, char ** argv, PARAM &cPar)
{
	string file_str;
	file_str=cPar.path_out+"/"+cPar.file_out;
	file_str+=".log.txt";

	ofstream outfile (file_str.c_str(), ofstream::out);
	if (!outfile) {cout<<"error writing log file: "<<file_str.c_str()<<endl; return;}

	outfile<<"##"<<endl;
	outfile<<"## GEMMA Version = "<<version<<endl;

	outfile<<"##"<<endl;
	outfile<<"## Command Line Input = ";
	for(int i = 0; i < argc; i++) {
		outfile<<argv[i]<<" ";
	}
	outfile<<endl;

	outfile<<"##"<<endl;
	time_t  rawtime;
	time(&rawtime);
	tm *ptm = localtime (&rawtime);

	outfile<<"## Date = "<<asctime(ptm);
	  //ptm->tm_year<<":"<<ptm->tm_month<<":"<<ptm->tm_day":"<<ptm->tm_hour<<":"<<ptm->tm_min<<endl;

	outfile<<"##"<<endl;
	outfile<<"## Summary Statistics:"<<endl;
	if (!cPar.file_cor.empty() || !cPar.file_study.empty() || !cPar.file_mstudy.empty() ) {
	  outfile<<"## number of total individuals in the sample = "<<cPar.ni_study<<endl;
	  outfile<<"## number of total individuals in the reference = "<<cPar.ni_ref<<endl;
	  //outfile<<"## number of total SNPs in the sample = "<<cPar.ns_study<<endl;
	  //outfile<<"## number of total SNPs in the reference panel = "<<cPar.ns_ref<<endl;
	  //outfile<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  //outfile<<"## number of analyzed SNP pairs = "<<cPar.ns_pair<<endl;
	  outfile<<"## number of variance components = "<<cPar.n_vc<<endl;

	  outfile<<"## pve estimates = ";
	    for (size_t i=0; i<cPar.v_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_pve[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(pve) = ";
	    for (size_t i=0; i<cPar.v_se_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_se_pve[i];
	    }
	    outfile<<endl;

	    if (cPar.n_vc>1) {
	      outfile<<"## total pve = "<<cPar.pve_total<<endl;
	      outfile<<"## se(total pve) = "<<cPar.se_pve_total<<endl;
	    }

	    outfile<<"## sigma2 per snp = ";
	    for (size_t i=0; i<cPar.v_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_sigma2[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(sigma2 per snp) = ";
	    for (size_t i=0; i<cPar.v_se_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_se_sigma2[i];
	    }
	    outfile<<endl;

	    outfile<<"## enrichment = ";
	    for (size_t i=0; i<cPar.v_enrich.size(); i++) {
	      outfile<<"  "<<cPar.v_enrich[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(enrichment) = ";
	    for (size_t i=0; i<cPar.v_se_enrich.size(); i++) {
	      outfile<<"  "<<cPar.v_se_enrich[i];
	    }
	    outfile<<endl;
	} else if (!cPar.file_beta.empty() && (cPar.a_mode==61 || cPar.a_mode==62) ) {
	  outfile<<"## number of total individuals in the sample = "<<cPar.ni_study<<endl;
	  outfile<<"## number of total individuals in the reference = "<<cPar.ni_total<<endl;
	  outfile<<"## number of total SNPs in the sample = "<<cPar.ns_study<<endl;
	  outfile<<"## number of total SNPs in the reference panel = "<<cPar.ns_total<<endl;
	  outfile<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  outfile<<"## number of variance components = "<<cPar.n_vc<<endl;
	} else if (!cPar.file_beta.empty() && (cPar.a_mode==66 || cPar.a_mode==67) ) {
	  outfile<<"## number of total individuals in the sample = "<<cPar.ni_total<<endl;
	  outfile<<"## number of total individuals in the reference = "<<cPar.ni_ref<<endl;
	  outfile<<"## number of total SNPs in the sample = "<<cPar.ns_total<<endl;
	  outfile<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  outfile<<"## number of variance components = "<<cPar.n_vc<<endl;

	  outfile<<"## pve estimates = ";
	    for (size_t i=0; i<cPar.v_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_pve[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(pve) = ";
	    for (size_t i=0; i<cPar.v_se_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_se_pve[i];
	    }
	    outfile<<endl;

	    if (cPar.n_vc>1) {
	      outfile<<"## total pve = "<<cPar.pve_total<<endl;
	      outfile<<"## se(total pve) = "<<cPar.se_pve_total<<endl;
	    }

	    outfile<<"## sigma2 per snp = ";
	    for (size_t i=0; i<cPar.v_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_sigma2[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(sigma2 per snp) = ";
	    for (size_t i=0; i<cPar.v_se_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_se_sigma2[i];
	    }
	    outfile<<endl;

	    outfile<<"## enrichment = ";
	    for (size_t i=0; i<cPar.v_enrich.size(); i++) {
	      outfile<<"  "<<cPar.v_enrich[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(enrichment) = ";
	    for (size_t i=0; i<cPar.v_se_enrich.size(); i++) {
	      outfile<<"  "<<cPar.v_se_enrich[i];
	    }
	    outfile<<endl;
	} else {
	  outfile<<"## number of total individuals = "<<cPar.ni_total<<endl;

	  if (cPar.a_mode==43) {
	    outfile<<"## number of analyzed individuals = "<<cPar.ni_cvt<<endl;
	    outfile<<"## number of individuals with full phenotypes = "<<cPar.ni_test<<endl;
	  } else if (cPar.a_mode!=27 && cPar.a_mode!=28) {
	    outfile<<"## number of analyzed individuals = "<<cPar.ni_test<<endl;
	  }

	  outfile<<"## number of covariates = "<<cPar.n_cvt<<endl;
	  outfile<<"## number of phenotypes = "<<cPar.n_ph<<endl;
	  if (cPar.a_mode==43) {
	    outfile<<"## number of observed data = "<<cPar.np_obs<<endl;
	    outfile<<"## number of missing data = "<<cPar.np_miss<<endl;
	  }
	  if (cPar.a_mode==25 || cPar.a_mode==26 || cPar.a_mode==27 || cPar.a_mode==28 || cPar.a_mode==61 || cPar.a_mode==62 || cPar.a_mode==63 || cPar.a_mode==66 || cPar.a_mode==67) {
	    outfile<<"## number of variance components = "<<cPar.n_vc<<endl;
	  }

	  if (!(cPar.file_gene).empty()) {
	    outfile<<"## number of total genes = "<<cPar.ng_total<<endl;
	    outfile<<"## number of analyzed genes = "<<cPar.ng_test<<endl;
	  } else if (cPar.file_epm.empty()) {
	    outfile<<"## number of total SNPs = "<<cPar.ns_total<<endl;
	    outfile<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  } else {
	    outfile<<"## number of analyzed SNPs = "<<cPar.ns_test<<endl;
	  }

	  if (cPar.a_mode==13) {
	    outfile<<"## number of cases = "<<cPar.ni_case<<endl;
	    outfile<<"## number of controls = "<<cPar.ni_control<<endl;
	  }
	}

	if ( (cPar.a_mode==61 || cPar.a_mode==62 || cPar.a_mode==63) && cPar.file_cor.empty() && cPar.file_study.empty() && cPar.file_mstudy.empty() ) {
	    //	        outfile<<"## REMLE log-likelihood in the null model = "<<cPar.logl_remle_H0<<endl;
	  if (cPar.n_ph==1) {
	    outfile<<"## pve estimates = ";
	    for (size_t i=0; i<cPar.v_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_pve[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(pve) = ";
	    for (size_t i=0; i<cPar.v_se_pve.size(); i++) {
	      outfile<<"  "<<cPar.v_se_pve[i];
	    }
	    outfile<<endl;

	    if (cPar.n_vc>1) {
	      outfile<<"## total pve = "<<cPar.pve_total<<endl;
	      outfile<<"## se(total pve) = "<<cPar.se_pve_total<<endl;
	    }

	    outfile<<"## sigma2 estimates = ";
	    for (size_t i=0; i<cPar.v_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_sigma2[i];
	    }
	    outfile<<endl;

	    outfile<<"## se(sigma2) = ";
	    for (size_t i=0; i<cPar.v_se_sigma2.size(); i++) {
	      outfile<<"  "<<cPar.v_se_sigma2[i];
	    }
	    outfile<<endl;

	    if (!cPar.file_beta.empty() ) {
	      outfile<<"## enrichment = ";
	      for (size_t i=0; i<cPar.v_enrich.size(); i++) {
		outfile<<"  "<<cPar.v_enrich[i];
	      }
	      outfile<<endl;

	      outfile<<"## se(enrichment) = ";
	      for (size_t i=0; i<cPar.v_se_enrich.size(); i++) {
		outfile<<"  "<<cPar.v_se_enrich[i];
	      }
	      outfile<<endl;
	    }
		  /*
			outfile<<"## beta estimate in the null model = ";
			for (size_t i=0; i<cPar.beta_remle_null.size(); i++) {
				outfile<<"  "<<cPar.beta_remle_null[i];
			}
			outfile<<endl;
			outfile<<"## se(beta) = ";
			for (size_t i=0; i<cPar.se_beta_remle_null.size(); i++) {
				outfile<<"  "<<cPar.se_beta_remle_null[i];
			}
			outfile<<endl;
		  */
	  }
	}

	if (cPar.a_mode==1 || cPar.a_mode==2 || cPar.a_mode==3 || cPar.a_mode==4 || cPar.a_mode==5 || cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13) {
		outfile<<"## REMLE log-likelihood in the null model = "<<cPar.logl_remle_H0<<endl;
		outfile<<"## MLE log-likelihood in the null model = "<<cPar.logl_mle_H0<<endl;
		if (cPar.n_ph==1) {
			//outfile<<"## lambda REMLE estimate in the null (linear mixed) model = "<<cPar.l_remle_null<<endl;
			//outfile<<"## lambda MLE estimate in the null (linear mixed) model = "<<cPar.l_mle_null<<endl;
			outfile<<"## pve estimate in the null model = "<<cPar.pve_null<<endl;
			outfile<<"## se(pve) in the null model = "<<cPar.pve_se_null<<endl;
			outfile<<"## vg estimate in the null model = "<<cPar.vg_remle_null<<endl;
			outfile<<"## ve estimate in the null model = "<<cPar.ve_remle_null<<endl;
			outfile<<"## beta estimate in the null model = ";
			for (size_t i=0; i<cPar.beta_remle_null.size(); i++) {
				outfile<<"  "<<cPar.beta_remle_null[i];
			}
			outfile<<endl;
			outfile<<"## se(beta) = ";
			for (size_t i=0; i<cPar.se_beta_remle_null.size(); i++) {
				outfile<<"  "<<cPar.se_beta_remle_null[i];
			}
			outfile<<endl;

		} else {
			size_t c;
			outfile<<"## REMLE estimate for Vg in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Vg_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## se(Vg): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<sqrt(cPar.VVg_remle_null[c])<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## REMLE estimate for Ve in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Ve_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## se(Ve): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<sqrt(cPar.VVe_remle_null[c])<<"\t";
				}
				outfile<<endl;
			}

			outfile<<"## MLE estimate for Vg in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<cPar.n_ph; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Vg_mle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## se(Vg): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<sqrt(cPar.VVg_mle_null[c])<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## MLE estimate for Ve in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<cPar.n_ph; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Ve_mle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## se(Ve): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<sqrt(cPar.VVe_mle_null[c])<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## estimate for B (d by c) in the null model (columns correspond to the covariates provided in the file): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<cPar.n_cvt; j++) {
					c=i*cPar.n_cvt+j;
					outfile<<cPar.beta_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## se(B): "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<cPar.n_cvt; j++) {
					c=i*cPar.n_cvt+j;
					outfile<<cPar.se_beta_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
		}
	}

	/*
	if (cPar.a_mode==1 || cPar.a_mode==2 || cPar.a_mode==3 || cPar.a_mode==4 || cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13) {
		if (cPar.n_ph==1) {
			outfile<<"## REMLE vg estimate in the null model = "<<cPar.vg_remle_null<<endl;
			outfile<<"## REMLE ve estimate in the null model = "<<cPar.ve_remle_null<<endl;
		} else {
			size_t c;
			outfile<<"## REMLE estimate for Vg in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Vg_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
			outfile<<"## REMLE estimate for Ve in the null model: "<<endl;
			for (size_t i=0; i<cPar.n_ph; i++) {
				for (size_t j=0; j<=i; j++) {
					c=(2*cPar.n_ph-min(i,j)+1)*min(i,j)/2+max(i,j)-min(i,j);
					outfile<<cPar.Ve_remle_null[c]<<"\t";
				}
				outfile<<endl;
			}
		}
	}
	 */


	if (cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13 || cPar.a_mode==14 || cPar.a_mode==16) {
	  outfile<<"## estimated mean = "<<cPar.pheno_mean<<endl;
	}

	if (cPar.a_mode==11 || cPar.a_mode==13) {
		outfile<<"##"<<endl;
		outfile<<"## MCMC related:"<<endl;
		outfile<<"## initial value of h = "<<cPar.cHyp_initial.h<<endl;
		outfile<<"## initial value of rho = "<<cPar.cHyp_initial.rho<<endl;
		outfile<<"## initial value of pi = "<<exp(cPar.cHyp_initial.logp)<<endl;
		outfile<<"## initial value of |gamma| = "<<cPar.cHyp_initial.n_gamma<<endl;
		outfile<<"## random seed = "<<cPar.randseed<<endl;
		outfile<<"## acceptance ratio = "<<(double)cPar.n_accept/(double)((cPar.w_step+cPar.s_step)*cPar.n_mh)<<endl;
	}

	outfile<<"##"<<endl;
	outfile<<"## Computation Time:"<<endl;
	outfile<<"## total computation time = "<<cPar.time_total<<" min "<<endl;
	outfile<<"## computation time break down: "<<endl;
	if (cPar.a_mode==21 || cPar.a_mode==22 || cPar.a_mode==11 || cPar.a_mode==13 || cPar.a_mode==14 || cPar.a_mode==16) {
		outfile<<"##      time on calculating relatedness matrix = "<<cPar.time_G<<" min "<<endl;
	}
	if (cPar.a_mode==31) {
		outfile<<"##      time on eigen-decomposition = "<<cPar.time_eigen<<" min "<<endl;
	}
	if (cPar.a_mode==1 || cPar.a_mode==2 || cPar.a_mode==3 || cPar.a_mode==4 || cPar.a_mode==5 || cPar.a_mode==11 || cPar.a_mode==12 || cPar.a_mode==13 || cPar.a_mode==14 || cPar.a_mode==16) {
		outfile<<"##      time on eigen-decomposition = "<<cPar.time_eigen<<" min "<<endl;
		outfile<<"##      time on calculating UtX = "<<cPar.time_UtX<<" min "<<endl;
	}
	if ((cPar.a_mode>=1 && cPar.a_mode<=4) || (cPar.a_mode>=51 && cPar.a_mode<=54) ) {
		outfile<<"##      time on optimization = "<<cPar.time_opt<<" min "<<endl;
	}
	if (cPar.a_mode==11 || cPar.a_mode==13) {
		outfile<<"##      time on proposal = "<<cPar.time_Proposal<<" min "<<endl;
		outfile<<"##      time on mcmc = "<<cPar.time_opt<<" min "<<endl;
		outfile<<"##      time on Omega = "<<cPar.time_Omega<<" min "<<endl;
	}
	if (cPar.a_mode==41 || cPar.a_mode==42) {
		outfile<<"##      time on eigen-decomposition = "<<cPar.time_eigen<<" min "<<endl;
	}
	if (cPar.a_mode==43) {
		outfile<<"##      time on eigen-decomposition = "<<cPar.time_eigen<<" min "<<endl;
		outfile<<"##      time on predicting phenotypes = "<<cPar.time_opt<<" min "<<endl;
	}
	outfile<<"##"<<endl;

	outfile.close();
	outfile.clear();
	return;
}


