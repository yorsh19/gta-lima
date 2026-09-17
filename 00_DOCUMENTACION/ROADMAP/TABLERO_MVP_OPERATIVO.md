# Tablero MVP Operativo - Kalex Urban

## Convenciones

- Estado: `todo` | `in_progress` | `blocked` | `review` | `done`
- Prioridad: `P0` (critico) | `P1` (alto) | `P2` (medio)
- Sprint: `S1` (Sem 1-4) | `S2` (Sem 5-8) | `S3` (Sem 9-12)

## Board

| ID | Disciplina | Tarea | Owner | Sprint | Estado | Prioridad | Dependencia | Criterio de aceptacion |
|---|---|---|---|---|---|---|---|---|
| PROD-01 | Produccion | Definir calendario 12 semanas y cadencia de dailies/review | Producer | S1 | todo | P0 | - | Calendario aprobado y compartido al equipo |
| PROD-02 | Produccion | Definir DoD por sprint y politica de freeze | Producer | S1 | todo | P0 | PROD-01 | Documento DoD visible y aplicado en reviews |
| GD-01 | Game Design | Documentar loop jugable de 15 min (core loop + emotional loop) | Lead Designer | S1 | todo | P0 | - | Loop validado en documento y usado por programacion |
| GD-02 | Game Design | Diseñar Mision 1 (flujo, objetivos, recompensas, fail states) | Lead Designer | S1 | todo | P0 | GD-01 | Mision implementable sin ambiguedades |
| GD-03 | Game Design | Diseñar economia base v1 (ingresos/gastos/urgencia) | Lead Designer | S1 | todo | P1 | GD-01 | Tabla de economia con valores iniciales y tuning notes |
| GD-04 | Game Design | Diseñar Mision 2 con decision moral gris | Lead Designer | S2 | todo | P0 | GD-02 | Documento de mision completo y testeable |
| GD-05 | Game Design | Diseñar 3 actividades sistemicas (mercado/mototaxi/batida) | Lead Designer | S2 | todo | P1 | GD-01 | Las 3 actividades tienen reglas y recompensas definidas |
| GD-06 | Game Design | Tuning final de ritmo y momentos clipables | Lead Designer | S3 | todo | P1 | GD-04, GD-05 | Sesion 10-20 min con 2+ momentos compartibles |
| BP-01 | Blueprint/C++ | Implementar control jugador + camara third person estable | Lead Gameplay Programmer | S1 | todo | P0 | - | Navegacion y camara estables sin bugs criticos |
| BP-02 | Blueprint/C++ | Sistema de mision v1 (start/update/complete/fail) | Lead Gameplay Programmer | S1 | todo | P0 | GD-02, BP-01 | Mision 1 completa de punta a punta |
| BP-03 | Blueprint/C++ | Abordaje combi v1 (subir/bajar/pagar/ruta corta) | Lead Gameplay Programmer | S1 | todo | P0 | BP-01 | Jugador usa combi funcionalmente sin bloqueo |
| BP-04 | Blueprint/C++ | Trafico base v1 (densidad + semaforos simples) | Lead Gameplay Programmer | S2 | todo | P0 | BP-01 | Flujo de trafico percibido como congestion real |
| BP-05 | Blueprint/C++ | Eventos de congestion/microchoques probabilisticos | Lead Gameplay Programmer | S2 | todo | P1 | BP-04 | Ocurren eventos emergentes sin romper FPS objetivo |
| BP-06 | Blueprint/C++ | IA NPC por roles (ambulante/pasajero/cobrador/policia) | Lead Gameplay Programmer | S2 | todo | P0 | BP-01 | 4 roles activos con comportamiento basico consistente |
| BP-07 | Blueprint/C++ | Mototaxi funcional con trayecto y evento emergente | Lead Gameplay Programmer | S2 | todo | P1 | BP-01, GD-05 | Trayecto completo y evento dispara correctamente |
| BP-08 | Blueprint/C++ | Guardado minimo de progreso de sesion | Lead Gameplay Programmer | S2 | todo | P1 | BP-02 | Sesion guarda y restaura estado base |
| BP-09 | Blueprint/C++ | Optimizacion: spawn/despawn y limpieza de cuellos de botella | Lead Gameplay Programmer | S3 | todo | P0 | BP-04, BP-06 | Build RC estable en 1080p objetivo |
| ART-01 | Arte | Graybox avenida + mercado + paradero | Lead Environment Artist | S1 | todo | P0 | - | Zona navegable completa para first playable |
| ART-02 | Arte | Set inicial de props urbanos (puestos/carteles/cables/basura) | Lead Environment Artist | S1 | todo | P1 | ART-01 | Biblioteca minima integrada en mapa |
| ART-03 | Arte | Art pass principal del slice | Lead Environment Artist | S2 | todo | P0 | ART-01 | Escenario pasa de graybox a lectura urbana clara |
| ART-04 | Arte | Variantes visuales combi/mototaxi/colectivo | Lead Environment Artist | S2 | todo | P1 | ART-03 | Vehiculos legibles y diferenciables en gameplay |
| ART-05 | Arte | Iluminacion v1 tarde-noche optimizada | Lead Environment Artist | S2 | todo | P0 | ART-03 | Atmsofera definida con rendimiento aceptable |
| ART-06 | Arte | Polish hotspots para capturas virales | Lead Environment Artist | S3 | todo | P1 | ART-05 | 3 hotspots listos para captura promocional |
| AUD-01 | Audio | Paisaje sonoro placeholder por zonas | Lead Sound Designer | S1 | todo | P0 | ART-01 | Cada zona tiene identidad sonora basica |
| AUD-02 | Audio | SFX base de trafico y multitud | Lead Sound Designer | S1 | todo | P1 | AUD-01 | Sonidos base disparan correctamente en contexto |
| AUD-03 | Audio | Voces ambientales (rutas/cobradores/ambulantes) | Lead Sound Designer | S2 | todo | P0 | AUD-01 | Se oyen lineas contextuales en puntos clave |
| AUD-04 | Audio | Mezcla dinamica por congestion/proximidad/evento policial | Lead Sound Designer | S2 | todo | P1 | AUD-03, BP-04 | Mix responde al estado del mundo en tiempo real |
| AUD-05 | Audio | Mezcla final y variaciones para reducir repeticion | Lead Sound Designer | S3 | todo | P1 | AUD-04 | Repeticion percibida reducida en playtest |
| NAR-01 | Narrative | Intro jugable de Nero (contexto + urgencia) | Narrative Designer | S1 | todo | P0 | GD-01 | Intro implementada y comprensible en <3 min |
| NAR-02 | Narrative | Dialogos completos de Mision 1 | Narrative Designer | S1 | todo | P1 | GD-02 | Dialogos integrados y consistentes con tono |
| NAR-03 | Narrative | Dialogos completos de Mision 2 | Narrative Designer | S2 | todo | P0 | GD-04 | Mision 2 ejecutable con narrativa integrada |
| NAR-04 | Narrative | Cierre breve con cliffhanger del conflicto mayor | Narrative Designer | S3 | todo | P1 | NAR-03 | Final del slice deja gancho claro |
| QA-01 | QA | Checklist smoke test por build | QA Lead | S1 | todo | P0 | BP-01, BP-02 | Checklist usado en todas las builds internas |
| QA-02 | QA | Pruebas diarias P0/P1 + repro steps | QA Lead | S1 | todo | P0 | QA-01 | Todos los bugs criticos tienen repro claro |
| QA-03 | QA | Regresion de flujo completo 15 min | QA Lead | S2 | todo | P0 | BP-08, GD-04 | Flujo completo sin blockers en build CC |
| QA-04 | QA | Playtest cerrado (10-20 usuarios) + encuesta KPI | QA Lead | S3 | todo | P0 | QA-03 | Reporte KPI final entregado con hallazgos priorizados |
| MKT-01 | Marketing | Definir pipeline de captura vertical y calendario de clips | Producer | S2 | todo | P2 | ART-06 | Calendario de 2-3 clips/semana listo |
| MKT-02 | Marketing | Paquete final de clips para lanzamiento del MVP | Producer | S3 | todo | P1 | QA-04, ART-06 | 5-10 clips listos para publicacion |

## Hitos

- H1 (fin S1): `First Playable` interno de 8-10 min.
- H2 (fin S2): `Content Complete` del vertical slice.
- H3 (fin S3): `Release Candidate` + paquete de clips.

## KPI de salida del MVP

- Autenticidad percibida: >70% testers reportan "se siente Lima".
- Sesion media: >12 minutos.
- Clipabilidad: >=2 momentos compartibles por sesion.
- Estabilidad: 0 bloqueos criticos en ruta principal de misiones.
