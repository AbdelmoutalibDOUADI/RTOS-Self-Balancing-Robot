import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# Configuration du port série
PORT = "COM3"  # Remplacez par le nom de votre port COM
BAUDRATE = 9600  # Vitesse de communication (doit correspondre à la config de l'appareil)

# Nombre maximal de points à afficher sur la courbe
MAX_POINTS = 100

# Initialisation de la connexion série
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
    print(f"Connecté au port {PORT} avec un débit de {BAUDRATE} bauds.")
except Exception as e:
    print(f"Impossible de se connecter au port série : {e}")
    exit()

# File pour stocker les données reçues
data = deque([0] * MAX_POINTS, maxlen=MAX_POINTS)

# Fonction pour lire les données du port série
def read_serial_data():
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode('ascii').strip()  # Lire une ligne et supprimer les espaces
            return float(line)  # Convertir en float
    except Exception as e:
        print(f"Erreur lors de la lecture des données : {e}")
        return None

# Fonction d'animation pour mettre à jour la courbe
def update(frame):
    global data
    value = read_serial_data()
    if value is not None:
        data.append(value)  # Ajouter la nouvelle valeur
    line.set_data(range(len(data)), data)  # Mettre à jour les données de la courbe
    ax.relim()  # Réadapter les axes
    ax.autoscale_view()  # Ajuster la vue
    return line,

# Configuration du tracé avec Matplotlib
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)
ax.set_title("Données en temps réel")
ax.set_xlabel("Temps")
ax.set_ylabel("Valeur")
ax.grid()

# Animation Matplotlib
ani = animation.FuncAnimation(fig, update, interval=50)  # Intervalle de 50 ms

try:
    plt.show()
except KeyboardInterrupt:
    print("Arrêt du programme.")
finally:
    ser.close()
    print("Port série fermé.")

