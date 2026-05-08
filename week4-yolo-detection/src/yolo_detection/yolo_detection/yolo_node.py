import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from ultralytics import YOLO


class YoloDetectionNode(Node):
    def __init__(self):
        super().__init__('yolo_detection_node')

        # Charger le modèle YOLOv8
        self.model = YOLO("yolov8n.pt")
        self.bridge = CvBridge()

        # Abonnement à la caméra du TurtleBot3
        self.sub = self.create_subscription(
            Image,
            '/camera/image_raw',
            self.image_callback,
            10)

        # Publisher image annotée
        self.pub = self.create_publisher(Image, '/yolo_image', 10)

        self.get_logger().info("YoloDetectionNode démarré !")

    def image_callback(self, msg):
        # Etape 1 : ROS Image → OpenCV via cv_bridge
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')

        # Etape 2 : Détection YOLOv8
        results = self.model(frame, verbose=False)
        annotated = results[0].plot()

        # Etape 3 : Logger les détections
        for box in results[0].boxes:
            cls = int(box.cls[0])
            conf = float(box.conf[0])
            nom = self.model.names[cls]
            if conf > 0.5:  # seuil de confiance
                self.get_logger().info(f"Détecté : {nom} ({conf:.2%})")

        # Etape 4 : OpenCV → ROS Image et publier
        out_msg = self.bridge.cv2_to_imgmsg(annotated, encoding='bgr8')
        out_msg.header = msg.header
        self.pub.publish(out_msg)


def main():
    rclpy.init()
    node = YoloDetectionNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == '__main__':
    main()
