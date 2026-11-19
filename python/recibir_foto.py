"""User-supplied two-camera receiver, normalized from a chat paste (2026-11-19).

Close Arduino IDE, Serial Monitor, Serial Plotter and other serial clients first.
COM13 / 115200 are the reported bench defaults; edit PUERTO for your machine.
No hardware revalidation is implied by the host-side tests in this repository.
"""
import serial
import time
import re
from pathlib import Path

# PI LOM - RECEPTOR DE DOS CAMARAS
PUERTO = "COM13"
BAUDIOS = 115200
MAX_JPEG = 2_000_000
CARPETA = Path("imagenes")
CARPETA.mkdir(exist_ok=True)


def siguiente_numero():
    numeros = []
    for archivo in CARPETA.glob("camara*_*.jpg"):
        resultado = re.fullmatch(r"camara[12]_(\d+)\.jpg", archivo.name)
        if resultado:
            numeros.append(int(resultado.group(1)))
    return max(numeros, default=0) + 1


def esperar_ready(esp):
    limite = time.monotonic() + 30
    while time.monotonic() < limite:
        linea = esp.readline().decode("ascii", errors="replace").strip()
        if linea:
            print("[ESP32]", linea)
        if linea == "READY":
            return
        if linea.startswith("ERROR"):
            raise RuntimeError(linea)
    raise TimeoutError("No se recibio READY")


def leer_exacto(esp, cantidad):
    datos = bytearray()
    while len(datos) < cantidad:
        bloque = esp.read(min(4096, cantidad - len(datos)))
        if not bloque:
            raise TimeoutError(f"Recepcion incompleta: {len(datos)}/{cantidad}")
        datos.extend(bloque)
    return bytes(datos)


def leer_linea(esp):
    linea = esp.readline()
    if not linea:
        raise TimeoutError("Tiempo de espera agotado")
    return linea.decode("ascii", errors="replace").strip()


def guardar_jpeg(datos, camara, numero):
    inicio = datos.find(b"\xff\xd8")
    fin = datos.rfind(b"\xff\xd9")
    if inicio < 0 or fin <= inicio:
        ruta = CARPETA / f"camara{camara}_{numero:04d}_invalida.bin"
        ruta.write_bytes(datos)
        raise ValueError(f"JPEG invalido. Datos en {ruta}")
    imagen = datos[inicio:fin + 2]
    ruta = CARPETA / f"camara{camara}_{numero:04d}.jpg"
    # No sobrescribir fotografias
    with ruta.open("xb") as archivo:
        archivo.write(imagen)
    print(f"[OK] {ruta}")
    print(f"     JPEG: {len(imagen)} bytes")
    print(f"     Bytes descartados: {len(datos) - len(imagen)}")
    return ruta


def capturar_dos(esp):
    numero = siguiente_numero()
    print("\n[ESP32] Solicitando dos fotografias")
    esp.write(b"F\n")
    esp.flush()
    for camara_esperada in (1, 2):
        encabezado = leer_linea(esp)
        if encabezado.startswith("ERROR"):
            raise RuntimeError(encabezado)
        partes = encabezado.split()
        if len(partes) != 3 or partes[0] != "JPEG":
            raise RuntimeError(f"Encabezado inesperado: {encabezado}")
        camara = int(partes[1])
        total = int(partes[2])
        if camara != camara_esperada:
            raise RuntimeError(f"Se esperaba camara {camara_esperada}, llego {camara}")
        if not 0 < total <= MAX_JPEG:
            raise ValueError(f"Tamano invalido: {total}")
        print(f"[CAM{camara}] Recibiendo {total} bytes...")
        # Consume ALL announced bytes, even if the JPEG ends earlier.
        datos = leer_exacto(esp, total)
        fin = leer_linea(esp)
        if fin != f"END {camara}":
            raise RuntimeError(f"Final inesperado: {fin}")
        guardar_jpeg(datos, camara, numero)
    confirmacion = leer_linea(esp)
    if confirmacion != "DONE":
        raise RuntimeError(f"Confirmacion inesperada: {confirmacion}")
    print(f"\n[LISTO] Pareja {numero:04d} recibida")


def main():
    print("=" * 42)
    print(" PI LOM - DOS CAMARAS")
    print("=" * 42)
    print(f"[USB] Conectando a {PUERTO}")
    with serial.Serial(PUERTO, BAUDIOS, timeout=15, write_timeout=10) as esp:
        esperar_ready(esp)
        print("[SISTEMA] Camaras listas")
        while True:
            print("\nENTER - Capturar ambas camaras")
            print("S     - Salir")
            opcion = input("Opcion: ").strip().lower()
            if opcion == "s":
                break
            if opcion == "":
                try:
                    capturar_dos(esp)
                except (TimeoutError, ValueError, RuntimeError, serial.SerialException) as error:
                    print("[ERROR]", error)
                    print("Reinicia el ESP32 y Python antes de repetir.")
                    break


if __name__ == "__main__":
    try:
        main()
    except (serial.SerialException, OSError, TimeoutError, RuntimeError) as error:
        print("[ERROR]", error)
    except KeyboardInterrupt:
        print("\n[SISTEMA] Interrumpido")
