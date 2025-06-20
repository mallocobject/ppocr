#include <opencv2/imgcodecs.hpp>

#include <include/args.h>
#include <include/paddlestructure.h>

#include <iostream>
#include <vector>
#include <fstream>

using namespace PaddleOCR;

void check_params()
{
	if (FLAGS_det)
	{
		if (FLAGS_det_model_dir.empty() || FLAGS_image_dir.empty())
		{
			std::cout << "Usage[det]: ./ppocr "
				"--det_model_dir=/PATH/TO/DET_INFERENCE_MODEL/ "
				<< "--image_dir=/PATH/TO/INPUT/IMAGE/" << std::endl;
			exit(1);
		}
	}
	if (FLAGS_rec)
	{
		std::cout
			<< "In PP-OCRv3, rec_image_shape parameter defaults to '3, 48, 320',"
			"if you are using recognition model with PP-OCRv2 or an older "
			"version, "
			"please set --rec_image_shape='3,32,320"
			<< std::endl;
		if (FLAGS_rec_model_dir.empty() || FLAGS_image_dir.empty())
		{
			std::cout << "Usage[rec]: ./ppocr "
				"--rec_model_dir=/PATH/TO/REC_INFERENCE_MODEL/ "
				<< "--image_dir=/PATH/TO/INPUT/IMAGE/" << std::endl;
			exit(1);
		}
	}
	if (FLAGS_cls && FLAGS_use_angle_cls)
	{
		if (FLAGS_cls_model_dir.empty() || FLAGS_image_dir.empty())
		{
			std::cout << "Usage[cls]: ./ppocr "
				<< "--cls_model_dir=/PATH/TO/REC_INFERENCE_MODEL/ "
				<< "--image_dir=/PATH/TO/INPUT/IMAGE/" << std::endl;
			exit(1);
		}
	}
	if (FLAGS_table)
	{
		if (FLAGS_table_model_dir.empty() || FLAGS_det_model_dir.empty() ||
			FLAGS_rec_model_dir.empty() || FLAGS_image_dir.empty())
		{
			std::cout << "Usage[table]: ./ppocr "
				<< "--det_model_dir=/PATH/TO/DET_INFERENCE_MODEL/ "
				<< "--rec_model_dir=/PATH/TO/REC_INFERENCE_MODEL/ "
				<< "--table_model_dir=/PATH/TO/TABLE_INFERENCE_MODEL/ "
				<< "--image_dir=/PATH/TO/INPUT/IMAGE/" << std::endl;
			exit(1);
		}
	}
	if (FLAGS_layout)
	{
		if (FLAGS_layout_model_dir.empty() || FLAGS_image_dir.empty())
		{
			std::cout << "Usage[layout]: ./ppocr "
				<< "--layout_model_dir=/PATH/TO/LAYOUT_INFERENCE_MODEL/ "
				<< "--image_dir=/PATH/TO/INPUT/IMAGE/" << std::endl;
			exit(1);
		}
	}
	if (FLAGS_precision != "fp32" && FLAGS_precision != "fp16" &&
		FLAGS_precision != "int8")
	{
		std::cout << "precision should be 'fp32'(default), 'fp16' or 'int8'. "
			<< std::endl;
		exit(1);
	}
}

void ocr(std::vector<cv::String>& cv_all_img_names)
{
	PPOCR ocr;

	if (FLAGS_benchmark)
	{
		ocr.reset_timer();
	}

	std::vector<cv::Mat> img_list;
	std::vector<cv::String> img_names;
	for (int i = 0; i < cv_all_img_names.size(); ++i)
	{
		cv::Mat img = cv::imread(cv_all_img_names[i], cv::IMREAD_COLOR);
		if (!img.data)
		{
			std::cerr << "[ERROR] image read failed! image path: "
				<< cv_all_img_names[i] << std::endl;
			continue;
		}
		img_list.emplace_back(std::move(img));
		img_names.emplace_back(cv_all_img_names[i]);
	}

	std::vector<std::vector<OCRPredictResult>> ocr_results =
		ocr.ocr(img_list, FLAGS_det, FLAGS_rec, FLAGS_cls);

	for (int i = 0; i < img_names.size(); ++i)
	{
		std::cout << "predict img: " << cv_all_img_names[i] << std::endl;
		Utility::print_result(ocr_results[i]);
		if (FLAGS_visualize && FLAGS_det)
		{
			std::string file_name = Utility::basename(img_names[i]);
			cv::Mat srcimg = img_list[i];
			Utility::VisualizeBboxes(srcimg, ocr_results[i],
				FLAGS_output + "/" + file_name);
		}
	}
	if (FLAGS_benchmark)
	{
		ocr.benchmark_log(cv_all_img_names.size());
	}
}

void structure(std::vector<cv::String>& cv_all_img_names)
{
	PaddleOCR::PaddleStructure engine;

	if (FLAGS_benchmark)
	{
		engine.reset_timer();
	}

	for (int i = 0; i < cv_all_img_names.size(); ++i)
	{
		std::cout << "predict img: " << cv_all_img_names[i] << std::endl;
		cv::Mat img = cv::imread(cv_all_img_names[i], cv::IMREAD_COLOR);
		if (!img.data)
		{
			std::cerr << "[ERROR] image read failed! image path: "
				<< cv_all_img_names[i] << std::endl;
			continue;
		}

		std::vector<StructurePredictResult> structure_results = engine.structure(
			img, FLAGS_layout, FLAGS_table, FLAGS_det && FLAGS_rec);

		for (size_t j = 0; j < structure_results.size(); ++j)
		{
			std::cout << j << "\ttype: " << structure_results[j].type
				<< ", region: [";
			std::cout << structure_results[j].box[0] << ","
				<< structure_results[j].box[1] << ","
				<< structure_results[j].box[2] << ","
				<< structure_results[j].box[3] << "], score: ";
			std::cout << structure_results[j].confidence << ", res: ";

			if (structure_results[j].type == "table")
			{
				//std::cout << structure_results[j].html << std::endl;
				std::string file_name = Utility::basename(cv_all_img_names[i]);
				std::string html_filename = FLAGS_output + "/" + file_name + "_" + std::to_string(j) + ".html";

				std::ofstream html_file(html_filename);
				if (html_file.is_open())
				{
					int index = structure_results[j].html.find_first_of("<body>");
					html_file << R"(<!DOCTYPE html>
										<html lang="en">
										<head>
											<meta charset="UTF-8">
											<meta name="viewport" content="width=device-width, initial-scale=1.0">
											<title>Table</title>
											<style>
												table {
													border-collapse: collapse; /* 合并边框 */
													width: 100%;             /* 表格宽度自适应 */
													margin: 20px 0;          /* 上下外边距 */
												}

												td, th {
													border: 1px solid #ddd;  /* 单元格边框 */
													padding: 12px;           /* 内边距 */
													text-align: left;        /* 左对齐 */
												}

												th {
													background-color: #f5f5f5;/* 表头背景色 */
													font-weight: bold;       /* 加粗 */
												}

												tr:nth-child(even) {
													background-color: #fafafa;/* 隔行变色 */
												}

												.highlight {
													background-color: yellow;/* 高亮行 */
												}
											</style>
										</head>
										)"
						<< structure_results[j].html.substr(index) << std::endl;
					html_file.close();
					std::cout << "HTML saved to: " << html_filename << std::endl;
				}
				else
				{
					std::cerr << "Error: Failed to open file " << html_filename << std::endl;
				}

				if (structure_results[j].cell_box.size() > 0 && FLAGS_visualize)
				{
					//std::string file_name = Utility::basename(cv_all_img_names[i]);

					Utility::VisualizeBboxes(img, structure_results[j],
						FLAGS_output + "/" + std::to_string(j) +
						"_" + file_name);
				}
			}
			else
			{
				std::cout << "count of ocr result is : "
					<< structure_results[j].text_res.size() << std::endl;
				if (structure_results[j].text_res.size() > 0)
				{
					std::cout << "********** print ocr result "
						<< "**********" << std::endl;
					Utility::print_result(structure_results[j].text_res);
					std::cout << "********** end print ocr result "
						<< "**********" << std::endl;
				}
			}
		}
	}
	if (FLAGS_benchmark)
	{
		engine.benchmark_log(cv_all_img_names.size());
	}
}