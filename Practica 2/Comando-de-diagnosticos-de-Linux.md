# Practico 2

---

## 1. Comando `lscpu` (Información del Procesador)

Este comando desglosa la arquitectura, topología y capacidades del procesador físico y su respectiva asignación en el hipervisor de la máquina virtual.

### Arquitectura y Capacidades
* **`Architecture: x86_64`**: El sistema operativo y el procesador están operando bajo la arquitectura de 64 bits.
* **`CPU op-mode(s): 32-bit, 64-bit`**: El procesador soporta la ejecución nativa de programas tanto de 32 como de 64 bits.
* **`Address sizes: 39 bits physical, 48 bits virtual`**:
  * **Físico:** El hardware puede manejar direcciones físicas de hasta 39 bits (permite mapear hasta 512 GB de RAM real).
  * **Virtual:** El sistema operativo puede asignar memoria virtual a los procesos usando direcciones de hasta 48 bits (hasta 256 TB de memoria virtual por proceso).
* **`Byte Order: Little Endian`**: Define cómo se guardan los datos en la memoria; el byte menos significativo se almacena en la dirección de memoria más baja.

### Topología y Modelo
* **`CPU(s): 2`**: Hay 2 procesadores lógicos asignados a esta máquina virtual.
* **`On-line CPU(s) list: 0,1`**: Ambos núcleos están activos y funcionales en el sistema.
* **`Vendor ID: GenuineIntel`**: El fabricante de la CPU física es Intel.
* **`Model name: Intel(R) Core(TM) i7-10510U CPU @ 1.80GHz`**: Es el modelo físico del host. Un procesador de 10.ª generación diseñado para portátiles ("U" = Ultra-low power), con una frecuencia base de 1.80 GHz.
* **`CPU family / Model / Stepping`**: Identificadores internos (`6`, `142`, `12`) que usa el kernel para aplicar parches específicos de microcódigo según la generación exacta del chip de silicio.
* **`Thread(s) per core: 1` / `Core(s) per socket: 2`**: Se dispone de 2 núcleos físicos virtuales en 1 solo zócalo (socket), sin hilos adicionales de Hyper-Threading expuestos al invitado.
* **`BogoMIPS: 4608.01`**: Métrica sintética (Bogus MIPS) que Linux calcula durante el arranque para temporizar ciclos de espera internos del kernel.

### Banderas (Flags) y Virtualización
* **`Flags`**: Lista extensa de instrucciones soportadas por la CPU. Destacan `fpu` (operaciones de punto flotante), `sse`/`sse2`/`sse4_1` (operaciones vectoriales multimedia), `aes` (cifrado acelerado por hardware) e `hypervisor` (bandera que indica al SO que está siendo virtualizado).
* **`Hypervisor vendor: KVM` / `Virtualization type: full`**: El entorno de virtualización se identifica como KVM bajo un esquema de virtualización completa del hardware.
* **`Caches (L1d, L1i, L2, L3)`**: Distribución de la memoria ultrarrápida del procesador. Existen cachés separadas para datos (`L1d`) e instrucciones (`L1i`) de 64 KiB, una caché de nivel intermedio L2 de 512 KiB y una memoria L3 compartida de 16 MiB.

### NUMA y Vulnerabilidades
* **`NUMA node(s): 1`**: Acceso a Memoria No Uniforme. Al existir un solo nodo, ambas CPUs (`0` y `1`) acceden a la totalidad de la RAM a la misma velocidad sin cuellos de botella de interconexión.
* **`Vulnerabilities`**: Muestra cómo el kernel de Linux responde a fallos de hardware conocidos a nivel arquitectónico (como Spectre o Meltdown). La mayoría de los estados reportan `Not affected` (hardware no vulnerable) o `Mitigation` (parcheado a nivel de software/microcódigo). El indicador `Spec store bypass: Vulnerable` señala que esta mitigación específica no se ha forzado en las configuraciones de la VM.

---

## 2. Comando `lsblk` (Dispositivos de Bloques)

Este comando genera el mapa de dispositivos de almacenamiento por bloques, evidenciando cómo el sistema estructura los discos, particiones y volúmenes montados.

### Los dispositivos `loop` (0 al 15)
* **`NAME: loop0 - loop15`**: Los dispositivos *loopback* son archivos contenedores que el sistema trata lógicamente como si fuesen discos duros físicos.
* **Uso en el Sistema (Snap)**: En la arquitectura de Ubuntu, estos se emplean para montar aplicaciones aisladas instaladas vía Snap (`core22`, `firefox`, `snap-store`, etc.). Están marcados con el atributo `RO = 1` (Solo lectura), ya que funcionan como contenedores inmutables que se ejecutan protegidos del resto del sistema operativo (sandboxing).

### El Disco Principal (`sda`)
* **`sda` (500G, `disk`)**: Representa el disco duro virtual principal, provisionado con una capacidad de 500 Gigabytes.
  * **`sda1` (1M, `part`)**: Partición inicial de 1 Megabyte de capacidad. Opera como la partición "BIOS Boot", una estructura requerida por el gestor de arranque GRUB cuando se implementa una tabla de particiones moderna GPT sobre un sistema de arranque heredado (sin UEFI puro).
  * **`sda2` (500G, `part`)**: Partición lógica principal del sistema operativo.
    * **`MOUNTPOINTS: /`**: Se encuentra montada en la raíz del sistema de archivos. Contiene la totalidad del SO, las librerías, los binarios de usuario y el espacio de trabajo. Adicionalmente, comparte un punto de montaje específico para los diccionarios ortográficos (`/var/snap/firefox/common/host-hunspell`).

### El CD-ROM (`sr0`)
* **`sr0` (1024M, `rom`)**: Interfaz óptica virtual. El parámetro `RM = 1` especifica que es un dispositivo extraíble (Removable). Posee un tamaño de lectura de bloque predeterminado de 1024 Megabytes.

---

## 3. Comando `lsmem` (Mapa de Memoria RAM)

Herramienta que lista los bloques de memoria dinámica disponibles y sus rangos de direccionamiento físico.

### Rangos Hexadecimales
Demuestra cómo el hardware mapea las direcciones de la RAM:
* El **primer bloque** continuo de `3.5G` abarca desde la dirección base `0x00...00` hasta `0x00...dfffffff`.
* El **segundo bloque** de `512M` salta a la dirección alta `0x0000000100000000`.
* *Justificación Técnica:* Este salto es un estándar de la arquitectura x86. El hardware reserva un segmento de direcciones justo por debajo del límite de los 4 GB para mapear operaciones MMIO (como la memoria de la tarjeta de video). La memoria RAM que entra en conflicto con este espacio es "empujada" a direcciones superiores.

### Parámetros de Gestión de Memoria
* **`Memory block size: 128M`**: Tamaño de granularidad lógica que utiliza el kernel de Linux para operar procesos de conexión o desconexión en caliente ("hotplug") de memoria dinámica.
* **`Total online memory: 4G`**: Certifica que existen exactamente 4 Gigabytes de memoria RAM en estado activo, asignados y plenamente operativos.

---

## 4. Comando `sudo lshw` (Árbol Completo del Hardware)

Ejecutado con privilegios de superusuario, este comando consulta los nodos de `/sys`, `/proc` y las tablas SMBIOS para generar la jerarquía topológica de todos los componentes de la placa base virtualizada.

### Componentes Base del Sistema
* **`osboxes (Computer)`**: Identifica la máquina globalmente como una entidad virtual (`family=Virtual Machine`) provisionada por `innotek GmbH` (desarrolladores originales del software VirtualBox). Posee un UUID único de vinculación.
* **`*-core (Motherboard)`**: Nomenclatura del bus o placa madre central emulada por Oracle.
* **`*-firmware (BIOS)`**: El firmware de arranque del sistema es una BIOS heredada (fechada el 12/01/2006) con un tamaño de 128 KiB. Declara soporte fundamental de hardware como el arranque óptico (`cdboot`) y manejo primitivo de interrupciones de teclado y video (`int9keyboard`, `int10video`).
* **`*-memory`**: Reconfirma el subsistema de RAM con una capacidad de 4 GiB mapeada directamente en el identificador físico 1 (`physical id: 1`).
* **`*-cpu`**: Detalla la inyección de la CPU (`i7-10510U`), su ubicación troncal en el hardware (`bus info: cpu@0`), su ancho de registro de `64 bits` e indica el nivel del parche de microcódigo inyectado para la gestión de seguridad especulativa (`microcode=4294967295`).

### Puentes (Bridges) y Controladores Clásicos
* **`*-pci` (Host bridge)**: Controlador `440FX - 82441FX PMC`. Ejerce la función de "Puente Norte" clásico (Northbridge), interconectando el procesador con la RAM y el bus PCI a una frecuencia nativa de `33MHz` con ancho de `32 bits`.
* **`*-isa` (ISA bridge)**: Controlador `82371SB PIIX3 ISA`. Actúa como "Puente Sur" (Southbridge) para garantizar la retrocompatibilidad con interfaces periféricas de baja velocidad.
* **`*-bridge` (Power Management)**: Controlador `82371AB/EB/MB PIIX4 ACPI`. Módulo vital que gobierna la interfaz avanzada de energía (ACPI), permitiendo la interpretación de señales hipervisor-invitado (ej. comandos de apagado o suspensión controlada).
* **`*-pnp00:00 / 01` (Plug and Play)**: Integración de los controladores de hardware legados para el teclado y ratón genérico a través del driver `i8042`.
* **`*-ide` / `*-cdrom`**: Interfaz de almacenamiento IDE virtual (`ata_piix`) a la cual se encuentra enlazada la unidad lectora de DVD-ROM (`/dev/sr0`).

### Pantalla, Red y Multimedia
* **`*-display` (Adaptador Gráfico)**: Integración del adaptador `SVGA II` de VMware.
  * **Driver:** `vmwgfx`. Confirma que Linux utiliza los controladores de aceleración visual optimizados para entornos virtuales.
  * **Resolución:** Opera a una resolución lógica de `1280x800` con profundidad de 32 bits de renderizado.
* **`*-network` (Interfaz de Red)**: Adaptador Gigabit `82540EM Gigabit Ethernet Controller`.
  * **Lógica:** Reconocida por el kernel bajo el nombre de interfaz `enp0s3`.
  * **Dirección MAC:** Identificada bajo el serial `08:00:27:3c:cb:05`.
  * **Conectividad:** Enlace detectado en modo Full-Duplex bajo una configuración de Red de Traducción de Direcciones (NAT), con la IP interna asignada `10.0.2.15`.
* **`*-multimedia` (Controlador de Audio)**: Chip de sonido `82801AA AC'97`. Instancia las rutas lógicas del subsistema ALSA (`/dev/snd/controlC0`, `/dev/snd/pcmC0D0c`), garantizando que los nodos de modulación y captura digital funcionan adecuadamente.

### Integración y Subsistemas USB
* **`*-generic` (System peripheral)**: Dispositivo clave registrado como `VirtualBox mouse integration` (manejado por el driver `vboxguest`). Constituye el núcleo de las *Guest Additions*, permitiendo la transición ininterrumpida del puntero del ratón entre el SO anfitrión y la ventana virtual.
* **`*-usb:0` (Controlador USB 1.1)**: Chip `KeyLargo/Intrepid USB` de Apple bajo el estándar OHCI. Opera a baja velocidad (`12Mbit/s`) para soportar periféricos antiguos sin cuellos de botella de compatibilidad.
* **`*-usb:1` / `*-usbhost` (Controlador USB 2.0)**: Chip `ICH6 Family USB2 EHCI`. Despliega capacidades de Alta Velocidad (`480Mbit/s`) administrando un hub lógico de hasta 12 puertos de interconexión.
* **`*-usb` (Tableta USB)**: Dispositivo `VirtualBox USB Tablet`. Trabaja en tándem con la integración del ratón, transmitiendo coordenadas cartesianas absolutas para alinear de forma milimétrica el cursor de la máquina host con la máquina guest.

### Controladores de Almacenamiento y Discos
* **`*-sata` (Controlador de Almacenamiento)**: Controlador `ICH8M/ICH8M-E SATA` operando en modo `AHCI`. Esta interfaz avanzada garantiza una alta eficiencia y colas de comandos en las operaciones de entrada/salida sobre discos magnéticos o sólidos virtuales, utilizando el driver nativo `ahci`.
* **`*-disk` (Disco Duro Virtual)**: Dispositivo físico lógico `VBOX HARDDISK`.
  * **Identificador:** Mapeado en `/dev/sda`.
  * **Capacidad Comercial y Real:** Declara un total de `500GiB` (Gibibytes), lo que el hardware de usuario interpreta como `536GB`.
  * **Estructura:** Utiliza una tabla de particionamiento GUID (`capabilities: partitioned:gpt`).

### Volúmenes Lógicos y Sistemas de Archivos
* **`*-volume:0` (Partición BIOS Boot)**:
  * **Identificador lógico:** `/dev/sda1` con un tamaño de `1023KiB`.
  * **Función Operativa:** Está declarada como "sin sistema de archivos" (`nofs`). Esta área está reservada exclusivamente para la incrustación del gestor de arranque secundario de GRUB, al existir un esquema GPT sin firmware UEFI.
* **`*-volume:1` (Volumen EXT4 de Datos)**:
  * **Identificador lógico:** `/dev/sda2` con un tamaño de `499GiB`.
  * **Sistema de Archivos:** Cuarto sistema de archivos extendido (`EXT4`).
  * **Características:** Cuenta con `journaled` (registro de transacciones tolerante a fallos de energía), `extents` (mapeo lógico para evitar fragmentación) y `huge_files`. El estado confirma que está activo, en estado de lectura/escritura (`rw`) y montado en el nodo de la raíz del sistema operativo (`/`).

### Dispositivos de Entrada Específicos (Input Subsystem)
* **`*-input:0` (Power Button)**: Actúa como el dispositivo ACPI encargado de capturar e interpretar la instrucción hipervisora de apagado ordenado (`/dev/input/event0`).
* **`*-input:1` (Sleep Button)**: Recibe rutinas avanzadas de suspensión para el cambio de estado de energía del procesador (`/dev/input/event1`).
* **`*-input:2` (Teclado PS/2)**: `AT Translated Set 2 keyboard`. Gestiona y expone de manera independiente los estados físicos de iluminación de las teclas modificadoras: `capslock`, `numlock` y `scrolllock`.
* **`*-input:3` (Video Bus)**: Responsable de alertar a los demonios del sistema (`/dev/input/event3`) ante eventos físicos de visualización, como el redimensionamiento de la ventana host para forzar el autoajuste de la resolución.
* **`*-input:4` (Ratón Emulado PS/2)**: `ImExPS/2 Generic Explorer Mouse`. Mapeado en `/dev/input/mouse0`, actúa como dispositivo de puntero primario en etapas tempranas de arranque o de respaldo en caso de que la inyección USB de las *Guest Additions* sufra una falla del módulo.