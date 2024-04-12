#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include <functional>

#include "typecvt.hpp"
#include "MoonRegistrate_pywrapper.hpp"

#include "MoonRegistration/MoonRegistrate.hpp"


std::unique_ptr<mr::MoonRegistrar> new_MoonRegistrar(
    const py::object& user_data,
    const py::object& model_data,
    const mr::RegistrationAlgorithms& algorithm
)
{
    // https://stackoverflow.com/a/8142943
    // https://stackoverflow.com/a/62310838
    
    // input bytes
    if (py::isinstance<py::bytes>(user_data) && py::isinstance<py::bytes>(model_data))
    {
        std::string user_val = py::cast<std::string>(user_data);
        std::string model_val = py::cast<std::string>(model_data);
        return std::unique_ptr<mr::MoonRegistrar>(
            new mr::MoonRegistrar(
                std::vector<unsigned char>(user_val.begin(), user_val.end()),
                std::vector<unsigned char>(model_val.begin(), model_val.end()),
                algorithm
            )
        );
    }
    // input str
    else if (py::isinstance<py::str>(user_data) && py::isinstance<py::str>(model_data))
    {
        std::string user_val = py::cast<std::string>(user_data);
        std::string model_val = py::cast<std::string>(model_data);
        return std::unique_ptr<mr::MoonRegistrar>(
            new mr::MoonRegistrar(
                user_val,
                model_val,
                algorithm
            )
        );
    }
    // if data is not None, assume data is a numpy.ndarray (cv2.MatLike)
    else if (
        !py::isinstance<py::none>(user_data) && py::isinstance<py::array>(user_data) &&
        !py::isinstance<py::none>(model_data) && py::isinstance<py::array>(model_data)
    )
    {
        cv::Mat user_image = py::cast<cv::Mat>(user_data);
        cv::Mat model_image = py::cast<cv::Mat>(model_data);
        return std::unique_ptr<mr::MoonRegistrar>(
            new mr::MoonRegistrar(
                user_image,
                model_image,
                algorithm
            )
        );
    }
    
    throw std::runtime_error("Cannot create mr::MoonRegistrar with given parameter");
}

void wrap_MoonRegistrar_update_images(mr::MoonRegistrar& self, const py::object& user_data, const py::object& model_data)
{
    // input bytes
    if (py::isinstance<py::bytes>(user_data) && py::isinstance<py::bytes>(model_data))
    {
        std::string user_val = py::cast<std::string>(user_data);
        std::string model_val = py::cast<std::string>(model_data);
        self.update_images(
            std::vector<unsigned char>(user_val.begin(), user_val.end()),
            std::vector<unsigned char>(model_val.begin(), model_val.end())
        );
    }
    // input str
    else if (py::isinstance<py::str>(user_data) && py::isinstance<py::str>(model_data))
    {
        std::string user_val = py::cast<std::string>(user_data);
        std::string model_val = py::cast<std::string>(model_data);
        self.update_images(
            user_val,
            model_val
        );
    }
    // if input is not None, assume input is a numpy.ndarray (cv2.MatLike)
    else if (
        !py::isinstance<py::none>(user_data) && py::isinstance<py::array>(user_data) &&
        !py::isinstance<py::none>(model_data) && py::isinstance<py::array>(model_data)
    )
    {
        cv::Mat user_image = py::cast<cv::Mat>(user_data);
        cv::Mat model_image = py::cast<cv::Mat>(model_data);
        self.update_images(
            user_image,
            model_image
        );
    }
}

void wrap_MoonRegistrar_update_f2d_detector(mr::MoonRegistrar& self, const mr::RegistrationAlgorithms& algorithm)
{
    self.update_f2d_detector(algorithm);
}

cv::Mat wrap_MoonRegistrar_transform_image(mr::MoonRegistrar& self, const cv::Mat& image_in)
{
    cv::Mat image_out;
    
    self.transform_image(image_in, image_out);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_transform_image_inverse(mr::MoonRegistrar& self, const cv::Mat& image_in)
{
    cv::Mat image_out;
    
    self.transform_image_inverse(image_in, image_out);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_transform_user_image(mr::MoonRegistrar& self)
{
    cv::Mat image_out;
    
    self.transform_user_image(image_out);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_transform_layer_image(mr::MoonRegistrar& self, const cv::Mat& layer_image_in)
{
    cv::Mat layer_image_out;
    
    self.transform_layer_image(layer_image_in, layer_image_out);
    
    return layer_image_out;
}

cv::Mat wrap_MoonRegistrar_draw_matched_keypoints(mr::MoonRegistrar& self)
{
    cv::Mat image_out;
    
    self.draw_matched_keypoints(image_out);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_draw_red_transformed_user_image(mr::MoonRegistrar& self, const cv::Mat& transformed_image_in)
{
    cv::Mat image_out;
    
    self.draw_red_transformed_user_image(image_out, transformed_image_in);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_draw_green_model_image(mr::MoonRegistrar& self)
{
    cv::Mat image_out;
    
    self.draw_green_model_image(image_out);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_draw_stacked_red_green_image(mr::MoonRegistrar& self, const cv::Mat& transformed_image_in)
{
    cv::Mat image_out;
    
    self.draw_stacked_red_green_image(image_out, transformed_image_in);
    
    return image_out;
}

cv::Mat wrap_MoonRegistrar_draw_layer_image(
    mr::MoonRegistrar& self,
    const cv::Mat& layer_image_in,
    const float layer_image_transparency,
    const py::object& filter_px
)
{
    int buffer[4];
    cv::Vec4b* vec4b_filter_px = NULL;
    if (py::isinstance<py::iterable>(filter_px))
    {
        if (py::len(filter_px) != 4)
            throw std::runtime_error("Input filter_px must be length of 4");
        
        int idx = 0;
        for (auto it : filter_px)
        {
            buffer[idx] = py::cast<int>(*it);
            idx++;
        }
        vec4b_filter_px = new cv::Vec4b(
            buffer[0],
            buffer[1],
            buffer[2],
            buffer[3]
        );
    }
    
    cv::Mat image_out;
    
    self.draw_layer_image(
        layer_image_in,
        image_out,
        layer_image_transparency,
        vec4b_filter_px
    );
    
    return image_out;
}

void set_MoonRegistrar_is_good_match(
    mr::MoonRegistrar& self,
    std::function<bool(
        const cv::DMatch&,
        const cv::DMatch&,
        const float,
        const cv::KeyPoint&,
        const cv::KeyPoint&,
        const cv::Mat&,
        const cv::Mat&
    )> func
)
{
    self.is_good_match = func;
}


// initialize submodule
void init_MoonRegistrate(py::module &module)
{
    // MoonRegistrate/registrar.hpp
    
    py::enum_<mr::RegistrationAlgorithms>(module, "RegistrationAlgorithms", py::arithmetic())
        .value("SIFT", mr::RegistrationAlgorithms::SIFT)
        .value("ORB", mr::RegistrationAlgorithms::ORB)
        .value("AKAZE", mr::RegistrationAlgorithms::AKAZE)
        .value("BRISK", mr::RegistrationAlgorithms::BRISK)
#ifdef MR_HAVE_OPENCV_NONFREE
        .value("SURF_NONFREE", mr::RegistrationAlgorithms::SURF_NONFREE)
#endif
        .value("EMPTY_ALGORITHM", mr::RegistrationAlgorithms::EMPTY_ALGORITHM)
        .value("INVALID_ALGORITHM", mr::RegistrationAlgorithms::INVALID_ALGORITHM)
    ;
    
    // Since OpenCV Python wrap cv::Ptr<cv::Features2D> differently,
    // we removed all interfaces relating to cv::Ptr<cv::Features2D>
    // include:
    // mr::create_f2d_detector()
    // mr::MoonRegistrar::update_f2d_detector(const cv::Ptr<cv::Feature2D>&)
    
    module.def("default_is_good_match", mr::default_is_good_match,
    py::arg("m"),
    py::arg("n"),
    py::arg("good_match_ratio"),
    py::arg("user_kpt"),
    py::arg("model_kpt"),
    py::arg("user_image"),
    py::arg("model_image")
    );
    
    py::class_<mr::MoonRegistrar>(module, "MoonRegistrar")
        .def(py::init<>())
        // handle bytes, str, & cv::Mat param
        .def(py::init(&new_MoonRegistrar),
        py::arg("user_data"),
        py::arg("model_data"),
        py::arg("algorithm"),
        R"pbdoc(
    Handling str image_filepath, bytes image_binary, or cv2.MatLike|numpy.ndarray image
    
    Parameters:
      - user_data: can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
      - model_data: can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
      - algorithm: mr::RegistrationAlgorithms enum object
        )pbdoc"
        )
        .def("update_images", wrap_MoonRegistrar_update_images,
        py::arg("user_data"),
        py::arg("model_data"),
        R"pbdoc(
    (re)init mr::MoonRegistrar::user_image & mr::MoonRegistrar::model_image.
    Handling str image_filepath, bytes image_binary, or cv2.MatLike|numpy.ndarray image
    
    Parameters:
      - user_data: can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
      - model_data: can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
        )pbdoc"
        )
        .def("update_f2d_detector", wrap_MoonRegistrar_update_f2d_detector,
        py::arg("algorithm"),
        R"pbdoc(
    (re)init f2d_detector with pre-defined algorithms
    
    Parameters:
      - algorithm: mr::RegistrationAlgorithms enum object
        )pbdoc"
        )
        .def("update_homography_matrix", &mr::MoonRegistrar::update_homography_matrix,
        py::arg("homography_matrix"),
        R"pbdoc(
    update homography_matrix
    
    Parameters:
      - homography_matrix: cv2.MatLike|numpy.ndarray matrix
        )pbdoc"
        )
        .def("update_good_keypoint_matches", &mr::MoonRegistrar::update_good_keypoint_matches,
        py::arg("good_keypoint_matches"),
        R"pbdoc(
    update good_keypoint_matches
    
    Parameters:
      - good_keypoint_matches: list[list[cv2.DMatch]]
        )pbdoc"
        )
        .def("get_homography_matrix", &mr::MoonRegistrar::get_homography_matrix)
        .def("get_user_image", &mr::MoonRegistrar::get_user_image)
        .def("get_user_keypoints", &mr::MoonRegistrar::get_user_keypoints)
        .def("get_model_image", &mr::MoonRegistrar::get_model_image)
        .def("get_model_keypoints", &mr::MoonRegistrar::get_model_keypoints)
        .def("get_good_keypoint_matches", &mr::MoonRegistrar::get_good_keypoint_matches)
        .def("compute_registration", &mr::MoonRegistrar::compute_registration,
        py::arg("knn_k") = 2,
        py::arg("good_match_ratio") = 0.7,
        py::arg("find_homography_method") = static_cast<int>(cv::RANSAC),
        py::arg("find_homography_ransac_reproj_threshold") = 5.0,
        R"pbdoc(
    Compute keypoints, good keypoint matches, and homography matrix.
    You must call this function before using any of the transform_* or draw_* functions.
    
    Parameters:
      - knn_k: int, k value for cv::BFMatcher::knnMatch(). default 2
      - good_match_ratio: float, n m distance ratio for filtering good matches. default 0.7
      - find_homography_method: int, method for cv::findHomography()
        The following methods are possible:
          - **0** - a regular method using all the points, i.e., the least squares method
          - RANSAC - RANSAC-based robust method
          - LMEDS - Least-Median robust method
          - RHO - PROSAC-based robust method
        default to RANSAC
      - find_homography_ransac_reproj_threshold: double, RansacReprojThreshold for cv::findHomograph().
        default 5.0
        )pbdoc"
        )
        .def("transform_image", wrap_MoonRegistrar_transform_image,
        py::arg("image_in"),
        R"pbdoc(
    Using computed homography_matrix to apply a perspective transformation to image_in.
    "Rotate" image_in using homography_matrix.
    
    Parameters:
      - image_in: input image
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("transform_image_inverse", wrap_MoonRegistrar_transform_image_inverse,
        py::arg("image_in"),
        R"pbdoc(
    Using computed homography_matrix inverse to apply a perspective transformation to image_in.
    "Rotate" image_in using homography_matrix inverse.
    
    Parameters:
      - image_in: input image
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("transform_user_image", wrap_MoonRegistrar_transform_user_image,
        R"pbdoc(
    Same as mr::transform_image(), but using user_image as image_in.
    Using computed homography_matrix to apply a perspective transformation to user_image.
    "Rotate" user_image to match model_image's perspective.
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("transform_layer_image", wrap_MoonRegistrar_transform_layer_image,
        py::arg("layer_image_in"),
        R"pbdoc(
    Wrapper around mr::transform_image_inverse(), transform any layer image.
    Using computed homography_matrix to transform layer_image_in in the perspective of user_image.
    "Rotate" layer_image_in to match user_image's perspective.
    
    Parameters:
      - layer_image_in: input image
    
    Returns:
      - output layer image
        )pbdoc"
        )
        .def("draw_matched_keypoints", wrap_MoonRegistrar_draw_matched_keypoints,
        R"pbdoc(
    Draw user_image and model_image side-by-side with good keypoint matches,
    and output to image_out
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("draw_red_transformed_user_image", wrap_MoonRegistrar_draw_red_transformed_user_image,
        py::arg("transformed_image_in") = cv::Mat(),
        R"pbdoc(
    Draw user_image after transforming in the perspective of model_image,
    and output to image_out's red channel
    
    Parameters:
      - transformed_image_in: a replacement of red transformed image
        if this is an empty cv::Mat, this function will generate one using user_image.
        otherwise, this function will use user provided image as red image.
        default is an empty cv::Mat()
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("draw_green_model_image", wrap_MoonRegistrar_draw_green_model_image,
        R"pbdoc(
    Draw model_image, and output to image_out's green channel
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("draw_stacked_red_green_image", wrap_MoonRegistrar_draw_stacked_red_green_image,
        py::arg("transformed_image_in") = cv::Mat(),
        R"pbdoc(
    Stack red image and green image generated by
    mr::draw_red_transformed_user_image() and mr::draw_green_model_image()
    and write to image_out as its red and green channel
    
    Parameters:
      - transformed_image_in: a replacement of red transformed image
        if this is an empty cv::Mat, this function will generate one using user_image.
        otherwise, this function will use user provided image as red image.
        default is an empty cv::Mat()
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("draw_layer_image", wrap_MoonRegistrar_draw_layer_image,
        py::arg("layer_image_in"),
        py::arg("layer_image_transparency") = 1.0f,
        py::arg("filter_px") = py::none(),
        R"pbdoc(
    Transform input layer image to the perspective of user_image
    using mr::transform_layer_image(), and draws it on top of
    user_image, then write to image_out.
    
    Parameters:
      - layer_image_in: input layer image
      - layer_image_transparency: a 0~1 float percentage changing layer image's transparency,
        default 1.0
      - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the foreground image.
        A pixel will be ignore when all of its values is <= filter_px.
        Set it to NULL if you don't need it, default NULL.
    
    Note:
      - this function is designed to work with different number of color channels
      - when using filter_px, we only read number of elements corresponding to
        number of channels in layer image. if layer image has 3 channels, we only read
        first 3 elements of filter_px. rest of elements will be ignored.
    
    Returns:
      - output image
        )pbdoc"
        )
        .def("set_is_good_match", set_MoonRegistrar_is_good_match,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonRegistrar::is_good_match
    
    Parameters:
      - func: callable function (cv2.DMatch, cv2.DMatch, float, cv2.KeyPoint, cv2.KeyPoint, cv2.MatLike|numpy.ndarray, cv2.MatLike|numpy.ndarray) -> bool
    
    A function pointer to a ratio test function that determines
    whether a pair of keypoints are good matches. It runs in a loop
    of all the elements in matches returned by cv::BFMatcher::knnMatch()
    
    function signature:
      bool (
          const cv::DMatch& m,
          const cv::DMatch& n,
          const float good_match_ratio,
          const cv::KeyPoint& user_kpt,
          const cv::KeyPoint& model_kpt,
          const cv::Mat& user_image,
          const cv::Mat& model_image
      )
    
    function parameters:
      - m: matches element[0]
      - n: matches element[1]
      - good_match_ratio: the same good_match_ratio pass into mr::MoonRegistrar::compute_registration(),
        a float of n m distance ratio for filtering good matches. default 0.7
      - user_kpt: user_keypoints[m.queryIdx]
      - model_kpt: user_keypoints[m.trainIdx]
      - user_image: a const reference to user_image
      - model_image: a const reference to model_image
    
    function pointer default points to mr::default_is_good_match()
        )pbdoc"
        )
    ;
}