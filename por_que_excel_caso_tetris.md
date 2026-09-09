# Por qué Excel (VBA) es una herramienta válida para simular y gamificar el flujo de memoria — El caso de Tetris

## 1. Definición de la herramienta

**Microsoft Excel** es una hoja de cálculo cuya estructura fundamental es una **grilla de celdas** (filas × columnas), donde cada celda es un objeto con propiedades propias: valor, color de relleno (`Interior.Color`), borde, fuente, posición, etc.

**VBA (Visual Basic for Applications)** es el lenguaje de programación integrado en Excel que permite controlar esos objetos mediante código: leer y escribir valores, cambiar colores, reaccionar a eventos (teclas, clics, tiempo) y ejecutar lógica (condicionales, bucles, funciones).

Juntos, Excel + VBA dejan de ser "una calculadora con tablas" y se convierten en un **entorno de programación con salida gráfica ya integrada**: la grilla de celdas actúa como una matriz de píxeles o casillas que se puede pintar y mover en tiempo real, sin necesidad de librerías gráficas externas.

---

## 2. Por qué se eligió Excel para este proyecto

| Razón | Detalle |
|---|---|
| **Ya está disponible** | No requiere instalar un motor de videojuegos ni un IDE gráfico nuevo; casi cualquier computadora con Office ya lo tiene. |
| **La grilla es gratis** | Un tablero de juego (10×20, 8×8, lo que sea) *ya existe* en forma de celdas; en otros lenguajes hay que construir el sistema de coordenadas y el renderizado desde cero. |
| **VBA da control real** | Bucles, condicionales, arrays, tipos definidos (`Type`), temporizadores (`Application.OnTime`) y captura de teclado (`Application.OnKey`) — todo lo necesario para un juego real, no solo una demo. |
| **Curva de aprendizaje accesible** | Comparado con programar gráficos en C++/SDL o incluso JavaScript + Canvas, VBA es más simple para alguien que recién está aprendiendo lógica de programación. |
| **Es un hecho comprobado, no una suposición** | Ya existen juegos completos y jugables hechos en Excel — el más citado como prueba de concepto es **Tetris**, que se analiza a continuación. |

---

## 3. Caso de estudio: cómo funciona Tetris hecho en Excel

Tetris es el ejemplo más usado para demostrar que Excel puede ser un motor de juego real, porque necesita **todos** los elementos que un juego necesita: tablero, piezas, gravedad, colisiones, entrada del usuario, puntaje y dificultad progresiva. Así se resuelve cada parte:

### 3.1 El tablero
Un rango de celdas, por ejemplo `B2:K21` (10 columnas × 20 filas), representa el área de juego. Cada celda vacía = espacio libre; cada celda coloreada = bloque ocupado.

```
Fila 1   [ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]
Fila 2   [ ][ ][ ][■][■][ ][ ][ ][ ][ ]
Fila 3   [ ][ ][ ][■][■][ ][ ][ ][ ][ ]
  ...
Fila 20  [■][■][ ][■][■][■][■][ ][■][■]   <- fila casi completa
```

### 3.2 Las piezas (tetrominós)
Cada pieza (I, O, T, S, Z, J, L) se define como un conjunto de coordenadas relativas a un punto de origen, usando un `Type`:

```vb
Type Coordenada
    Fila As Integer
    Columna As Integer
End Type

' Pieza "T", 4 bloques relativos al punto de origen (0,0)
Dim PiezaT(1 To 4) As Coordenada
PiezaT(1) = Coordenada(0, 0)
PiezaT(2) = Coordenada(0, 1)
PiezaT(3) = Coordenada(0, 2)
PiezaT(4) = Coordenada(1, 1)
```

### 3.3 El bucle de juego (game loop)
`Application.OnTime` es la pieza clave: reprograma la ejecución de una macro para dentro de X segundos, generando un "tick" constante — el equivalente a los *frames* de un juego real:

```vb
Sub TickDelJuego()
    If Not HayColision(FilaActual + 1, ColumnaActual) Then
        BorrarPiezaDeCeldas
        FilaActual = FilaActual + 1
        DibujarPiezaEnCeldas
    Else
        FijarPiezaEnTablero
        RevisarLineasCompletas
        GenerarNuevaPieza
    End If

    Application.OnTime Now + TimeSerial(0, 0, VelocidadActual), "TickDelJuego"
End Sub
```

`VelocidadActual` se reduce a medida que sube el nivel, haciendo que el juego se sienta más rápido — el mismo principio de dificultad progresiva de cualquier Tetris real.

### 3.4 Render (dibujar en pantalla)
En cada tick se borra la posición anterior de la pieza y se pinta la nueva, usando `Interior.Color` sobre el rango del tablero:

```vb
Sub DibujarPiezaEnCeldas()
    Dim i As Integer
    For i = 1 To 4
        Sheets("Tablero").Cells(FilaActual + PiezaActual(i).Fila, _
                                 ColumnaActual + PiezaActual(i).Columna).Interior.Color = RGB(0, 112, 192)
    Next i
End Sub
```

### 3.5 Colisiones
Antes de mover la pieza, se revisa si la nueva posición está fuera del tablero o ya ocupada por un bloque fijo (comparando contra una matriz que guarda el estado "congelado" del tablero, separado de la pieza en movimiento):

```vb
Function HayColision(filaDestino As Integer, columnaDestino As Integer) As Boolean
    If filaDestino > 20 Or columnaDestino < 1 Or columnaDestino > 10 Then
        HayColision = True
        Exit Function
    End If
    If EstadoTablero(filaDestino, columnaDestino) = True Then
        HayColision = True
    End If
End Function
```

### 3.6 Entrada del jugador
`Application.OnKey` asigna teclas a subrutinas — así se mueve o rota la pieza sin usar el mouse:

```vb
Sub ConfigurarControles()
    Application.OnKey "{LEFT}", "MoverIzquierda"
    Application.OnKey "{RIGHT}", "MoverDerecha"
    Application.OnKey "{DOWN}", "CaerRapido"
    Application.OnKey "{UP}", "RotarPieza"
End Sub
```

### 3.7 Líneas completas y puntaje
Se recorre cada fila revisando si las 10 celdas están ocupadas; si es así, esa fila se elimina y todo lo de arriba "baja" una posición (shift), sumando puntos:

```vb
Sub RevisarLineasCompletas()
    Dim fila As Integer, columna As Integer, completa As Boolean
    For fila = 20 To 1 Step -1
        completa = True
        For columna = 1 To 10
            If EstadoTablero(fila, columna) = False Then
                completa = False
                Exit For
            End If
        Next columna
        If completa Then
            EliminarFilaYBajarSuperiores fila
            Puntaje = Puntaje + 100
        End If
    Next fila
End Sub
```

### 3.8 Diagrama del ciclo completo

```
        ┌──────────────────────────┐
        │      Application.OnTime  │  <- "tick" cada N milisegundos
        └────────────┬─────────────┘
                      ▼
        ┌──────────────────────────┐
        │  ¿Hubo tecla presionada?  │ <- Application.OnKey
        └────────────┬─────────────┘
                      ▼
        ┌──────────────────────────┐
        │   Mover / rotar pieza     │
        └────────────┬─────────────┘
                      ▼
        ┌──────────────────────────┐
        │   ¿Hay colisión?          │
        └──────┬─────────────┬─────┘
             No│             │Sí
               ▼             ▼
      ┌────────────────┐  ┌─────────────────────┐
      │ Redibujar pieza │  │ Fijar pieza, revisar │
      │  en su lugar    │  │ líneas, sumar puntos │
      └────────┬────────┘  └──────────┬───────────┘
               │                      ▼
               │           ┌──────────────────────┐
               │           │  Generar nueva pieza  │
               │           └──────────┬────────────┘
               ▼                      ▼
        ┌──────────────────────────────────┐
        │   Volver a programar el OnTime    │
        └──────────────────────────────────┘
```

Este ciclo — **tick → input → mover → colisión → fijar/limpiar → redibujar → reprogramar tick** — es la estructura universal de cualquier juego en tiempo real, y es exactamente la misma que se necesita para el juego de memoria.

---

## 4. Aplicando la misma estructura a la simulación de memoria ("Cache Runner")

Como Tetris ya demuestra que Excel soporta un juego completo en tiempo real, la misma arquitectura se reutiliza para representar el flujo de memoria, cambiando solo el significado de cada pieza:

| Elemento en Tetris | Equivalente en Cache Runner (memoria) |
|---|---|
| Tablero 10×20 | Tablero con "carriles" horizontales: Registros → Caché L1 → RAM → Disco |
| Pieza que cae | "Paquete de datos" que el jugador mueve entre niveles |
| `Application.OnTime` (velocidad de caída) | `Application.OnTime` como reloj de la simulación (representa ciclos de CPU) |
| `Application.OnKey` (mover/rotar pieza) | `Application.OnKey` para decidir a qué dirección de memoria acceder |
| Colisión con el fondo o bloques fijos | Verificar si la dirección solicitada está en caché (**hit**) o no (**miss**) |
| Limpiar líneas completas → sumar puntos | Acertar un **hit** → sumar puntos; forzar un **miss** → penalizar tiempo/puntos |
| Aumentar velocidad por nivel | Aumentar la exigencia (menos tiempo, direcciones más dispersas) a medida que sube el puntaje |
| Colores por tipo de pieza | Colores por resultado: verde = hit en caché, amarillo = miss resuelto en RAM, rojo = fallo de página (disco) |

### 4.1 Ejemplo de cómo se vería el tablero del juego (mockup en texto)

```
 REGISTROS   [R1][R2][R3][R4][R5][R6][R7][R8]
 CACHÉ L1    [░░][▓▓][░░][░░][▓▓][░░][░░][░░][░░][░░][░░][░░][░░][░░][░░][░░]
 RAM         [ 0][ 1][ 2][ 3][ 4][ 5][ 6][ 7][ 8][ 9][10][11][12][13][14][15] ...
 DISCO       [………………………………… memoria secundaria (muy lenta) …………………………………]

 Paquete de datos: ●   Dirección solicitada: 37   Resultado: MISS → buscando en RAM...
 Puntaje: 480        Ciclos usados: 214        Nivel: 3
```

### 4.2 Núcleo del bucle de juego reutilizado (adaptado de Tetris)

```vb
Sub TickDeSimulacion()
    ContadorCiclos = ContadorCiclos + 1

    If HayPeticionPendiente Then
        ResultadoAcceso = BuscarDato(DireccionSolicitada)   ' misma función de la investigación anterior
        PintarResultado ResultadoAcceso                     ' verde / amarillo / rojo
        ActualizarPuntaje ResultadoAcceso
    End If

    If ContadorCiclos Mod 20 = 0 Then
        SubirDificultad     ' reduce el tiempo disponible, como Tetris aumenta la velocidad de caída
    End If

    Application.OnTime Now + TimeSerial(0, 0, VelocidadTick), "TickDeSimulacion"
End Sub
```

La función `BuscarDato`, la búsqueda en cascada (caché → RAM → disco) y el registro en el log son exactamente los mismos que se diseñaron en la investigación de la simulación de memoria — lo único nuevo aquí es el **envoltorio de juego** (tick, teclado, puntaje, dificultad progresiva) tomado directamente de cómo funciona Tetris.

---

## 5. Conclusión

Excel + VBA no es una simplificación forzada ni una analogía débil: es una plataforma donde ya existen juegos completos y exigentes técnicamente (Tetris, Snake, incluso motores tipo Doom), lo que confirma que tiene todo lo necesario — bucle de tiempo real, entrada de teclado, renderizado por color y detección de colisiones — para representar de forma jugable el flujo real de memoria en el hardware. El diseño de **Cache Runner** no inventa una técnica nueva: reutiliza exactamente la misma arquitectura que hace posible Tetris en Excel, reemplazando "piezas que caen" por "datos que se buscan en la jerarquía de memoria".
