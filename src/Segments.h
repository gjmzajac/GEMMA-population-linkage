#ifndef __SEGMENTS_H__
#define __SEGMENTS_H__

// #include <vector>
// #include <bool>
#include <cstdint>

using namespace std;

class Segments {
	public:
		uint32_t pos = 0;
		uint32_t id1 = 0;
		uint32_t id2 = 0;
		uint8_t ibd = 0;
// 		vector<size_t> id1;
// 		vector<size_t> id2;
// 		vector<double> ibd;
// 		int i=0;
		
		// Segments();
		
// 		bool add_seg(size_t n_id1, size_t n_id2, double n_ibd);
// 		bool get_seg(size_t *n_id1, size_t *n_id2, double *n_ibd);
};

bool comp_segs (Segments a, Segments b);

#endif
