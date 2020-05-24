#include <iostream>
#include <fstream>
// #include <sstream>
#include <string>
// #include <iomanip>
// #include <bitset>
// #include <vector>
#include <map>
#include <unordered_map>
// #include <set>
#include <cstring>
// #include <cmath>
// #include <stdio.h>
// #include <stdlib.h>

#include "gzstream.h"

using namespace std;

/* Compilation: 
g++ -Wall -Weffc++ -O3 -std=gnu++11 -m64 -static \
    extract_chr_ends.cpp \
    ../src/gzstream.o \
    -I../src \
    -lz -o extract_chr_ends

/* Usage:
./extract_chr_ends \
	//argv[1] fam
	//argv[2] allsegs.txt
	//argv[3] segments.gz
	//argv[4] Mb
	//argv[5] out_f
*/

void fam (
	const string &file_fam,
	bool &error,
	ogzstream &outfile);

void allsegs (
	const string &file_allsegs,
	bool &error,
	int &tot_reg,
	map <int, double> &chr_start, 
	map <int, double> &chr_end);

void segments (
	const string &file_segments, 
	bool &error, 
	const double tot_reg,
	const map <int, double> &chr_start, 
	const map <int, double> &chr_end, 
	const double extract_Mb,
	ogzstream &outfile);

int main(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++){
		cout << i << "\t" << argv[i] << endl;
	}
	
	bool error = false;
	int tot_reg = -1;
	map <int, double> chr_start;
	map <int, double> chr_end;
	const double extract_Mb = atof(argv[4]);
	ogzstream outfile (argv[5]);

	fam(argv[1], error, outfile);
	allsegs(argv[2], error, tot_reg, chr_start, chr_end);
//debug
// for(auto &ent1 : chr_start)
// cout << ent1.first << "\t" << ent1.second << "\n";
// for(auto &ent1 : chr_end)
// cout << ent1.first << "\t" << ent1.second << "\n";
//end debug
	segments(argv[3], error, tot_reg, chr_start, chr_end, extract_Mb, outfile);

	outfile.close();
	outfile.clear();

	return 0;
}

void fam (
	const string &file_fam,
	bool &error,
	ogzstream &outfile)
{
	string line;
	char *ch_ptr;
	string iid;
	
	igzstream infile (file_fam.c_str(), igzstream::in);
	if (!infile) {cout<<"error! fail to open fam file: "<<file_fam<<endl; error=true; return;}
	
	while (getline(infile, line)) {
		ch_ptr=strtok ((char *)line.c_str(), " , \t");
		ch_ptr=strtok (NULL, " , \t"); //skip fid
		iid=ch_ptr;

		outfile << iid << " " << iid << " " << 1 << "\n";
	}

	infile.close();
	infile.clear();

	return;
}

void allsegs (
	const string &file_allsegs,
	bool &error,
	int &tot_reg,
	map <int, double> &chr_start, 
	map <int, double> &chr_end)
{
	igzstream infile (file_allsegs.c_str(), igzstream::in);
	if (!infile) {cout<<"error! fail to open KING allsegs.txt file: "<<file_allsegs<<endl; error=true; return;}
	
	string line;
	char *ch_ptr;
	int chr;
	double StartMB;
	double StopMB;
	
	// Segment	Chr	StartMB	StopMB	Length	N_SNP	StartSNP	StopSNP
	//skip the header row
	string header;
	getline(infile, header);

	while (getline(infile, line)) {
		ch_ptr=strtok ((char *)line.c_str(), " , \t");
		ch_ptr=strtok (NULL, " , \t"); //skip segment #
		chr=atoi(ch_ptr);
		ch_ptr=strtok (NULL, " , \t");
		StartMB=atof(ch_ptr);
		ch_ptr=strtok (NULL, " , \t");
		StopMB=atof(ch_ptr);
		
		if (chr_start.find(chr) == chr_start.end())
			chr_start[chr] = StartMB;
		else if (chr_start[chr] > StartMB)
			chr_start[chr] = StartMB;
		
		if (chr_end.find(chr) == chr_end.end())
			chr_end[chr] = StopMB;
		else if (chr_end[chr] < StopMB)
			chr_end[chr] = StopMB;
	}
	
	infile.close();
	infile.clear();

	if (chr_start.size() == chr_end.size())
		tot_reg = 4 * chr_start.size();
	else {
		cout << "error! in KING allsegs.txt file: " << file_allsegs << endl;
		error = true;
	}
	
	return;
}

void segments (
	const string &file_segments, 
	bool &error, 
	const double tot_reg,
	const map <int, double> &chr_start, 
	const map <int, double> &chr_end, 
	const double extract_Mb,
	ogzstream &outfile)
{
	igzstream infile (file_segments.c_str(), igzstream::in);
	if (!infile) {cout<<"error! fail to open KING segments.gz file: "<<file_segments<<endl; error=true; return;}

	string line;
	char *ch_ptr;
	uint8_t ibd;
	// const uint8_t zero = 0;

	string id1, id2;
	// double Cov_d;
	// uint32_t n_id1, n_id2;
	int chr;
	double StartMB, StopMB;
	// int StartMB, StopMB;
	// string StartSNP, StopSNP;
	// uint32_t StartPos, StopPos;

	unordered_map <string, double> pair_t_ibd;
	
	//skip the header row
	string header;
	getline(infile, header);

	while (getline(infile, line)) {
		ch_ptr=strtok ((char *)line.c_str(), " , \t");
		ch_ptr=strtok (NULL, " , \t"); //skip fid1
		id1=ch_ptr;
		ch_ptr=strtok (NULL, " , \t"); //skip fid2
		ch_ptr=strtok (NULL, " , \t");
		id2=ch_ptr;
		ch_ptr=strtok (NULL, " , \t");
		ibd=atof(ch_ptr+3); //+3 to skip the characters "I" "B" "D" and just get the number following
		ch_ptr=strtok (NULL, " , \t");
		chr=atoi(ch_ptr);
		ch_ptr=strtok (NULL, " , \t"); //skip StartMB
		StartMB=atof(ch_ptr);
		ch_ptr=strtok (NULL, " , \t"); //skip StopMb
		StopMB=atof(ch_ptr);
		// ch_ptr=strtok (NULL, " , \t");
		// StartSNP=ch_ptr;
		// ch_ptr=strtok (NULL, " , \t");
		// StopSNP=ch_ptr;
		if (StartMB < chr_start.at(chr) + extract_Mb || chr_end.at(chr) - extract_Mb < StopMB) {
			pair_t_ibd[id1 + " " + id2] += ibd;
//debug
// cout << "chr: " << chr << " ";
// cout << "StartMB: " << StartMB << " ";
// cout << "chr_start.at(chr): " << chr_start.at(chr) << " ";
// cout << "extract_Mb: " << extract_Mb << " ";
// cout << "chr_end.at(chr): " << chr_end.at(chr) << " ";
// cout << "StopMB: " << StopMB << "\n";
// cout << line << "\n";
//end debug
		}
	}
	
	infile.close();
	infile.clear();

	//loop over the map
	for(auto &ent1 : pair_t_ibd)
		outfile << ent1.first << " " << ent1.second / tot_reg << "\n";
	
	return;
}
