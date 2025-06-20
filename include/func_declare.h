#pragma once

#include <vector>
#include <opencv2/imgcodecs.hpp>

void check_params();
void ocr(std::vector<cv::String>& cv_all_img_names);
void structure(std::vector<cv::String>& cv_all_img_names);