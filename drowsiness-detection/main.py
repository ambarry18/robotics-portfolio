# main.py — version 2 : seuils calibrés + lissage

import cv2
import numpy as np
from imutils.video import VideoStream
import imutils
import time
from collections import deque

from src.landmarks import (
    load_detector_and_predictor,
    get_landmarks,
    compute_avg_ear,
    compute_mouth_mar,
)

PREDICTOR_PATH    = "models/shape_predictor_68_face_landmarks.dat"

# Seuils 
EAR_THRESHOLD     = 0.22
MAR_THRESHOLD     = 0.80
EAR_CONSEC_FRAMES = 20   # ~0.67s à 30fps avant alerte somnolence

# Taille de la fenêtre de lissage
SMOOTH_N = 10


def draw_landmarks_region(frame, points, color=(0, 255, 0)):
    pts = cv2.convexHull(points)
    cv2.drawContours(frame, [pts], -1, color, 1)


def main():
    print("[INFO] Chargement des modèles...")
    detector, predictor = load_detector_and_predictor(PREDICTOR_PATH)

    print("[INFO] Démarrage webcam...")
    vs = VideoStream(src=0).start()
    time.sleep(1.0)

    ear_counter = 0

    # Files FIFO pour la moyenne glissante
    ear_buffer = deque(maxlen=SMOOTH_N)
    mar_buffer = deque(maxlen=SMOOTH_N)

    print("[INFO] Appuie sur 'q' pour quitter.")

    while True:
        frame = vs.read()
        if frame is None:
            break

        frame = imutils.resize(frame, width=450)
        gray  = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        landmarks, rect = get_landmarks(gray, detector, predictor)

        if landmarks is not None:
            raw_ear, right_eye_pts, left_eye_pts = compute_avg_ear(landmarks)
            raw_mar, mouth_pts = compute_mouth_mar(landmarks)

            # Ajout dans les buffers
            ear_buffer.append(raw_ear)
            mar_buffer.append(raw_mar)

            # Valeurs lissées (moyenne des N dernières frames)
            ear = np.mean(ear_buffer)
            mar = np.mean(mar_buffer)

            # Dessin des régions
            draw_landmarks_region(frame, right_eye_pts, color=(0, 255, 0))
            draw_landmarks_region(frame, left_eye_pts,  color=(0, 255, 0))
            draw_landmarks_region(frame, mouth_pts,     color=(0, 255, 255))

            # Logique d'alerte EAR
            if ear < EAR_THRESHOLD:
                ear_counter += 1
                if ear_counter >= EAR_CONSEC_FRAMES:
                    cv2.putText(frame, "SOMNOLENCE DETECTEE",
                                (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                                0.7, (0, 0, 255), 2)
            else:
                ear_counter = 0

            # Logique d'alerte MAR
            if mar > MAR_THRESHOLD:
                cv2.putText(frame, "BAILLEMENT",
                            (10, 60), cv2.FONT_HERSHEY_SIMPLEX,
                            0.7, (0, 165, 255), 2)

            # Affichage métriques lissées
            cv2.putText(frame, f"EAR: {ear:.3f}",
                        (300, 30), cv2.FONT_HERSHEY_SIMPLEX,
                        0.6, (255, 255, 255), 2)
            cv2.putText(frame, f"MAR: {mar:.3f}",
                        (300, 55), cv2.FONT_HERSHEY_SIMPLEX,
                        0.6, (255, 255, 255), 2)

        else:
            # Vider les buffers si pas de visage détecté
            ear_buffer.clear()
            mar_buffer.clear()
            cv2.putText(frame, "Aucun visage detecte",
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                        0.6, (0, 0, 255), 2)

        cv2.imshow("Drowsiness Detection - Phase 1", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    vs.stop()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()