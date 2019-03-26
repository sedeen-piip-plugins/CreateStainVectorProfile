/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

#ifndef SEDEEN_SRC_PLUGINS_STAINVECTORSFROMROI_STAINVECTORSFROMROI_H
#define SEDEEN_SRC_PLUGINS_STAINVECTORSFROMROI_STAINVECTORSFROMROI_H

 // DPTK headers - a minimal set 
#include "algorithm/AlgorithmBase.h"
#include "algorithm/Parameters.h"
#include "algorithm/Results.h"

#include <omp.h>
#include <Windows.h>
#include <fstream>

namespace sedeen {
	namespace tile {

	} // namespace tile

	namespace algorithm {

#define round(x) ( x >= 0.0f ? floor(x + 0.5f) : ceil(x - 0.5f) )

		class StainVectorsFromROI : public algorithm::AlgorithmBase {
		public:
			StainVectorsFromROI();

		private:
			// virtual function
			virtual void run();
			virtual void init(const image::ImageHandle& image);







		}
	}
}