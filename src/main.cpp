//// copyright (c) 2020 paddlepaddle authors. all rights reserved.
////
//// licensed under the apache license, version 2.0 (the "license");
//// you may not use this file except in compliance with the license.
//// you may obtain a copy of the license at
////
////     http://www.apache.org/licenses/license-2.0
////
//// unless required by applicable law or agreed to in writing, software
//// distributed under the license is distributed on an "as is" basis,
//// without warranties or conditions of any kind, either express or implied.
//// see the license for the specific language governing permissions and
//// limitations under the license.
//
//#include <iostream>
//#include <include/args.h>
//#include <include/paddlestructure.h>
//#include "include/func_declare.h"
//#include <include/args.h>
//#include <include/paddlestructure.h>
//#include <opencv2/imgcodecs.hpp>
//
//using namespace PaddleOCR;
//
//int main(int argc, char** argv)
//{
//	if (!Utility::PathExists(FLAGS_image_dir))
//	{
//		std::cerr << "[ERROR] image path not exist! image_dir: " << FLAGS_image_dir
//			<< std::endl;
//		exit(1);
//	}
//
//	std::vector<cv::String> cv_all_img_names;
//	cv::glob(FLAGS_image_dir, cv_all_img_names);
//	std::cout << "total images num: " << cv_all_img_names.size() << std::endl;
//
//	if (!Utility::PathExists(FLAGS_output))
//	{
//		Utility::CreateDir(FLAGS_output);
//	}
//	if (FLAGS_type == "ocr")
//	{
//		ocr(cv_all_img_names);
//	}
//	else if (FLAGS_type == "structure")
//	{
//		structure(cv_all_img_names);
//	}
//	else
//	{
//		std::cout << "only value in ['ocr','structure'] is supported" << std::endl;
//	}
//}
