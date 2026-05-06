#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <laser_geometry/laser_geometry.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/search/kdtree.h>
#include <pcl/point_cloud.h>

using namespace std;
using namespace pcl;

class LidarPipelineNode : public rclcpp::Node {
public:
    LidarPipelineNode() : Node("lidar_pipeline_node") {
        sub_ = create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10,
            bind(&LidarPipelineNode::scan_callback, this, placeholders::_1));

        pub_filtered_  = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_filtered", 10);
        pub_ground_    = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_ground", 10);
        pub_obstacles_ = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_obstacles", 10);

        RCLCPP_INFO(get_logger(), "LidarPipelineNode démarré !");
    }

private:
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan_msg) {

        // Etape 1 : Convertir LaserScan → PointCloud2
        sensor_msgs::msg::PointCloud2 cloud_ros;
        projector_.projectLaser(*scan_msg, cloud_ros);

        // Etape 2 : Convertir PointCloud2 ROS → PCL
        PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
        fromROSMsg(cloud_ros, *cloud);
        RCLCPP_INFO(get_logger(), "Points reçus : %zu", cloud->size());

        // Etape 3 : Filtrage VoxelGrid
        PointCloud<PointXYZ>::Ptr cloud_filtered(new PointCloud<PointXYZ>);
        VoxelGrid<PointXYZ> vox;
        vox.setInputCloud(cloud);
        vox.setLeafSize(0.05f, 0.05f, 0.05f);
        vox.filter(*cloud_filtered);
        RCLCPP_INFO(get_logger(), "Après filtre : %zu", cloud_filtered->size());

        // Etape 4 : Clustering (LiDAR 2D = pas de sol à retirer)
        PointCloud<PointXYZ>::Ptr cloud_obstacles(new PointCloud<PointXYZ>);
        PointCloud<PointXYZ>::Ptr cloud_ground(new PointCloud<PointXYZ>);
        *cloud_obstacles = *cloud_filtered;

        if (!cloud_obstacles->empty()) {
            search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>);
            tree->setInputCloud(cloud_obstacles);

            vector<PointIndices> cluster_indices;
            EuclideanClusterExtraction<PointXYZ> ec;
            ec.setClusterTolerance(0.3);
            ec.setMinClusterSize(5);
            ec.setMaxClusterSize(5000);
            ec.setSearchMethod(tree);
            ec.setInputCloud(cloud_obstacles);
            ec.extract(cluster_indices);

            RCLCPP_INFO(get_logger(), "Clusters détectés : %zu", cluster_indices.size());
        }

        // Etape 5 : Publier pour RViz2
        auto publish_cloud = [&](PointCloud<PointXYZ>::Ptr& c,
                                  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr& pub) {
            sensor_msgs::msg::PointCloud2 msg;
            toROSMsg(*c, msg);
            msg.header.frame_id = "base_scan";
            msg.header.stamp = now();
            pub->publish(msg);
        };

        publish_cloud(cloud_filtered,  pub_filtered_);
        publish_cloud(cloud_ground,    pub_ground_);
        publish_cloud(cloud_obstacles, pub_obstacles_);
    }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_filtered_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_ground_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_obstacles_;
    laser_geometry::LaserProjection projector_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LidarPipelineNode>());
    rclcpp::shutdown();
    return 0;
}
