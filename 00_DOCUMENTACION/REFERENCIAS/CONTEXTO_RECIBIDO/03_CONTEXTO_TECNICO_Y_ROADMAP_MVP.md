# Contexto técnico y roadmap del MVP (fuente de contexto)

> Importado el 17 de septiembre de 2026. Describe el estado y las decisiones técnicas comunicadas en ese momento; debe contrastarse con el código y el tablero operativo antes de ejecutar tareas.

Sí. Para elaborar el roadmap conviene separar **dos proyectos que se conectan entre sí**: el **generador de Lima/SJL** y el **juego que corre sobre esa ciudad**. No deberíamos seguir agregando cosas aleatoriamente al `LimaCityGenerator`; necesitamos una arquitectura donde ciudad, tráfico, peatones, vehículos, crimen, policía, armas, economía, etc. sean sistemas independientes.

## 1. Visión actual de Kalex Urban

El objetivo que hemos venido definiendo es un juego de mundo abierto ambientado en Lima, inicialmente en **San Juan de Lurigancho / Canto Grande**, con una experiencia tipo sandbox urbano: caminar, conducir, entrar/salir de vehículos, interactuar con NPC, cometer delitos, ser perseguido por policías, comprar combustible, usar armas, robar, tener tráfico y peatones, etc.

El MVP no pretende terminar toda Lima. La idea es construir **una zona suficientemente pequeña pero funcional**, donde casi todos los sistemas importantes tengan una primera versión jugable.

Actualmente estás en **Unreal Engine 5.6**, con proyecto C++ y Blueprints.

---

# 2. Lo que ya existe

## A. Pipeline de ciudad real

Ya tienes un pipeline externo que termina produciendo un:

```text
03_CITY_DATA/
└── SJL_CantoGrande_MVP/
    └── processed/
        └── city.json
```

`LimaCityGenerator` puede cargar ese JSON desde una ruta configurable. Además trabajas con escala estándar de Unreal de `100 UU = 1 metro`. 

El JSON ya representa, como mínimo:

```text
City
├── Origin
├── Roads
├── Blocks
├── Lots
├── Buildings
└── Parks
```

En nuestras pruebas llegamos a trabajar con aproximadamente:

```text
Roads      201
Blocks      53
Lots      1047
Buildings   27 OSM originales
Parks       11
```

y el generador procedural llegó a generar del orden de **950 edificios** a partir de los lotes.

Las carreteras además ya contienen datos muy útiles para el futuro sistema de tráfico:

```text
id
name
type
surface
lanes
width
oneway
points[]
```

El parser ya lee `surface`, cantidad de carriles, ancho y si la vía es de un solo sentido. 

Eso es importante: **no tenemos que reconstruir desde cero la semántica de las calles para el tráfico**.

---

# 3. Generador urbano que hemos estado desarrollando

Partimos de una representación sencilla y la fuimos llevando hacia:

```text
OSM / datos procesados
        ↓
city.json
        ↓
LimaCityGenerator
        ↓
Roads
Blocks
Lots
Buildings
Parks
        ↓
Ciudad editable en Unreal
```

El generador utiliza los puntos de las carreteras y los convierte a coordenadas Unreal respetando `UnitsPerMeter`. 

También tenemos footprints reales cuando OSM dispone de edificios. Cada footprint se representa mediante un polígono de al menos tres puntos. 

## Edificios procedurales

Ya conseguimos una versión que:

* genera casas en lotes;
* produce edificios de distintas alturas;
* genera fachadas simples;
* tiene ventanas y puertas;
* tiene colores/materiales diferentes;
* adapta aproximadamente las casas al terreno;
* evita muchos casos de superposición;
* genera cientos de edificios;
* mantiene cada edificio como actor independiente.

Esta última decisión es especialmente importante porque tú pediste explícitamente que la generación procedural **no convierta la ciudad en algo intocable**.

La filosofía debe seguir siendo:

```text
GENERAR
   ↓
obtener una buena base
   ↓
seleccionar cualquier edificio
   ↓
moverlo
cambiarlo
eliminarlo
reemplazarlo
personalizarlo
   ↓
versión final artesanal
```

El procedural nos ahorra trabajo. **No reemplaza el trabajo manual.**

---

# 4. Problema pendiente más importante del generador

La integración:

```text
ROAD
 ↕
LANDSCAPE
```

todavía no está suficientemente resuelta.

Durante las iteraciones vimos:

```text
carreteras enterradas
carreteras flotantes
pendientes exageradas
splines deformados
saltos verticales
intersecciones defectuosas
casas invadiendo carreteras
diferencias entre editor y Play
```

La versión básica del código simplemente levantaba la calle mediante `RoadZOffset` para evitar Z-fighting. 

Posteriormente intentamos hacer adaptación al terreno y fue precisamente ahí donde aparecieron varios de los problemas.

Por eso tomaste una decisión razonable para el MVP:

> **seguir con las mecánicas y posteriormente terminar/ajustar manualmente el entorno.**

Eso significa que **no debemos seguir bloqueando el desarrollo del juego porque SJL todavía no sea perfecto**.

---

# 5. El sistema vial que queremos finalmente

Aquí hay que separar dos cosas que anteriormente estábamos mezclando.

```text
           ROAD SYSTEM
               │
       ┌───────┴────────┐
       ↓                ↓
VISUAL ROAD        LOGICAL ROAD
       │                │
asfalto             carriles
vereda              nodos
líneas              direcciones
huecos              conexiones
materiales          velocidad
desgaste            intersecciones
etc.                tráfico
```

Esto es fundamental.

Una carretera visual puede ser modificada manualmente sin destruir el sistema de tráfico.

---

# 6. Road Network lógico

Queremos convertir las calles en una verdadera red navegable:

```text
Road
 ├── RoadSpline
 ├── Lane 0
 ├── Lane 1
 ├── ...
 ├── Direction
 ├── SpeedLimit
 ├── Surface
 ├── Width
 └── Connections
```

Las intersecciones serán nodos:

```text
          Lane
           ↓
───────────●──────────
           │
           │
           │
           ↓
```

Y un vehículo podrá consultar:

```text
estoy en Road 43
        ↓
Lane 1
        ↓
se aproxima Intersection 18
        ↓
opciones:
    recto
    izquierda
    derecha
```

Esto será la base tanto de NPC como del GPS y posiblemente de policía/persecuciones.

---

# 7. Tráfico peruano

No queremos una IA donde absolutamente todos conduzcan perfectamente.

Queremos perfiles.

```text
DriverProfile

NORMAL
CAUTELOSO
AGRESIVO
IMPRUDENTE
MOTOTAXISTA
MOTOCICLISTA
COMBI
TAXISTA
POLICÍA
```

Un conductor normal:

```text
respeta carril
respeta dirección
reduce en intersecciones
respeta semáforo
evita vehículos
```

Uno agresivo podría:

```text
cambiar de carril constantemente
exceder velocidad
adelantar
tocar bocina
forzar incorporaciones
```

Y determinados NPC de moto/mototaxi:

```text
filtrar entre vehículos
invadir parcialmente carril
adelantar
usar espacios pequeños
hacer maniobras arriesgadas
```

No queremos programar una IA distinta para cada vehículo. Queremos parámetros:

```cpp
Aggressiveness
SpeedCompliance
LaneDiscipline
RiskTolerance
ReactionTime
OvertakeProbability
HornProbability
```

y de ahí producir comportamientos distintos.

---

# 8. Carreteras visuales configurables

También definimos que una misma carretera no necesariamente tiene un solo aspecto.

Necesitamos segmentos:

```text
Road
│
├── Segment 0 → asfalto nuevo
├── Segment 1 → asfalto gastado
├── Segment 2 → concreto
├── Segment 3 → asfalto parchado
└── Segment 4 → tierra
```

Y por segmento:

```text
material
líneas
estado
huecos
vereda
berma
vegetación
props
```

Por ejemplo:

```text
Av. principal

0m ───────── 100m
asfalto
línea central
líneas laterales

100m ─────── 180m
asfalto gastado
líneas casi borradas

180m ─────── 240m
asfalto parchado
sin líneas
```

---

# 9. Huecos y deterioro

Pediste poder colocar algo equivalente a las zonas de edificios:

```text
LimaPotholeZone
```

o seleccionar un punto/tramo.

Después:

```text
Generate Potholes
```

con parámetros como:

```text
Density
MinRadius
MaxRadius
MinDepth
MaxDepth
Seed
Material
```

Los huecos no deberían ser solamente decorativos.

Más adelante podrán afectar:

```text
suspensión
sonido
cámara
agarre
velocidad
daño
IA
```

---

# 10. Veredas y configuración urbana

También queremos soportar diferentes perfiles de calle reales de Lima.

Ejemplo:

```text
CASA
│
├── vereda
├── jardín/retiro
├── estacionamiento
└── pista
```

o:

```text
CASA
│
├── vereda
└── pista
```

o incluso:

```text
CASA
│
└── pista/tierra
```

Las esquinas deben permitir radios distintos, porque no todas tienen una curva perfecta o del mismo tamaño.

---

# 11. Postes, cables e iluminación

También quedó planteado un generador basado en spline:

```text
UtilitySpline
       ↓
poste
       ↓ 50 m
poste
       ↓ 50 m
poste
```

Configurable:

```text
Spacing
PoleType
LightType
CableCount
CableSag
RandomVariation
```

y posteriormente:

```text
postes eléctricos
postes de alumbrado
cables
transformadores
cajas
semáforos
señales
```

La distancia no debe estar codificada como "cada 50 m"; debe ser parámetro editable.

---

# 12. Mercados

También identificamos que OSM no te va a construir automáticamente algo como un mercado de SJL con el nivel que necesitas.

La propuesta es:

```text
LimaMarketZone
```

Tú dibujas:

```text
┌──────────────────────────┐
│                          │
│       MERCADO            │
│                          │
└──────────────────────────┘
```

y configuras:

```text
pasillos
ancho pasillos
puestos
tamaño puestos
entradas
zonas libres
```

Generando:

```text
┌─┬─┬─┬─┐
│P│P│P│P│
├─┴─┴─┴─┤
│ PASAJE│
├─┬─┬─┬─┤
│P│P│P│P│
└─┴─┴─┴─┘
```

Cada puesto deberá seguir siendo editable individualmente.

Más adelante podrá convertirse además en un espacio jugable para NPC, comerciantes, robos, policía, etc.

---

# 13. Terreno

Los cerros son importantes porque SJL no puede parecer una ciudad construida sobre un plano.

Pero tampoco debemos deformar el Landscape entero para acomodarlo artificialmente a cada spline.

A largo plazo:

```text
Landscape = geografía
Road      = infraestructura sobre geografía
```

y solamente cuando corresponda:

```text
corte
relleno
muro
puente
escalera
talud
```

Eso nos permite conservar los cerros y pendientes característicos.

---

# 14. Punto actual del gameplay

Ahora estamos entrando en una etapa nueva.

Hasta ahora gran parte del esfuerzo fue:

> **¿Cómo construimos Lima?**

Ahora empezamos:

> **¿Qué puede hacer el jugador dentro de Lima?**

Tu proyecto ya contiene contenido de Manny/Quinn y animaciones de caminar/correr, entre otros assets. 

Y acabamos de plantear empezar por:

# Chaos Vehicles

La primera vertical jugable debe ser:

```text
PERSONAJE
    ↓
camina
    ↓
encuentra auto
    ↓
E
    ↓
entra
    ↓
conduce
    ↓
F
    ↓
sale
    ↓
continúa caminando
```

---

# 15. Sistema de vehículos que estamos empezando

La arquitectura que propusimos es:

```text
Vehicles/
├── KalexVehicleBase
├── KalexVehicleWheel
├── VehicleFuelComponent
├── VehicleDamageComponent
├── VehicleInteractionComponent
├── VehicleLightComponent
└── VehicleAudioComponent
```

y utilizar:

**Chaos Vehicles** para la física.

Primero vehículo de prueba. Después metemos tu **Chevrolet Captiva 2023**.

No debemos intentar configurar la Captiva, tráfico, gasolina, daños y entrada al mismo tiempo.

---

# 16. Interacción general

También empezamos a plantear algo que va a resultar extremadamente importante:

```text
KalexInteractable
```

No quiero que `E` signifique específicamente "entrar al carro".

`E` debe significar:

```text
INTERACT
```

y el objeto decide qué ocurre.

Por tanto:

```text
E
│
├── Auto       → Entrar
├── Puerta     → Abrir
├── NPC        → Hablar
├── Arma       → Recoger
├── Gasolinera → Usar
├── Tienda     → Comprar
├── Botín      → Robar
└── Objeto     → Examinar
```

Esto nos evita crear veinte sistemas de interacción diferentes.

---

# 17. Vehículos: objetivo del MVP

El vehículo finalmente debería soportar:

```text
conducción
aceleración
frenado
dirección
reversa
freno de mano

entrar
salir
cambiar asiento

encender/apagar
combustible
velocidad
RPM

luces
direccionales
bocina
freno
reversa

puertas
maletera
capó

daño
choques

cámara 3ª persona
cámara interior

conductor visible
pasajeros
```

No todo tiene que ser hiperrealista en el MVP, pero el diseño debe permitir agregarlo.

---

# 18. Gasolina

Queremos un componente independiente:

```text
VehicleFuelComponent

FuelCapacity
CurrentFuel
ConsumptionRate
EngineRunning
```

El consumo depende después de:

```text
RPM
aceleración
velocidad
motor
```

Flujo:

```text
conducir
   ↓
consume combustible
   ↓
reserva
   ↓
0
   ↓
motor se apaga
```

Y una gasolinera:

```text
estacionar
    ↓
salir
    ↓
interactuar surtidor
    ↓
elegir monto/litros
    ↓
pagar
    ↓
cargar
```

Esto conecta posteriormente con la economía.

---

# 19. Choques y daños

Necesitamos distinguir:

```text
Vehicle Physics
```

de:

```text
Vehicle Damage
```

Chaos resuelve el choque físico.

Nuestro sistema calcula las consecuencias:

```text
ImpactVelocity
ImpactDirection
ImpactLocation
        ↓
Damage
```

Posteriormente:

```text
parachoques
capó
puertas
vidrios
luces
motor
ruedas
```

Para MVP basta inicialmente:

```text
Health = 100

choque pequeño → -5
choque fuerte  → -30
choque brutal  → vehículo inutilizable
```

---

# 20. NPC

Necesitamos una arquitectura común:

```text
NPC
├── Civilian
├── Merchant
├── Driver
├── Criminal
├── Police
└── Special
```

Cada uno con:

```text
AIController
Behavior
Perception
State
Schedule
Faction
Reactions
```

Estados básicos:

```text
Idle
Walking
Talking
Working
Fleeing
Fighting
Driving
Dead
```

---

# 21. Peatones

Las veredas tendrán también su propia red lógica.

```text
PedestrianNetwork
```

No queremos simplemente:

```text
Random Point → Random Point
```

Queremos:

```text
vereda
 ↓
esquina
 ↓
cruce
 ↓
vereda
 ↓
tienda
```

Entonces los NPC realmente parecerán habitantes de la ciudad.

---

# 22. NPC + vehículos

Un NPC conductor podrá:

```text
Spawn
 ↓
buscar vehículo
 ↓
entrar
 ↓
seleccionar destino
 ↓
consultar RoadNetwork
 ↓
conducir
 ↓
estacionar
 ↓
salir
```

Inicialmente podemos simplificarlo:

```text
vehículo ya ocupado
        ↓
seguir LaneSpline
```

y agregar comportamiento humano después.

---

# 23. Armas

El sistema debería ser modular:

```text
WeaponBase
├── Firearm
├── Melee
└── Throwable
```

Y componentes:

```text
WeaponComponent
InventoryComponent
AmmoComponent
```

Primera versión:

```text
recoger pistola
equipar
apuntar
disparar
recargar
munición
impacto
daño
```

Después:

```text
retroceso
precisión
animaciones
sonidos
muzzle flash
casquillos
headshots
diferentes armas
```

---

# 24. Inventario

Esto debería aparecer relativamente pronto porque muchos sistemas lo necesitarán.

```text
Inventory

dinero
armas
munición
comida
objetos
llaves
misión/items
```

Pero no necesitamos hacer todavía un RPG gigantesco.

---

# 25. Crimen

Un crimen produce un evento:

```text
CrimeEvent
```

Ejemplos:

```text
robo
asalto
disparo
agresión
robo de vehículo
atropello
homicidio
daño a propiedad
```

Pero cometer un delito **no implica automáticamente que la policía mágicamente sepa que ocurrió**.

Queremos:

```text
CRIMEN
  ↓
¿testigos?
  ↓
¿policía cercana?
  ↓
¿cámara?
  ↓
¿denuncia?
  ↓
CrimeReported
```

Esto hará el sistema bastante más interesante.

---

# 26. Robos

Primera versión:

```text
acercarse NPC
 ↓
amenazar
 ↓
NPC reacciona
 ↓
entrega dinero / huye / pelea
 ↓
testigos
 ↓
posible denuncia
```

Después:

```text
tiendas
bodegas
mercados
vehículos
casas
```

Y eventualmente atracos más complejos.

---

# 27. Policía

Necesitamos algo similar al wanted system, pero separado en componentes.

```text
CrimeSystem
      ↓
WantedSystem
      ↓
PoliceDispatch
      ↓
PoliceAI
```

Por ejemplo:

```text
Nivel 0
sin búsqueda

Nivel 1
policía cercana investiga

Nivel 2
patrulleros

Nivel 3
persecución fuerte

Nivel 4+
respuesta especial
```

El número exacto de niveles lo definiremos después.

---

# 28. Persecuciones

La policía podrá usar el mismo RoadNetwork.

```text
Jugador huye
       ↓
PoliceController
       ↓
RoadNetwork
       ↓
interceptar
```

No solamente:

```text
Follow Player Location
```

Eso permitirá posteriormente:

```text
atajos
bloqueos
intercepciones
varios patrulleros
```

---

# 29. Economía

Necesitaremos un sistema pequeño:

```text
MoneyComponent
```

para:

```text
gasolina
armas
comida
tiendas
reparaciones
multas
ropa
vehículos
```

y obtener dinero mediante:

```text
misiones
trabajos
ventas
robos
```

---

# 30. Sistema de mundo

Después:

```text
DayNightSystem
WeatherSystem
WorldTimeSystem
```

El tiempo permitirá cosas interesantes:

```text
07:00 mercado abre
08:00 tráfico aumenta
12:00 más peatones
18:00 tráfico fuerte
23:00 comercios cierran
02:00 pocas personas
```

No es prioritario todavía, pero debemos evitar arquitecturas que lo impidan.

---

# 31. Audio de Lima

Para que el MVP realmente empiece a sentirse como Lima necesitaremos posteriormente:

```text
bocinas
motores
mototaxis
combis
vendedores
perros
música de locales
tráfico
policía
sirenas
ambiente de mercado
```

Esto puede producir muchísimo realismo con relativamente poco coste comparado con modelar miles de assets únicos.

---

# 32. Arquitectura global que propongo mantener

No convertir `LimaCityGenerator` en un dios de 20,000 líneas.

Debe quedar aproximadamente:

```text
KALEX URBAN
│
├── World/
│   ├── City/
│   ├── Roads/
│   ├── Zones/
│   ├── Markets/
│   ├── Utilities/
│   └── Environment/
│
├── Character/
│   ├── Player/
│   ├── Interaction/
│   ├── Inventory/
│   └── Health/
│
├── Vehicles/
│   ├── Physics/
│   ├── Interaction/
│   ├── Fuel/
│   ├── Damage/
│   ├── Lights/
│   └── Audio/
│
├── AI/
│   ├── Pedestrians/
│   ├── Drivers/
│   ├── Traffic/
│   └── Police/
│
├── Combat/
│   ├── Weapons/
│   ├── Damage/
│   └── Ammo/
│
├── Crime/
│   ├── CrimeSystem/
│   ├── Witnesses/
│   ├── Wanted/
│   └── PoliceDispatch/
│
├── Economy/
│   ├── Money/
│   ├── Shops/
│   └── FuelStations/
│
├── Missions/
│
├── UI/
│
├── Audio/
│
└── Save/
```

---

# 33. Roadmap que usaría para llegar al MVP

No avanzaría por "módulos terminados". Haría **vertical slices**. Es decir: cada fase debe dejar algo nuevo que puedas jugar.

| Fase                         | Resultado jugable                                                                 |
| ---------------------------- | --------------------------------------------------------------------------------- |
| **0 — Base estable**         | Personaje aparece siempre correctamente, ciudad no cambia al Play, mapa guardable |
| **1 — Vehicle Core**         | Puedes manejar un Chaos Vehicle                                                   |
| **2 — Vehicle Interaction**  | Caminas → E → entras → conduces → F → sales                                       |
| **3 — Vehicle Complete MVP** | Motor, gasolina, luces, bocina, HUD, daño básico                                  |
| **4 — Road Network**         | Calles tienen carriles, sentidos, nodos e intersecciones                          |
| **5 — Traffic MVP**          | 10–30 autos conducen solos correctamente                                          |
| **6 — Pedestrians MVP**      | NPC caminan por veredas y cruzan calles                                           |
| **7 — World Interaction**    | Puertas, objetos, tiendas, dinero e inventario                                    |
| **8 — Weapons MVP**          | Recoger → equipar → apuntar → disparar → recargar                                 |
| **9 — NPC Reactions**        | NPC reaccionan a disparos, choques y amenazas                                     |
| **10 — Crime MVP**           | Robar/atacar/disparar produce CrimeEvents y testigos                              |
| **11 — Police MVP**          | Policía recibe delito, llega y persigue al jugador                                |
| **12 — Vehicle Chase**       | Policía y jugador pueden perseguirse en vehículos                                 |
| **13 — Lima Traffic**        | Motos, mototaxis, combis y perfiles de conducción                                 |
| **14 — Urban Detail**        | Huecos, líneas, postes, cables, mercados, props                                   |
| **15 — Presentation Pass**   | SJL manualmente pulido, iluminación, audio, VFX, UI                               |
| **16 — MVP Investor Build**  | Ejecutable estable con gameplay completo de demostración                          |

---

# 34. Qué debería mostrar el MVP a un inversionista

No necesitas 20 km².

Prefiero que tengamos **un sector pequeño de SJL extremadamente vivo**.

La demo ideal podría durar unos 10 minutos:

```text
SPAWN EN SJL
      ↓
caminar por la calle
      ↓
NPC + vendedores + tráfico
      ↓
acercarse a Captiva
      ↓
abrir puerta / entrar
      ↓
encender motor
      ↓
conducir por Canto Grande
      ↓
mototaxis + combis + tráfico
      ↓
choque pequeño
      ↓
seguir conduciendo
      ↓
combustible bajo
      ↓
gasolinera
      ↓
pagar / repostar
      ↓
estacionar
      ↓
salir
      ↓
entrar a mercado
      ↓
interacción / robo
      ↓
NPC gritan / huyen
      ↓
policía recibe alerta
      ↓
huir
      ↓
robar otro vehículo
      ↓
persecución vehicular
      ↓
escapar o ser detenido
```

Eso ya comunica muchísimo más que enseñar una Lima gigantesca pero sin gameplay.

---

## Estado real ahora

Yo pondría el proyecto **aquí**:

```text
             KALEX URBAN MVP

CITY PIPELINE       ███████░░░
BUILDINGS           ███████░░░
ROADS VISUAL        ████░░░░░░
TERRAIN/ROADS       ███░░░░░░░
MANUAL EDITING      █████░░░░░

PLAYER              ██████░░░░
INTERACTION CORE    ██░░░░░░░░
VEHICLES            █░░░░░░░░░   ← ESTAMOS AQUÍ
ROAD NETWORK        ░░░░░░░░░░
TRAFFIC AI          ░░░░░░░░░░
PEDESTRIANS         ░░░░░░░░░░
INVENTORY           ░░░░░░░░░░
WEAPONS             ░░░░░░░░░░
CRIME               ░░░░░░░░░░
POLICE              ░░░░░░░░░░
ECONOMY             ░░░░░░░░░░
MISSIONS            ░░░░░░░░░░
SAVE GAME           ░░░░░░░░░░
```

Esos porcentajes son únicamente una representación conceptual de avance, no una medición del código.

Y hay una decisión que mantendría desde este momento: **congelar temporalmente el generador urbano que ya funciona suficientemente bien para probar gameplay**. El `city.json` ya nos proporciona calles con tipo, superficie, carriles, ancho y sentido, que es justamente una base aprovechable cuando lleguemos al Road Network. 

Por tanto, el siguiente objetivo concreto no debería ser otra V5.x del generador. Debería ser una **V6.0 Gameplay Foundation**, cuyo primer hito sea: **personaje → entrar a un Chaos Vehicle → conducir físicamente por SJL → chocar → salir del vehículo**, sin que `GenerateCity` intervenga para nada durante el Play. Después de conseguir ese vertical slice, empezamos gasolina/daño y posteriormente Road Network + tráfico.


