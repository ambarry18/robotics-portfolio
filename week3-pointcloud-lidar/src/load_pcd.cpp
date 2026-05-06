#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

int main() {

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);


    if (pcl::io::loadPCDFile<pcl::PointXYZ>("/home/amadou/bunny.pcd", *cloud) == -1) {
        PCL_ERROR("Impossible de charger le fichier PCD\n");
        return -1;
    }


    std::cout << "=== Infos du nuage de points ===" << std::endl;
    std::cout << "Nombre de points : " << cloud->size() << std::endl;
    std::cout << "Organisé : " << (cloud->isOrganized() ? "Oui" : "Non") << std::endl;

  
    std::cout << "\n=== 5 premiers points ===" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << "Point " << i << " : ("
                  << cloud->points[i].x << ", "
                  << cloud->points[i].y << ", "
                  << cloud->points[i].z << ")" << std::endl;
    }

  
    pcl::visualization::PCLVisualizer viewer("Mon premier viewer PCL");
    viewer.setBackgroundColor(0.1, 0.1, 0.1);
    viewer.addPointCloud<pcl::PointXYZ>(cloud, "bunny");
    viewer.setPointCloudRenderingProperties(
        pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "bunny");
    viewer.addCoordinateSystem(0.1);
    viewer.spin();

    return 0;
}
