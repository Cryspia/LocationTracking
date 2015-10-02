#include "global.hpp"
#include "dict.hpp"

namespace TractUtil
{
    class ImgDatabase
    {
        private:
            vector<vector<KeyPoint>> kps;   // The Keypoints found in the imgs.
            vector<Mat> articles;
            // Each img as an article to record the words used.
            vector<Entry> words;   // The word set.
            Dict dict;  // The dictionary for this database.

            void getKeyPoints(const Mat& iImg,
                              vector<KeyPoint>& oKps,
                              Mat& oDescriptors) const;
            // To get the keypoints and the normalized descriptors for the
            // input image.
        public:
            ImgDatabase(const vector<Mat>& iImgs);
            // The contructor that read in all the image database and prepare the
            // word set.

            vector<pair<size_t, double>> TestImg(const Mat& iImg,
                                                 const size_t&& firstX = 1) const;
            // Compare the input image with the articles in the database.
            // Output the X most similar images with their index and the similarity.
    };
}
