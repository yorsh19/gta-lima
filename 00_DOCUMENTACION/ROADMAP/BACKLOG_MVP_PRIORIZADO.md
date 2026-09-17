# Backlog MVP Priorizado - Kalex Urban

## Criterios

- Must: indispensable para que el MVP cumpla la promesa "Lima se siente viva".
- Should: mejora fuerte de calidad e inmersion; se incluye si no compromete fecha.
- Could: valor adicional para viralidad/pulido; entra solo con holgura.
- Won't (MVP): queda fuera del vertical slice actual.

## Owners por disciplina

- Game Design: Lead Designer
- Blueprint/C++: Lead Gameplay Programmer
- Arte: Lead Environment Artist
- Audio: Lead Sound Designer
- Narrative: Narrative Designer
- QA: QA Lead
- Produccion/Tracking: Producer

## Estimacion macro (12 semanas)

- Preproduccion (Semanas 1-2): 2 semanas
- Produccion Core (Semanas 3-8): 6 semanas
- Pulido y Vertical Slice (Semanas 9-11): 3 semanas
- Buffer + Release Candidate (Semana 12): 1 semana

## Estimacion por disciplina (esfuerzo total)

- Game Design: 6-8 semanas (distribuidas durante todo el ciclo)
- Blueprint/C++: 9-11 semanas
- Arte: 8-10 semanas
- Audio: 6-8 semanas
- Narrative: 4-6 semanas
- QA: 8-12 semanas (continuo)
- Produccion: 12 semanas (continuo)

## Must (imprescindible)

### Game Design

- Definir loop jugable de 15 min con onboarding diegetico.
- Diseñar 2 misiones principales (supervivencia + decision gris).
- Diseñar 3 actividades sistemicas cortas (mercado, mototaxi, batida).
- Definir reglas base de economia (ingresos, gastos urgentes, riesgo).
- Definir variables minimas de reputacion (callejera/policial/barrial).

### Blueprint/C++

- Implementar movimiento del jugador y camara estable para third person.
- Implementar abordaje de combi (subir/bajar/pagar/trigger de ruta).
- Implementar mototaxi funcional en trayecto corto con evento emergente.
- Implementar trafico base con densidad controlada y semaforos simples.
- Implementar eventos de congestion y microchoques con probabilidad.
- Implementar IA NPC basica por roles (ambulante, pasajero, cobrador, policia).
- Implementar sistema de mision simple (objetivos, estados, recompensa).
- Implementar guardado minimo de progreso de sesion.

### Arte

- Construir graybox y luego art pass de 1 avenida principal + mercado + paradero.
- Crear set minimo de props urbanos: puestos, carteles, cables, vallas, basura.
- Crear variantes visuales de combi/mototaxi/colectivo para legibilidad local.
- Iluminacion tarde-noche optimizada para atmosfera y rendimiento.

### Audio

- Implementar paisaje sonoro por zonas (avenida, mercado, paradero).
- Implementar voces ambientales de rutas/cobradores/ambulantes.
- Implementar mezcla dinamica por congestion, proximidad y eventos policiales.
- Implementar set base de SFX (trafico, bocinas, frenadas, multitud).

### Narrative

- Escribir intro jugable breve de Nero (contexto familiar + urgencia).
- Escribir dialogos de Mision 1 y Mision 2 con tono autentico.
- Definir beats emocionales clave (presion, oportunidad, consecuencia).

### QA

- Crear checklist de smoke test por build (movimiento, misiones, transporte).
- Ejecutar pruebas de estabilidad de 15 min por sesion.
- Reportar bugs P0/P1 diarios con repro steps claros.

## Should (muy importante)

### Game Design

- Ajustar dificultad economica para evitar frustracion temprana.
- Diseñar eventos de calle con variaciones (protesta, cierre, operativo).

### Blueprint/C++

- Sistema de notoriedad policial basico por acciones ilegales.
- Reacciones de NPC por contexto (miedo, huida, alerta).
- Sistema de spawn/despawn inteligente para mantener FPS estable.

### Arte

- Segunda pasada de detalle ambiental (calcomanias, suciedad, desgaste).
- Variaciones climaticas ligeras (neblina/humedad) para atmosfera limena.

### Audio

- Integrar radios diegeticas y anuncios de transporte.
- Variantes de locuciones por hora/zona para evitar repeticion.

### Narrative

- Escena corta de cierre del slice con cliffhanger del conflicto mayor.

### QA

- Playtest cerrado con 10-20 usuarios objetivo y encuesta KPI.

## Could (si hay holgura)

### Game Design

- Minijuego de cobro en combi con bonus por eficiencia.
- Evento viral de persecucion en mototaxi con rutas alternas.

### Blueprint/C++

- Sistema simple de favores barriales (micro encargos dinamicos).
- Prototipo temprano de estres psicologico (feedback audiovisual).

### Arte

- Pulido cinematografico de puntos iconicos para capturas virales.

### Audio

- Capas de crowd avanzadas con variacion estocastica por densidad.

### Marketing/Comunidad

- Modo captura basico para clips verticales (UI reducida).

## Won't (fuera del MVP)

- Mapa gigante multi-distrito completo.
- Multiplayer masivo/online competitivo.
- Sistema politico de gran escala jugable completo.
- Arco narrativo de 5 actos completo.
- Sistemas criminales avanzados (armas/drogas/redes complejas).
- Pulido AAA de animaciones/cinematics de larga duracion.

## Entregables por disciplina (MVP)

- Game Design: documento de loop + fichas de misiones + tabla de economia.
- Blueprint/C++: build jugable con transporte, trafico, NPCs y 2 misiones.
- Arte: bloque urbano final del slice con lectura visual clara.
- Audio: mezcla funcional por zonas + eventos clave.
- Narrative: guion de slice implementado en juego.
- QA: matriz de bugs priorizada + reporte de KPI de playtest.

## Criterios de aceptacion (Definition of Done)

- El jugador puede iniciar, completar 2 misiones y cerrar sesion sin bloqueo critico.
- Se percibe densidad urbana creible en al menos 3 hotspots del mapa.
- Transporte publico es util y entendible sin tutorial invasivo.
- La demo sostiene 10-20 minutos con al menos 2 momentos clipables.
- KPIs minimos cumplidos: autenticidad percibida >70% y sesion media >12 min.

## Plan por sprints (1, 2, 3)

### Sprint 1 (Semanas 1-4) - Fundacion jugable

- Objetivo: lograr un "first playable" estable con loop base y zona graybox.
- Game Design (Owner: Lead Designer): loop 15 min, especificacion de Mision 1, economia base v1.
- Blueprint/C++ (Owner: Lead Gameplay Programmer): control jugador, sistema de mision v1, abordaje combi v1.
- Arte (Owner: Lead Environment Artist): graybox avenida + mercado + paradero, set inicial de props.
- Audio (Owner: Lead Sound Designer): paisaje sonoro placeholder por zonas + SFX base de trafico.
- Narrative (Owner: Narrative Designer): intro de Nero + dialogos preliminares de Mision 1.
- QA (Owner: QA Lead): smoke tests diarios y registro de bugs P0/P1.
- Entregable Sprint 1: build interna "First Playable" de 8-10 min.

### Sprint 2 (Semanas 5-8) - Core vertical slice

- Objetivo: completar sistemas core y contenido jugable principal del MVP.
- Game Design (Owner: Lead Designer): balance economia v2, Mision 2, actividades sistemicas completas.
- Blueprint/C++ (Owner: Lead Gameplay Programmer): trafico denso v1, IA NPC por roles, mototaxi funcional, guardado minimo.
- Arte (Owner: Lead Environment Artist): art pass principal, variantes visuales de transporte, iluminacion v1.
- Audio (Owner: Lead Sound Designer): voces ambientales de rutas/cobradores + mezcla dinamica v1.
- Narrative (Owner: Narrative Designer): guion implementado de Mision 1 y Mision 2.
- QA (Owner: QA Lead): pruebas de regresion por build + validacion de flujo completo 15 min.
- Entregable Sprint 2: build "Content Complete" del slice.

### Sprint 3 (Semanas 9-12) - Pulido, validacion y captura

- Objetivo: cerrar calidad, rendimiento y material de presentacion.
- Game Design (Owner: Lead Designer): tuning final de ritmo/recompensas y moments clipables.
- Blueprint/C++ (Owner: Lead Gameplay Programmer): optimizacion, fixes criticos, spawn/despawn estable.
- Arte (Owner: Lead Environment Artist): polish de hotspots y legibilidad urbana final.
- Audio (Owner: Lead Sound Designer): mezcla final, variaciones de locuciones, limpieza de repeticion.
- Narrative (Owner: Narrative Designer): cierre breve tipo cliffhanger para salida de demo.
- QA (Owner: QA Lead): playtest cerrado 10-20 usuarios + reporte KPI final.
- Produccion (Owner: Producer): freeze de alcance, checklist RC y empaquetado de build.
- Entregable Sprint 3: build "Release Candidate" + paquete de clips para marketing.
