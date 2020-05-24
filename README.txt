PROGRAM: GEMMA with Population Linkage

AUTHOR: Xiang Zhou, Gregory Zajac

YEAR: 2012, 2020

DESCRIPTION: This program is largely based on the GEMMA v0.96 
(Genome-wide Efficient Mixed Model Association) 
<https://github.com/genetics-statistics/GEMMA> package by Xiang Zhou but 
has been modified to implement the Population Linkage method developed by 
Gregory Zajac and Gonçalo Abecasis at the University of Michigan for 
performing genome-wide linkage analysis on a population cohort. This
implementation expects kinship and IBD inferred in KING
<http://people.virginia.edu/~wc9c/KING/manual.html>

This repository also contains two helpful tools k_cov_pheno for simulating
a phenotype with covariance from a kinship matrix and extract_chr_ends to
make a kinship matrix based on the the ends of chromosomes, and an R script
to make a LOD plot and other plots.

USAGE: all GEMMA usage applies. To run Population Linkage:
```
./gemma \
    -bfile [prefix] \
    -k [filename] \
    -segments [KING IBD segments.gz filename] \
    -km [num] -n [num] -vc 1 \
    -outdir [output directory] \
    -o [output prefix]
```

replace -k with -mk to use multiple kinship files, -king for a KING kinship 
file, or -seg for a KING seg file. Optionally, set -max-link-tests [num] to
limit the number of tests and speed up the analysis.

To combine multiple kinship files based on a VC fit for a phenotype:
```
./gemma \
    -bfile [prefix] \
    -mk [filename] \
    -combine-mats -km [num] -n [num] -vc 1 \
    -outdir [output directory] \
    -o [output prefix]
```

DOCUMENTATION: A detailed user manual (for GEMMA, not Population Linkage) 
is available online and a recent version is provided inside the doc 
directory. A brief description is also available if you type "./gemma -h". 
Please refer to the online user manual for updated information.

LICENSE: Distributed under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

INSTALLATION: An executable binary (x86 64bit Linux) is available in
the bin directory. No installation is necessary if the executable
works in your system. Otherwise, see compilation section below.

COMPILATION: You will need a standard C/C++ compiler such as GNU gcc,
as well as GSL, LAPACK libraries. You will need to change the library
paths in the Makefile accordingly.

For details on installing GSL library, please refer to:
http://www.gnu.org/s/gsl/

For details on installing LAPACK library, please refer to:
http://www.netlib.org/lapack/

Xiang Zhou, December 2012
Gregory Zajac, May 2020
