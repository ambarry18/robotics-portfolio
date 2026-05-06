#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/random.h>

using namespace std;
using namespace pcl;

int main() {
    PointCloud<PointXYZ>::Ptr scene(new PointCloud<PointXYZ>);

    common::UniformGenerator<float> rand_xy(-5.0f, 5.0f, 42);
    common::UniformGenerator<float> rand_noise(-0.005f, 0.005f, 43);
    common::UniformGenerator<float> rand_obj(0.0f, 1.0f, 44);

    // Sol plat : Z = 0 avec un peu de bruit
    for (int i = 0; i < 2000; i++) {
        PointXYZ p;
        p.x = rand_xy.run();
        p.y = rand_xy.run();
        p.z = rand_noise.run();  // Z ≈ 0
        scene->push_back(p);
    }

    // Objet 1 : boîte à gauche
    for (int i = 0; i < 300; i++) {
        PointXYZ p;
        p.x = -2.0f + rand_obj.run() * 1.0f;
        p.y = -1.0f + rand_obj.run() * 1.0f;
        p.z =  0.0f + rand_obj.run() * 1.5f;
        scene->push_back(p);
    }

    // Objet 2 : boîte à droite
    for (int i = 0; i < 300; i++) {
        PointXYZ p;
        p.x =  2.0f + rand_obj.run() * 1.0f;
        p.y =  1.0f + rand_obj.run() * 1.0f;
        p.z =  0.0f + rand_obj.run() * 1.0f;
        scene->push_back(p);
    }

    scene->width = scene->size();
    scene->height = 1;

    io::savePCDFileASCII("/home/amadou/scene.pcd", *scene);
    cout << "Scène générée : " << scene->size() << " points" << endl;
    cout << "Fichier sauvegardé : /home/amadou/scene.pcd" << endl;

    return 0;
}
