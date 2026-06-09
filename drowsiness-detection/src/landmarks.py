# src/landmarks.py
# ---------------------------------------------------------
# Extraction des 68 landmarks faciaux, calcul EAR et MAR
# ---------------------------------------------------------

import dlib
import numpy as np
from scipy.spatial.distance import euclidean
from imutils import face_utils


# ------------------------------------------------------------------
# Indices des landmarks (0-indexés) pour chaque région du visage
# ------------------------------------------------------------------
# dlib numérote de 1 à 68, on décale de 1 pour Python
RIGHT_EYE  = list(range(36, 42))   # 6 points
LEFT_EYE   = list(range(42, 48))   # 6 points
MOUTH      = list(range(48, 68))   # 20 points

# Indices spécifiques pour le calcul MAR
# On utilise 8 points clés de la bouche (voir schéma dans README)
MOUTH_MAR  = [60, 50, 51, 52, 64, 56, 57, 58]


def load_detector_and_predictor(predictor_path: str):

    detector  = dlib.get_frontal_face_detector()
    predictor = dlib.shape_predictor(predictor_path)
    return detector, predictor


def get_landmarks(frame_gray, detector, predictor):

    # upsample_num_times=0 : pas d'upsampling, on priorise la vitesse
    # mettre 1 pour détecter les visages plus petits/éloignés
    rects = detector(frame_gray, 0)

    if len(rects) == 0:
        return None, None

    # On prend le premier visage détecté (le plus grand en surface)
    rect = rects[0]

    # shape : objet dlib contenant les 68 points
    shape = predictor(frame_gray, rect)

    # face_utils.shape_to_np convertit l'objet dlib en ndarray (68, 2)
    landmarks = face_utils.shape_to_np(shape)

    return landmarks, rect


def compute_ear(eye_points):

    # Distances verticales
    d1 = euclidean(eye_points[1], eye_points[5])  # P2 - P6
    d2 = euclidean(eye_points[2], eye_points[4])  # P3 - P5

    # Distance horizontale
    d3 = euclidean(eye_points[0], eye_points[3])  # P1 - P4

    ear = (d1 + d2) / (2.0 * d3)
    return ear


def compute_mar(mouth_points):

    # Distances verticales 
    d1 = euclidean(mouth_points[2],  mouth_points[10])  # haut-bas gauche
    d2 = euclidean(mouth_points[3],  mouth_points[9])   # haut-bas centre
    d3 = euclidean(mouth_points[4],  mouth_points[8])   # haut-bas droit

    # Distance horizontale (largeur bouche)
    d4 = euclidean(mouth_points[0],  mouth_points[6])

    mar = (d1 + d2 + d3) / (2.0 * d4)
    return mar


def compute_avg_ear(landmarks):

    right_eye = landmarks[RIGHT_EYE]
    left_eye  = landmarks[LEFT_EYE]

    ear_right = compute_ear(right_eye)
    ear_left  = compute_ear(left_eye)

    avg_ear = (ear_right + ear_left) / 2.0
    return avg_ear, right_eye, left_eye


def compute_mouth_mar(landmarks):
 
    mouth_pts = landmarks[MOUTH]
    mar = compute_mar(mouth_pts)
    return mar, mouth_pts