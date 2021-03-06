//
// Created by sergio on 16/05/19.
//

#include "../../include/Model.h"
#include "../../include/Tensor.h"
#include <opencv2/opencv.hpp>
#include <numeric>
#include <iomanip>

int main() {
    Model model("../ssd_inception/frozen_inference_graph.pb");
    auto outNames1 = new Tensor(model, "num_detections");
    auto outNames2 = new Tensor(model, "detection_scores");
    auto outNames3 = new Tensor(model, "detection_boxes");
    auto outNames4 = new Tensor(model, "detection_classes");

    auto inpName = new Tensor(model, "image_tensor");


    // Read image
    cv::Mat img, inp;
    img = cv::imread("../test.jpg", CV_LOAD_IMAGE_COLOR);

    int rows = img.rows;
    int cols = img.cols;

    cv::resize(img, inp, cv::Size(300, 300));
    cv::cvtColor(inp, inp, CV_BGR2RGB);

    // Put image in Tensor
    std::vector<uint8_t > img_data;
    img_data.assign(inp.data, inp.data + inp.total() * inp.channels());
    inpName->set_data(img_data, {1, 300, 300, 3});

    model.run(inpName, {outNames1, outNames2, outNames3, outNames4});

    // Visualize detected bounding boxes.
    int num_detections = (int)outNames1->get_data<float>()[0];
    for (int i=0; i<num_detections; i++) {
        int classId = (int)outNames4->get_data<float>()[i];
        float score = outNames2->get_data<float>()[i];
        auto bbox_data = outNames3->get_data<float>();
        std::vector<float> bbox = {bbox_data[i*4], bbox_data[i*4+1], bbox_data[i*4+2], bbox_data[i*4+3]};
        if (score > 0.3) {
            float x = bbox[1] * cols;
            float y = bbox[0] * rows;
            float right = bbox[3] * cols;
            float bottom = bbox[2] * rows;

            cv::rectangle(img, {(int)x, (int)y}, {(int)right, (int)bottom}, {125, 255, 51}, 2);
        }
    }

    cv::imshow("Image", img);
    cv::waitKey(0);
}