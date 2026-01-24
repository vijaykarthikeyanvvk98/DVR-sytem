import pyaudio
import socket
import numpy as np

# Audio settings
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
CHUNK = 128  # Number of audio samples per frame

# Network settings
UDP_IP = "192.168.56.2"  # IP address of the Arduino
UDP_PORT = 8888          # Port to send audio data to

# Set up the audio stream
audio = pyaudio.PyAudio()
stream = audio.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK)

# Set up the UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("Streaming audio to Arduino...")

try:
    while True:
        # Read audio data from the microphone
        data = stream.read(CHUNK)
        
        # Convert audio data to numpy array for processing (optional)
        audio_data = np.frombuffer(data, dtype=np.int16)

        # Send the audio data via UDP
        sock.sendto(data, (UDP_IP, UDP_PORT))
        
except KeyboardInterrupt:
    print("Streaming stopped.")

finally:
    # Clean up
    stream.stop_stream()
    stream.close()
    audio.terminate()
    sock.close()