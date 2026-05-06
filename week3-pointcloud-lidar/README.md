# Week 3 — PointCloud & LiDAR 3D

## Objectifs
Maîtriser le traitement de nuages de points 3D avec PCL et intégrer un pipeline
de perception LiDAR en temps réel dans ROS 2.

## Environnement
- Ubuntu 22.04
- PCL 1.12.1
- ROS 2 Humble
- Gazebo Classic
- TurtleBot3 Waffle

## Pipeline implémenté
LaserScan (ROS 2) → PointCloud2 → Filtrage VoxelGrid → Clustering Euclidien → RViz2
## Programmes C++ standalone (PCL)

### load_pcd.cpp
Chargement et visualisation d'un fichier .pcd avec PCLVisualizer.
- Affichage des métadonnées (nombre de points, type)
- Accès aux coordonnées XYZ individuelles

### filter_pcd.cpp
Filtrage d'un nuage de points avec deux filtres PCL :
- **VoxelGrid** : sous-échantillonnage par cubes (LeafSize = 5mm)
- **StatisticalOutlierRemoval** : suppression du bruit (MeanK=50, StddevMulThresh=1.0)

### ransac_plane.cpp
Détection du plan dominant avec RANSAC :
- Segmentation sol / obstacles
- DistanceThreshold = 1cm
- Equation du plan : -4.33955e-05x + -2.61092e-06y + 1z + -0.000229327 = 0

### clustering.cpp
Pipeline complet sur scène synthétique :
- Retrait du sol (RANSAC)
- Clustering Euclidien (tolérance = 50cm)
- Visualisation de chaque cluster en couleur distincte
- Résultat : 2 clusters détectés sur 2 objets générés

## Package ROS 2 — lidar_pipeline

Nœud ROS 2 temps réel abonné au topic `/scan` du TurtleBot3 Waffle.

### Pipeline
1. Conversion `LaserScan` → `PointCloud2` via `laser_geometry`
2. Filtrage `VoxelGrid` (LeafSize = 5cm)
3. Clustering Euclidien (tolérance = 30cm, min 5 points)
4. Publication sur 3 topics :
   - `/cloud_filtered` — nuage filtré
   - `/cloud_ground` — sol (vide pour LiDAR 2D)
   - `/cloud_obstacles` — clusters détectés

### Résultats
- 360 points reçus par scan à 5Hz
- ~270 points après filtrage
- **9 clusters stables** détectés en temps réel dans turtlebot3_world

## Concepts clés maîtrisés
- Structure d'un fichier .pcd (header + données XYZ)
- Nuage organisé vs non-organisé
- VoxelGrid : compromis densité / précision via LeafSize
- StatisticalOutlierRemoval : suppression bruit statistique
- RANSAC : détection de modèle géométrique robuste au bruit
- Clustering Euclidien : séparation d'objets par proximité spatiale
- KdTree : indexation spatiale pour recherche de voisins en O(log n)

## Lancer le pipeline ROS 2

```bash
# Terminal 1 — Gazebo
source /opt/ros/humble/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py

# Terminal 2 — Pipeline LiDAR
source /opt/ros/humble/setup.bash
source ~/ros2_ws/install/setup.bash
ros2 run lidar_pipeline lidar_pipeline_node

# Terminal 3 — Visualisation
source /opt/ros/humble/setup.bash
rviz2
```
