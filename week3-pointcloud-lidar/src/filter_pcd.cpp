#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/visualization/pcl_visualizer.h>

using namespace std;
using namespace pcl;

int main() {
    PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
    PointCloud<PointXYZ>::Ptr cloud_voxel(new PointCloud<PointXYZ>);
    PointCloud<PointXYZ>::Ptr cloud_filtered(new PointCloud<PointXYZ>);


    io::loadPCDFile<PointXYZ>("/home/amadou/bunny.pcd", *cloud);
    cout << "Points originaux : " << cloud->size() << endl;


    VoxelGrid<PointXYZ> vox;
    vox.setInputCloud(cloud);
    vox.setLeafSize(0.005f, 0.005f, 0.005f);
    vox.filter(*cloud_voxel);
    cout << "Après VoxelGrid : " << cloud_voxel->size() << endl;

    
    StatisticalOutlierRemoval<PointXYZ> sor;
    sor.setInputCloud(cloud_voxel);
    sor.setMeanK(50);
    sor.setStddevMulThresh(1.0);
    sor.filter(*cloud_filtered);
    cout << "Après SOR : " << cloud_filtered->size() << endl;


    visualization::PCLVisualizer viewer("Filtrage PCL");
    viewer.setBackgroundColor(0.1, 0.1, 0.1);


    visualization::PointCloudColorHandlerCustom<PointXYZ> white(cloud, 255, 255, 255);
    viewer.addPointCloud<PointXYZ>(cloud, white, "original");
    viewer.setPointCloudRenderingProperties(
        visualization::PCL_VISUALIZER_POINT_SIZE, 2, "original");


    visualization::PointCloudColorHandlerCustom<PointXYZ> red(cloud_filtered, 255, 0, 0);
    viewer.addPointCloud<PointXYZ>(cloud_filtered, red, "filtre");
    viewer.setPointCloudRenderingProperties(
        visualization::PCL_VISUALIZER_POINT_SIZE, 3, "filtre");

    viewer.addCoordinateSystem(0.1);
    viewer.spin();

    return 0;
}
