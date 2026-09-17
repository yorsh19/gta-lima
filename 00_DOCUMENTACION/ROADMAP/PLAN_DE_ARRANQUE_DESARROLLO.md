# Plan de Arranque de Desarrollo (Semana 1)

## Objetivo de la semana

Tener un `First Playable` minimo: personaje jugable + mapa graybox simple + una mision corta de prueba.

## Dia 1 - Base tecnica

- Crear proyecto UE5.5 en `01_PROYECTO_UNREAL/` con plantilla Third Person.
- Activar plugins: Enhanced Input, Chaos Vehicles, PCG Framework, World Partition, Niagara, MetaSounds.
- Definir estructura de carpetas de contenido segun `04_GAMEPLAY/sistemas/ESTRUCTURA_CONTENIDO_UE.md`.

## Dia 2 - Control y camara

- Implementar input actions de movimiento/camara/sprint/interactuar.
- Crear `BP_KalexCharacter` y `BP_KalexPlayerController` base.
- Validar jugabilidad estable a 60 FPS en mapa vacio.

## Dia 3 - Graybox de avenida

- Montar nivel de prueba: avenida + paradero + mercado simple.
- Colocar colisiones, nav mesh y puntos de spawn.
- Crear `BP_InteractionPoint` para pruebas de interaccion.

## Dia 4 - Mision de prueba

- Implementar flujo simple de mision: iniciar -> objetivo -> completar.
- Mision vertical test: "habla con cobrador" + "sube a combi".
- Guardar estado minimo en memoria de sesion.

## Dia 5 - QA + cierre sprint

- Correr checklist de smoke tests en `06` escenarios basicos.
- Corregir bloqueos criticos (P0/P1).
- Capturar video interno de 60-90 segundos del estado actual.

## Definicion de listo (fin Semana 1)

- El jugador entra al nivel, se mueve sin errores criticos y completa mision de prueba.
- Existe una base reusable para Sprint 2.
