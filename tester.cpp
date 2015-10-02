#include "Util/imgDatabase.hpp"
#include <dirent.h>

#define DATA_DIR "Pic/Data/"
#define TARGET_DIR "Pic/Target/"

using namespace TractUtil;
using namespace std;


int main()
{
    vector<Mat> imgs;
    imgs.clear();
    // Read all database image files.
    DIR *dir;
    dirent *ent;
    cout<<"Try to read img database..."<<endl;
    if ((dir = opendir(DATA_DIR)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            // Check the file type
            size_t slen = strlen(ent->d_name);
            if (slen > 4 && !strcmp(ent->d_name + slen - 4, ".jpg"))
            {
                // The right image files
                string fileName(DATA_DIR);
                fileName.append(ent->d_name);
                Mat img = imread(fileName, 0);
                if (!img.empty())
                {
                    imgs.push_back(img);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        cout<<"Input dir not exist, please make sure '"<<DATA_DIR<<"' is accessable"<<endl;
        return 0;
    }

    cout<<"Input into database structure..."<<endl;
    shared_ptr<ImgDatabase> db(new ImgDatabase(imgs));
    cout<<"Succeeded!"<<endl;
    int windowId = 0;
    do
    // Loop until no input file name.
    {
        string fileName;
        cout<<"Input a Img file name under '"<<TARGET_DIR<<"':"<<endl;
        getline(cin, fileName);
        if (fileName.length() <= 1)
        {
            break;
        }
        fileName = TARGET_DIR + fileName;
        Mat img = imread(fileName, 0);
        if (img.empty())
        {
            cout<<"File does not exist!"<<endl;
            continue;
        }
        vector<pair<size_t, double>> && res = db->TestImg(img, 2);
        imshow(to_string(++windowId), imgs[res[0].first]);
        cout<<windowId<<": "<<res[0].first<<','<<res[0].second<<endl;
        imshow(to_string(++windowId), imgs[res[1].first]);
        cout<<windowId<<": "<<res[1].first<<','<<res[1].second<<endl;
    } while (true);
    return 0;
}
