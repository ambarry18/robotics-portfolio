#include <iostream>
#include <vector>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/search/kdtree.h>
#include <pcl/visualization/pcl_visualizer.h>

using namespace std;
using namespace pcl;

int main() {
    PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
    PointCloud<PointXYZ>::Ptr cloud_no_ground(new PointCloud<PointXYZ>);


    io::loadPCDFile<PointXYZ>("/home/amadou/scene.pcd", *cloud);
    cout << "Points totaux : " << cloud->size() << endl;


    ModelCoefficients::Ptr coefficients(new ModelCoefficients);
    PointIndices::Ptr inliers(new PointIndices);

    SACSegmentation<PointXYZ> seg;
    seg.setOptimizeCoefficients(true);
    seg.setModelType(SACMODEL_PLANE);
    seg.setMethodType(SAC_RANSAC);
    seg.setDistanceThreshold(0.01);
    seg.setInputCloud(cloud);
    seg.segment(*inliers, *coefficients);

    ExtractIndices<PointXYZ> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(inliers);
    extract.setNegative(true);  
    extract.filter(*cloud_no_ground);

    cout << "Points après retrait sol : " << cloud_no_ground->size() << endl;


    search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>);
    tree->setInputCloud(cloud_no_ground);

    vector<PointIndices> cluster_indices;
    EuclideanClusterExtraction<PointXYZ> ec;
    ec.setClusterTolerance(0.5);   // 50cm de rayon
    ec.setMinClusterSize(10);      // minimum 10 points
    ec.setMaxClusterSize(10000);   // maximum 10000 points
    ec.setSearchMethod(tree);
    ec.setInputCloud(cloud_no_ground);
    ec.extract(cluster_indices);

    cout << "Nombre de clusters trouvés : " << cluster_indices.size() << endl;

   
    visualization::PCLVisualizer viewer("Clustering Euclidien");
    viewer.setBackgroundColor(0.1, 0.1, 0.1);


    vector<array<int,3>> colors = {
        {255, 0,   0},    // Rouge
        {0,   255, 0},    // Vert
        {0,   0,   255},  // Bleu
        {255, 255, 0},    // Jaune
        {255, 0,   255}   // Magenta
    };

    int cluster_id = 0;
    for (const auto& indices : cluster_indices) {
        PointCloud<PointXYZ>::Ptr cluster(new PointCloud<PointXYZ>);
        for (int idx : indices.indices)
            cluster->push_back(cloud_no_ground->points[idx]);

        auto& c = colors[cluster_id % colors.size()];
        visualization::PointCloudColorHandlerCustom<PointXYZ> color(cluster, c[0], c[1], c[2]);
        string name = "cluster_" + to_string(cluster_id);
        viewer.addPointCloud<PointXYZ>(cluster, color, name);
        viewer.setPointCloudRenderingProperties(
            visualization::PCL_VISUALIZER_POINT_SIZE, 4, name);

        cout << "Cluster " << cluster_id
             << " : " << cluster->size() << " points" << endl;
        cluster_id++;
    }

    viewer.addCoordinateSystem(0.1);
    viewer.spin();

    return 0;
}
