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
//#include "ColorDeconvolutionKernel.h"

#include <omp.h>
#include <Windows.h>
#include <fstream>

namespace sedeen {
namespace tile {

} // namespace tile

namespace algorithm {
#define round(x) ( x >= 0.0f ? floor(x + 0.5f) : ceil(x - 0.5f) )

///Stain Vectors From ROI
///This plugin creates a way for the user to define a combination
///of stain vectors to be used in the Stain Analysis plugin
class StainVectorsFromROI : public algorithm::AlgorithmBase {
public:
    StainVectorsFromROI();
    virtual ~StainVectorsFromROI();

private:
    // virtual function
    virtual void run();
    virtual void init(const image::ImageHandle& image);



    //Member methods
    std::string openFile(std::string path);

private:
    //Member parameters
    TextFieldParameter m_nameOfStainProfile;




//I don't know how many of these will end up being used.
    //std::string m_path_to_root;
    //std::string m_path_to_stainfile;

    //algorithm::DisplayAreaParameter m_display_area;
    //algorithm::OptionParameter m_retainment;
    //algorithm::OptionParameter m_displayOptions;
    /// Parameter for selecting threshold retainment 
    //algorithm::OptionParameter m_behavior;
    /// User defined Threshold value.
    //algorithm::DoubleParameter m_threshold;
    /// The output result
    //algorithm::ImageResult m_result;
    //algorithm::TextResult m_output_text;
    //std::string m_report;
    /// Parameter for selecting which of the intermediate result to display
    //algorithm::OptionParameter m_output_option;
    /// User region of interest
    //std::vector<algorithm::GraphicItemParameter> m_region_interest;
    //algorithm::GraphicItemParameter m_region_toProcess;

    /// The intermediate image factory after color deconvolution
    //std::shared_ptr<image::tile::Factory> m_colorDeconvolution_factory;

    /// The intermediate image factory after thresholding
    //std::shared_ptr<image::tile::Factory> m_threshold_factory;
    //std::ofstream log_file;


};

} // namespace algorithm
} // namespace sedeen

#endif