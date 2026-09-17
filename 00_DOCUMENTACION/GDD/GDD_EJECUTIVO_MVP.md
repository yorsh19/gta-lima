# GDD Ejecutivo - Kalex Urban (MVP Vertical Slice)

- Proyecto: Kalex Urban
- Estudio: Kalex Soluciones
- Objetivo del documento: Alinear vision creativa, alcance tecnico y estrategia de produccion del MVP
- Version: 0.1 (Ejecutiva)
- Horizonte: Demo jugable + contenido viral + validacion de comunidad

## 1) High Concept

- Kalex Urban es un videojuego open world narrativo y sandbox urbano inspirado en Lima Metropolitana.
- El MVP no busca competir con AAA; busca demostrar una experiencia autentica, caotica, satirica y emocional.
- Propuesta de valor: "Hacer que Lima se sienta viva" mediante trafico realista, transporte informal, NPCs culturales y eventos urbanos emergentes.
- Resultado esperado: vertical slice jugable que genere clips virales, crecimiento de comunidad y traccion para inversion/colaboracion.

## 2) Objetivos del MVP

- Entregar una demo de 10-20 minutos con alta rejugabilidad emergente.
- Validar 4 pilares:
  - movilidad urbana autentica,
  - trafico limeno creible,
  - densidad social (NPCs y audio),
  - narrativa de supervivencia con tono satirico.
- Producir momentos "clipables" para TikTok/Reels/Shorts.
- Construir base de branding para Kalex Soluciones + visibilidad de Kalex Soluciones.

## 3) Publico Objetivo

- Primario: jugadores de LATAM (18-35) que consumen sandbox, humor social y contenido viral.
- Secundario: creadores de contenido, comunidad gamer peruana/latina, posibles colaboradores de desarrollo.
- Tercero: inversionistas/partners interesados en IP culturalmente diferenciada.

## 4) Fantasia del Jugador

- "Sobrevivir al caos urbano limeno usando calle, ingenio y riesgo."
- Ser parte de una ciudad que reacciona: trafico, cobradores, policia, ambulantes, protestas, zonas calientes.
- Tomar decisiones moralmente grises sin sistema binario bueno/malo.

## 5) Tono y Direccion Creativa

- Tono: satira social + thriller criminal + tragedia humana.
- Mensaje central: ascenso social con herramientas corruptas tiene costo irreversible.
- Evitar: caricatura simplista o burla vacia; el enfoque es critica con autenticidad cultural.
- Promesa estetica: semi-realista, denso, cinematografico, reconocible como Lima.

## 6) Resumen Narrativo (Macro)

- Protagonista: Gabriel "Nero" Ramirez, 22, SJL, precariedad economica.
- Detonante: enfermedad de la abuela + robo del hogar.
- Arco completo (vision futura): supervivencia -> ascenso criminal -> poder politico -> crisis nacional -> caida publica.
- En MVP: se muestra el inicio del Acto 1 con senales tempranas del conflicto moral.

## 7) Alcance del Vertical Slice (MVP)

- Mapa: 1 zona condensada inspirada en SJL (avenida principal + mercado + paraderos + callejon + zona roja ligera).
- Duracion sesion base: 15 minutos (mas variacion por sistemas emergentes).
- Contenido narrativo: intro breve + 2 misiones principales + 3 actividades sistemicas.
- Objetivo de experiencia: que el jugador diga "esto se siente Lima" en los primeros 3 minutos.

## 8) Gameplay Loop (MVP)

- Loop principal:
  1. desplazarte en caos urbano (a pie/combi/mototaxi),
  2. buscar ingresos rapidos (legal/ilegal),
  3. gestionar riesgo social (policia, bandas, reputacion),
  4. gastar recursos (familia/supervivencia),
  5. desbloquear nuevas oportunidades y conflictos.
- Loop emocional:
  - urgencia economica -> decision moral gris -> consecuencia inmediata -> presion creciente.

## 9) Sistemas Jugables Prioritarios (MVP)

- Transporte publico funcional (core):
  - subir/bajar combi,
  - pagar pasaje,
  - rutas cortas,
  - interaccion con cobrador.
- Trafico limeno (core):
  - congestion densa,
  - bloqueos espontaneos,
  - semaforos imperfectos,
  - microchoques/eventos.
- NPCs urbanos (core):
  - ambulantes, pasajeros, cobradores, policia, mototaxistas.
  - dialogo ambiental reactivo (frases contextuales).
- Economia base:
  - 2 trabajos legales cortos,
  - 2 oportunidades ilegales de riesgo.
- Reputacion inicial (sin barra moral clasica):
  - reputacion callejera,
  - atencion policial basica,
  - percepcion barrial simple.

## 10) Misiones MVP (Propuesta)

- Mision 1: "Paradero de Supervivencia"
  - Nero debe reunir dinero urgente trabajando entre cobranzas y carga.
  - Ensena transporte, flujo urbano y presion temporal.
- Mision 2: "Noche en la Avenida"
  - oportunidad ilegal rapida aparece tras evento de robo/caos.
  - Introduce consecuencia social y primera fractura moral.
- Actividades sistemicas:
  - viaje emergente en mototaxi,
  - mini-trabajo de mercado,
  - evento de batida/policia en ruta.

## 11) Direccion de Mundo (Worldbuilding MVP)

- Inspiracion en SJL: cerros, comercio informal, densidad de carteles, cables, transito mixto.
- Sonido como pilar de inmersion:
  - rutas gritadas por cobradores,
  - claxones, radios locales, vendedores.
- Senales culturales diegeticas:
  - grafitis, afiches politicos, menus, anuncios barriales.

## 12) Arte y Audio

- Visual: semi-realista UE5; foco en atmosfera, no hiperfidelidad.
- Iluminacion: horario pico tarde-noche para dramatizar trafico y neon popular.
- Audio: diseno espacial denso; capas por zona (mercado/paradero/avenida).
- UI: minima, funcional, con lenguaje local contextual.

## 13) Stack Tecnologico

- Engine: Unreal Engine 5.5+
- Sistemas: Chaos Vehicles, World Partition (escala futura), PCG (poblacion ambiental), Enhanced Input, Niagara, MetaSounds.
- Implementacion: Blueprints-first para velocidad + C++ en sistemas criticos (trafico/performance/IA base).

## 14) KPIs de Validacion del MVP

- Autenticidad percibida: % de testers que afirman "se siente Lima" (>70% objetivo).
- Retencion sesion demo: tiempo medio >12 min.
- Clipabilidad: numero de momentos compartibles por sesion (meta: 2+).
- Performance objetivo: 1080p estable (prioridad jugabilidad > fidelidad maxima).
- Interaccion sistemica: uso de transporte publico por sesion (>1.5 veces promedio).

## 15) Plan de Produccion (Macro)

- Fase 1 - Preproduccion corta (2-3 semanas):
  - graybox de zona,
  - prototipo de transporte,
  - audio placeholder cultural.
- Fase 2 - Produccion MVP (6-8 semanas):
  - trafico + NPCs + economia base + misiones.
- Fase 3 - Pulido y Captura (2-3 semanas):
  - polish visual/audio,
  - captura de clips,
  - playtests cerrados,
  - build de presentacion.

## 16) Riesgos y Mitigacion

- Riesgo: Scope creep (querer "hacer todo Lima").
  - Mitigacion: regla rigida "1 avenida viva > 1 mapa vacio".
- Riesgo: IA/trafico consume demasiado tiempo.
  - Mitigacion: comportamientos simples pero convincentes + eventos scriptados hibridos.
- Riesgo: tono satirico mal interpretado.
  - Mitigacion: direccion narrativa con enfoque humano, no burla superficial.
- Riesgo: rendimiento.
  - Mitigacion: budgets tecnicos desde inicio (NPC count, LOD, audio layers).

## 17) Estrategia de Go-To-Market (MVP)

- Publicar clips de 15-45s centrados en caos urbano y momentos reconocibles.
- Construir comunidad en Discord/TikTok/YouTube Shorts con updates semanales.
- Abrir formulario de playtest cerrado para early adopters.
- Usar demo como pieza de pitch para inversionistas y reclutamiento tecnico/artistico.

## 18) Definicion de Exito del MVP

- Build jugable estable que comunique identidad unica en minutos.
- Comunidad inicial activa y crecimiento organico en redes.
- Interes tangible de colaboradores o potenciales aliados estrategicos.
- Base solida para pasar a produccion de una version ampliada.
