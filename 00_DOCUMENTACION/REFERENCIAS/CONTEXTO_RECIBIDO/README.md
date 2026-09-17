# Contexto recibido — mapa y decisiones abiertas

Esta carpeta conserva íntegramente los tres textos entregados el 17 de septiembre de 2026. Su finalidad es evitar que se pierdan detalles narrativos, críticas editoriales o decisiones técnicas mientras los documentos canónicos evolucionan.

## Documentos fuente

| Documento | Qué contiene | Cómo usarlo |
| --- | --- | --- |
| [`01_HISTORIA_BASE_CONSOLIDADA.md`](01_HISTORIA_BASE_CONSOLIDADA.md) | Historia expandida en 32 secciones: origen de Nero, funeral, ascenso criminal y político, crisis, secuestro, muerte y epílogo. | Fuente narrativa extensa para desarrollar actos, personajes y misiones. |
| [`02_ANALISIS_CRITICO_HISTORIA.md`](02_ANALISIS_CRITICO_HISTORIA.md) | Evaluación de fortalezas, debilidades, riesgos culturales y recomendaciones de guion. | Lista de control editorial antes de cerrar el argumento. |
| [`03_CONTEXTO_TECNICO_Y_ROADMAP_MVP.md`](03_CONTEXTO_TECNICO_Y_ROADMAP_MVP.md) | Estado del generador urbano, arquitectura de sistemas, gameplay, vehículos, IA, crimen y roadmap de 16 fases. | Fuente técnica y de planificación; validar siempre contra el repositorio actual. |

## Visión consolidada

Kalex Urban es un sandbox criminal de mundo abierto ambientado en una Lima semi-realista, inicialmente concentrado en San Juan de Lurigancho y Canto Grande. La ciudad es tanto escenario como personaje: transporte informal, tráfico, mercados, cerros, comercio, desigualdad, humor cotidiano, corrupción e informalidad forman parte del gameplay y de la narrativa.

Gabriel “Nero”, de 22 años, inicia como un joven pobre y desempleado que vive con su madre y su abuela. La muerte de la abuela y la necesidad de pagar el funeral precipitan su primer delito. Su arco escala desde trabajos precarios y crimen barrial hasta negocios, influencia social y poder político; termina destruido por las deudas, favores y redes que él mismo creó. El epílogo devuelve la mirada a Lima, que continúa indiferente a su caída.

El tono buscado combina sátira social, humor negro, absurdo reconocible y tragedia humana. La sátira debe apuntar hacia el poder y los sistemas, no convertir la pobreza ni a las víctimas en caricatura.

## Fortalezas narrativas que deben preservarse

- La geografía expresa el ascenso: Huáscar/SJL → Rímac → Centro → poder nacional.
- La abuela funciona como ancla emocional y el funeral como detonante comprensible.
- La ambigüedad moral evita un sistema simplista de “bueno contra malo”.
- El pasado criminal que ayuda a sostener el poder de Nero también causa su caída.
- La muerte devuelve simbólicamente a “Nero” a Gabriel.
- El epílogo “Nero murió; Lima siguió” cierra la tesis sin sobreexplicarla.

## Decisiones narrativas aún abiertas

1. **Nombre definitivo:** la fuente expandida deja el apellido de Gabriel abierto, mientras `HISTORIA_PRINCIPAL.md` usa “Ramírez”. Debe confirmarse cuál es canónico.
2. **Segundo acto:** faltan catalizadores concretos que conecten líder barrial, empresario, figura social, político y presidente.
3. **Valeria:** necesita objetivos, conflicto y acciones propias que alteren la trama; no debe limitarse a ser conciencia moral o interés romántico.
4. **Antagonista personal:** el sistema es el antagonista temático, pero falta decidir qué personaje o alianza será su rostro jugable.
5. **Organización insurgente:** debe definirse si se tratará con seriedad, si se alejará por completo de referentes históricos peruanos o si tendrá sátira explícita dirigida al poder. No debe quedar como decorado ambiguo.
6. **Tono y humor:** faltan escenas concretas que prueben la combinación de drama, absurdo y humor negro peruano.
7. **“Por qué ahora”:** falta formular qué rasgo del Perú contemporáneo hace urgente esta historia.
8. **Ramificaciones:** deben desarrollarse decisiones, consecuencias y misiones sin romper la columna vertebral del arco trágico.
9. **Reparto y cronología:** faltan nombres y desarrollo definitivo de la madre, El Gallo, la banda, aliados, traidores, partido político y organización antagonista.

## Decisiones técnicas y de producción que orientan el MVP

- Separar el generador de ciudad del juego que corre sobre ella.
- Evitar que `LimaCityGenerator` concentre carreteras, tráfico, peatones, vehículos, policía y economía.
- Congelar temporalmente la expansión del generador cuando sea suficientemente estable para validar gameplay.
- Mantener un `RoadNetwork` lógico independiente de la representación visual de las vías.
- Implementar interacción genérica (`Interact`) en vez de una tecla acoplada a “entrar al vehículo”.
- Construir vehículos, combustible, daño, tráfico, peatones, crimen, testigos, búsqueda y despacho policial como sistemas modulares.
- Trabajar por vertical slices que siempre produzcan un resultado jugable.
- Priorizar densidad, autenticidad, audio y comportamiento sistémico sobre extensión geográfica.

## Contradicciones o datos que deben verificarse

- **Versión de Unreal Engine:** el GDD ejecutivo indica “UE 5.5+”; el contexto técnico declara que el proyecto está actualmente en “UE 5.6”. El repositorio/proyecto debe ser la fuente definitiva.
- **Estado de avance:** las barras incluidas en el contexto técnico son conceptuales, no métricas verificadas.
- **Roadmap general:** el archivo `ROADMAP_GENERAL.md` es una síntesis de tres fases, mientras la fuente técnica propone 16 fases jugables. El backlog y el tablero operativo deben decidir el orden ejecutable.
- **Alcance narrativo del MVP:** la historia completa llega al poder nacional, pero el vertical slice se limita al comienzo del Acto I y a la primera fractura moral.

## Próximas definiciones recomendadas

1. Confirmar versión de Unreal y estado real de cada sistema desde el repositorio.
2. Convertir el roadmap de 16 fases en hitos verificables del tablero, sin duplicar tareas existentes.
3. Escribir el primer acto como guion jugable: introducción → robo de la casa → muerte/funeral → primer trabajo con El Gallo.
4. Prototipar una escena de humor negro para validar el tono antes de expandir el guion.
5. Resolver el tratamiento del arco insurgente antes de diseñar sus misiones.
