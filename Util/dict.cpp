#include "dict.hpp"

namespace TractUtil
{
    Dict::Branch::Branch():
        midPoint(1, 128, FLOAT_CV, 0.0),
        belongs(),
        children(),
        index(-1)
    {}

    Dict::Dict(): root(new Branch()), leafCount(0),
                       leaves(), wi(), pQue({root}) {}

    double Dict::StandardDev(const vector<shared_ptr<Branch>>& iMeans,
                             const size_t& iSum) const
    {
        size_t len = iMeans.size();
        double mean = 0.0;
        double dev = 0.0;

        if (len <= 0 || iSum <=0)
        // The input vector is actually empty.
        {
            return 0.0;
        }

        mean = static_cast<double>(iSum) / len;

        for (const auto& i: iMeans)
        // Calculate deviation
        {
            double dif = i->belongs.size() - mean;
            dev += dif * dif;
        }

        return sqrt(dev);
    }

    bool Dict::IsLeaf(shared_ptr<Branch>& iBranch)
    {
        size_t cnt = iBranch->belongs.size();
        if (cnt > K)
        {
            // The number of objects under the branch is greater than the number
            // of samples taken. Test their distance from the midpoint then.
            bool inRange = true;
            for (const auto& i: iBranch->belongs)
            {
                if (iBranch->midPoint.dot(i->descriptor) < bound)
                {
                    // One object divert from the midpoint, need further division.
                    inRange = false;
                    break;
                }
            }
            if (!inRange)
            // Not a leaf
            {
                return false;
            }
        }

        // Is a leaf
        //cout<<cnt<<' ';
        // Push to the leaf array
        leaves.push_back(iBranch);
        iBranch->index = leafCount;
        ++ leafCount;
        return true;
    }

    void Dict::GetDescriptor(Mat& ioVector) const
    {
        if (ioVector.rows != 1 || static_cast<size_t>(ioVector.cols) != leafCount)
        {
            cout<< "GetDescriptor dismatch"<<endl;
            return;
        }
        for (size_t i = 0; i < leafCount; ++i)
        {
            ioVector.at<double>(0, i) *= wi[i];
        }
        normalize(ioVector, ioVector);
        ioVector.convertTo(ioVector, FLOAT_CV);
    }

    void Dict::BuildDict(vector<Entry>& iWords,
                         const size_t&& iCount,
                         vector<Mat>& oArticles)
    {
        srand((size_t)time(0));

        for (auto& i: iWords)
        // Record the address for the entries
        {
            root->belongs.push_back(&i);
            root->midPoint += i.descriptor;
        }
        // The center point for all points
        normalize(root->midPoint, root->midPoint);
        root->midPoint.convertTo(root->midPoint, FLOAT_CV);

        // Maintain a query for operation
        while (pQue.size())
        {
            if (!IsLeaf(pQue.front()))
            // The branch can still be divided
            {
                size_t entryLen = pQue.front()->belongs.size();
                double minSDev = DBL_MAX;
                vector<shared_ptr<Branch>> bestDiv = {}; // Best division
                vector<shared_ptr<Branch>> presDiv = {}; // Present division
                unordered_map<size_t, bool> numUsed;

                for (size_t l = 0; l < attemps; ++l)
                // Randomly pick starting points for multi-times and take the best
                {
                    presDiv.clear();
                    numUsed.clear();
                    for (size_t i = 0; i < K; ++i)
                    // Generate K starting points
                    {
                        size_t index;
                        while (index = rand()%entryLen, numUsed[index]);
                        numUsed[index] = true;
                        presDiv.push_back(shared_ptr<Branch>(new Branch()));
                        presDiv.back()->midPoint = pQue.front()->belongs[index]
                                                               ->descriptor;
                    }
                    double pSDev = 0.0;
                    double sDev = 0.0;

                    for (size_t h = 0; h < maxLoop; ++h)
                    // Do min-shift on each cluster.
                    // Set a maxloop to prevent endless loop.
                    {
                        for (auto& i: presDiv)
                        {
                            i->belongs.clear();
                        }
                        for (const auto& i: pQue.front()->belongs)
                        // For each object, find its closest cluster to join
                        {
                            double maxProd = -2.0;
                            shared_ptr<Branch> maxBranch = nullptr;
                            for (auto& j: presDiv)
                            {
                                double dotProd = i->descriptor.dot(j->midPoint);
                                if (dotProd >= maxProd)
                                {
                                    maxProd = dotProd;
                                    maxBranch = j;
                                }
                            }
                            maxBranch->belongs.push_back(i);
                        }
                        for (auto& i: presDiv)
                        // Recalculate the midpoint from the objects in each cluster
                        {
                            Mat midPoint = Mat(1, 128, FLOAT_CV, 0.0);
                            for (auto& j: i->belongs)
                            {
                                midPoint += j->descriptor;
                            }
                            normalize(midPoint, i->midPoint);
                            i->midPoint.convertTo(i->midPoint, FLOAT_CV);
                        }
                        // Calculate the standard devision of the numbers of
                        // objects in each cluster and find selection with a
                        // minimal sDev as the best one. (Evenly distributed)
                        sDev = StandardDev(presDiv, entryLen);
                        if (abs(sDev - pSDev) <= EPS)
                        {
                            break;
                        }
                        pSDev = sDev;
                    }

                    if (sDev < minSDev)
                    {
                        minSDev = sDev;
                        bestDiv = move(presDiv);
                    }
                }

                // Push the list of the best devision as the children array
                // of the parent.
                pQue.front()->children.clear();
                for (const auto& i: bestDiv)
                {
                    if (i->belongs.size())
                    {
                        pQue.front()->children.push_back(i);
                        pQue.push(i);
                    }
                }
            }
            pQue.pop();
        }

        //cout<<endl<<leafCount<<endl;
        // Get the article vector for each database image
        oArticles = vector<Mat>(iCount, Mat(1, leafCount, FLOAT_CV, 0.0));
        wi = vector<double>(leafCount, 0.0);
        for (auto& i: oArticles)
        {
            i = Mat(1, leafCount, FLOAT_CV, 0.0);
        }

        for (size_t i = 0; i < leafCount; ++i)
        {
            map<size_t, bool> containWord;
            containWord.clear();
            for (const auto& j: leaves[i]->belongs)
            {
                size_t ind = j->belong;
                oArticles[ind].at<CV_TYPE>(0, i) += (double)1.0;
                if (!containWord[ind])
                {
                    ++ wi[i];
                    containWord[ind] = true;
                }
            }
            // Calculate the wi array used for setting the weight.
            wi[i] = log(iCount / wi[i]);
        }

        for (auto& i: oArticles)
        // Include the weights and normalize the final result.
        {
            GetDescriptor(i);
        }
    }

    void Dict::TestArticle(const vector<Mat>& iWord, Mat& oArticle) const
    {
        oArticle = Mat(1, leafCount, FLOAT_CV, 0.0);
        for (const auto& i: iWord)
        // Find the position in the tree of each descriptor in the input image
        // and get the article vector according.
        {
            shared_ptr<Branch> ptr = root;
            while (ptr->index == -1)
            {
                double closest = -2.0;
                size_t closest_j = 0;
                for (size_t j = 0; j < ptr->children.size(); ++j)
                {
                    double dotP = ptr->children[j]->midPoint.dot(i);
                    if (dotP > closest)
                    {
                        closest = dotP;
                        closest_j = j;
                    }
                }
                ptr = ptr->children[closest_j];
            }
            oArticle.at<CV_TYPE>(0, ptr->index) += 1;
        }
        GetDescriptor(oArticle);
    }
}
