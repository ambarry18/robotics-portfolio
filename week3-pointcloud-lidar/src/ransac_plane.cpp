#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/visualization/pcl_visualizer.h>

using namespace std;
using namespace pcl;

int main() {
    PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
    PointCloud<PointXYZ>::Ptr cloud_plane(new PointCloud<PointXYZ>);
    PointCloud<PointXYZ>::Ptr cloud_rest(new PointCloud<PointXYZ>);


    io::loadPCDFile<PointXYZ>("/home/amadou/scene.pcd", *cloud);
    cout << "Points totaux : " << cloud->size() << endl;


    ModelCoefficients::Ptr coefficients(new ModelCoefficients);
    PointIndices::Ptr inliers(new PointIndices);

    SACSegmentation<PointXYZ> seg;
    seg.setOptimizeCoefficients(true);
    seg.setModelType(SACMODEL_PLANE);
    seg.setMethodType(SAC_RANSAC);
    seg.setDistanceThreshold(0.01);  // 1cm de tolérance
    seg.setInputCloud(cloud);
    seg.segment(*inliers, *coefficients);

    cout << "Points dans le plan : " << inliers->indices.size() << endl;
    cout << "Equation du plan : "
         << coefficients->values[0] << "x + "
         << coefficients->values[1] << "y + "
         << coefficients->values[2] << "z + "
         << coefficients->values[3] << " = 0" << endl;


    ExtractIndices<PointXYZ> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(inliers);

    extract.setNegative(false);  
    extract.filter(*cloud_plane);

    extract.setNegative(true);   
    extract.filter(*cloud_rest);

    cout << "Points plan : " << cloud_plane->size() << endl;
    cout << "Points reste : " << cloud_rest->size() << endl;


    visualization::PCLVisualizer viewer("RANSAC - Detection de plan");
    viewer.setBackgroundColor(0.1, 0.1, 0.1);

    
    visualization::PointCloudColorHandlerCustom<PointXYZ> green(cloud_plane, 0, 255, 0);
    viewer.addPointCloud<PointXYZ>(cloud_plane, green, "plan");
    viewer.setPointCloudRenderingProperties(
        visualization::PCL_VISUALIZER_POINT_SIZE, 4, "plan");

    
    visualization::PointCloudColorHandlerCustom<PointXYZ> red(cloud_rest, 255, 0, 0);
    viewer.addPointCloud<PointXYZ>(cloud_rest, red, "reste");
    viewer.setPointCloudRenderingProperties(
        visualization::PCL_VISUALIZER_POINT_SIZE, 2, "reste");

    viewer.addCoordinateSystem(0.1);
    viewer.spin();

    return 0;
}
