#include "Segments.h"

using namespace std;

// bool Segments::add_seg(size_t n_id1, size_t n_id2, double n_ibd){
// 	id1.push_back(n_id1);
// 	id2.push_back(n_id2);
// 	ibd.push_back(n_ibd);
// 
// 	return true;
// }
// 
// bool Segments::get_seg(size_t *n_id1, size_t *n_id2, double *n_ibd){
// 	// if (i > id1.size() || i > id2.size() || i > ibd.size())
// 	if (i >= id1.size() || i >= id2.size() || i >= ibd.size())
// 		return false;
// 	else {
// 		*n_id1 = id1.at(i);
// 		*n_id2 = id2.at(i);
// 		*n_ibd = ibd.at(i);
// 		i++;
// 		return true;
// 	}
// }

bool comp_segs (Segments a, Segments b) {return (a.pos < b.pos); }
