#include "imgDatabase.hpp"

namespace TractUtil
{
    void ImgDatabase::getKeyPoints(const Mat& iImg,
                                  vector<KeyPoint>& oKps,
                                  Mat& oDescriptors) const
    // Calculate the keypoint and the descriptor for the input image using
    // OpenCV. SIFT keypoint is used in this version.
    {
        SiftFeatureDetector siftdtc;
        SiftDescriptorExtractor extractor;
        siftdtc.detect(iImg, oKps);
        extractor.compute(iImg, oKps, oDescriptors);
    }

    ImgDatabase::ImgDatabase(const vector<Mat>& iImgs):
        kps(), articles(), words(), dict()
    {
        kps.clear();
        words.clear();
        for (size_t i = 0; i < iImgs.size(); ++i)
        // Get keypoints and descriptors for each input image.
        {
            vector<KeyPoint> kp;
            Mat des;
            getKeyPoints(iImgs[i], kp, des);
            /*if (i==0) {
                Mat out1;
                drawKeypoints(iImgs[0], kp, out1);
                imshow("d1", out1);
            }*/
            kps.push_back(kp);
            for (size_t j = 0; j < static_cast<size_t>(des.rows); ++j)
            // Split the Mat descriptor output into Vectors.
            {
                Mat normed;
                normalize(des.row(j), normed);
                normed.convertTo(normed, FLOAT_CV);
                words.push_back(Entry{normed, i});
            }
        }
        // Build tree on the database image
        dict.BuildDict(words, iImgs.size(), articles);
    }

    vector<pair<size_t, double>> ImgDatabase::TestImg(const Mat& iImg,
                                                      const size_t&& firstX) const
    {
        Mat des, art;
        vector<KeyPoint> kp;
        vector<Mat> word;
        kp.clear();
        word.clear();
        getKeyPoints(iImg, kp, des);
        Mat out;
        drawKeypoints(iImg, kp, out);
        imshow("t1", out);
        for (size_t i = 0; i < static_cast<size_t>(des.rows); ++i)
        {
            Mat normed;
            normalize(des.row(i), normed);
            normed.convertTo(normed, FLOAT_CV);
            word.push_back(normed);
        }
        // Get the article vector in the dictionary
        dict.TestArticle(word, art);
        vector<pair<size_t, double>> result;
        result.clear();
        /*cout<<art<<endl;
        cout<<articles[0]<<endl;*/
        for (size_t i = 0; i < articles.size(); ++i)
        // Find the firstX number of images in the database with the greatest
        // dot production with the test image.
        {
            double sim = articles[i].dot(art);
            if (result.size() == 0)
            {
                result.push_back(make_pair(i, sim));
            }
            else
            {
                for (size_t j = result.size(); j > 0; --j)
                {
                    if (sim <= result[j - 1].second)
                    {
                        if (j == firstX)
                        {
                            continue;
                        }
                        result.insert(result.begin() + j, make_pair(i, sim));
                        break;
                    }
                    else
                    {
                        if (j == 1)
                        {
                            result.insert(result.begin(), make_pair(i, sim));
                        }
                    }
                }
            }
            if (result.size() > firstX)
            {
                result.pop_back();
            }
        }
        return result;
    }
}
