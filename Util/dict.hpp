#include "global.hpp"

namespace TractUtil
{
    class Dict
    {
        private:
            struct Branch // The structure to store the info for each branch
            {
                Mat midPoint;
                // The normalized mid point for the entries under the branch
                // calculated using arithmatic mean value.
                vector<Entry*> belongs;
                // The entries that belongs to this branch.
                vector<shared_ptr<Branch>> children;
                // The children branch / leaf under this branch
                int index;
                // If the branch is already a leaf, it need a index in the
                // dictionary.
                Branch();
            };

            static constexpr double bound = 0.999;
            // The branch will become a leaf if the all the dot products of the
            // entries under the branch and the midpoint extend `bound`
            static constexpr size_t K = 10;
            // The exact value for K-Means
            static constexpr size_t attemps = 5;
            // Randomly try `attemps` times to divide a branch and find a best one.
            static constexpr size_t maxLoop = 500;
            // The max loop for to confirm a division.

            shared_ptr<Branch> root;    // The dummy root node.
            size_t leafCount;       // count the number of leaves (words in dict).
            vector<shared_ptr<Branch>> leaves;
            // The array to record leaves in tree of dict.
            vector<double> wi;          // The array to record log(N/Ni).
            queue<shared_ptr<Branch>> pQue; //process queue for tree building.

            double StandardDev(const vector<shared_ptr<Branch>>& iMeans,
                               const size_t& iSum) const;
            // Calculate the standard deviation for the numbers of entries in the
            // Branch array.

            bool IsLeaf(shared_ptr<Branch>& iBranch);
            // To test if the input branch is already a leaf.
            // If so, add it to the leaf array and return true.

            void GetDescriptor(Mat& ioVector) const;
            // Times the input vector with the wi factors and normalize
            // the result.

        public:
            Dict();
            void BuildDict(vector<Entry>& iWords,
                           const size_t&& iCount,
                           vector<Mat>& oArticles);
            // Build the KMeans dictionary use the input word set
            // and output the test results for the database images using
            // the dict and the number of images `iCount`.
            void TestArticle(const vector<Mat>& iWord, Mat& oArticle) const;
            // Get the article test result for the input image with its words.
    };
}
