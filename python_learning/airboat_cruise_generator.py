import csv
import math
import random

# Parametry logów
duration = 180  # 3 minuty
dt = 0.04  # 25 Hz
rows = int(duration / dt)

filename = "airboat_3min_test.csv"

# Startowe wartości GPS (okolice Wrocławia)
lat = 51.107800
lon = 17.038500
speed = 15.0

with open(filename, mode='w', newline='') as file:
    writer = csv.writer(file, delimiter=';')
    writer.writerow(["Time", "Roll", "Pitch", "Heading", "Latitude", "Longitude", "Speed", "GpsCourse", "Sats"])

    for i in range(rows):
        time = i * dt

        # Symulacja fal: połączenie wolnego przechyłu na fali i szybkich wibracji silnika
        roll = math.sin(time * 2.5) * 3.0 + random.uniform(-0.5, 0.5)
        pitch = math.sin(time * 1.8) * 2.0 + random.uniform(-0.3, 0.3)

        # Symulacja zataczania wolnego koła na wodzie
        heading = (90 + time * 2) % 360  # Obraca się o 2 stopnie na sekundę

        # GPS z lekkim znoszeniem wiatru (+3 stopnie)
        gps_course = (heading + 3.0) % 360

        # Prosta symulacja ruchu na podstawie kursu
        lat += math.cos(math.radians(gps_course)) * 0.000001
        lon += math.sin(math.radians(gps_course)) * 0.000001

        sats = int(random.uniform(9, 12))  # Zmiana satelitów w czasie

        writer.writerow([
            f"{time:.3f}",
            f"{roll:.2f}",
            f"{pitch:.2f}",
            f"{heading:.1f}",
            f"{lat:.6f}",
            f"{lon:.6f}",
            f"{speed:.1f}",
            f"{gps_course:.1f}",
            sats
        ])

print(f"Wygenerowano plik: {filename} ({rows} wierszy)")