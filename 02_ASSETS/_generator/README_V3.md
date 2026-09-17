# KALEX URBAN — Building Generator V3

## Uso

1. Ejecutar `powershell/generate_assets.ps1`.
2. Ejecutar `powershell/import_to_unreal.ps1`.
3. Compilar `kalexurbanEditor Win64 Development` con UE 5.6.
4. Abrir `/Game/KalexUrban/Levels/L_SJL_V3_Validation` para revisar semillas 100–103,
   o colocar `BP_LimaBuildingGenerator` desde `/Game/KalexUrban/Procedural`.

`Seed` controla acabados por piso, distribución y fierros. Cambiarlo mantiene las
dimensiones explícitas. El botón `RandomizeBuilding` también deriva pisos, medidas,
garaje y props de esa misma semilla. Repetirlo con la misma semilla es determinista.

Desactivar `bVaryFinishBySeed` permite escoger `FacadeType` explícitamente.
`bOverrideGroundFloorFinish` habilita `GroundFloorFinish`.
`FloorMaterialOverrides` permite asignar cualquier material propio por piso y tiene
prioridad sobre las otras opciones. Los acabados sólo sustituyen el slot de pared:
marcos, rejas, puerta/portón y vidrio mantienen sus materiales.

## Contrato de geometría

- 11 módulos de fachada y 4 piezas estructurales nuevas; sin variantes FBX por color.
- X = ancho, Y = espesor, Z = altura. Origen central en suelo. Módulos de fachada:
  280 cm de alto y 20 cm de espesor.
- Los números de ventana/puerta/garaje indican ancho total del módulo, incluidos
  jambas de 8 cm por lado, no la luz libre.
- Ventana: antepecho de 100 cm y hueco hasta 220 cm. Puerta: hasta 220 cm.
  Garaje: hasta 235 cm. Cierres metálicos y vidrio oscuro dentro del hueco.
- UV0: una unidad por metro. UE genera UV de lightmap por separado.
- UCX independiente por pieza sólida. Una envolvente convexa nunca une toda la fachada.
- Grilla de 50 cm, con pequeña corrección distribuida para medidas fuera de grilla.
  Los laterales y el fondo repiten módulos de hasta 200 cm.
- Los FBX antiguos continúan disponibles por compatibilidad; V3 usa los nuevos módulos.

## Materiales en dos niveles

`materials.json` es la paleta compartida. Blender crea vistas procedurales ligeras:
ladrillo, mayólica con juntas, tarrajeado, concreto, pintura, metal, óxido y vidrio oscuro.
La exportación escribe un manifiesto de slots junto a cada FBX.

UE importa geometría y slots, comprueba el manifiesto y asigna materiales nativos:

```
/Game/KalexUrban/Materials/
  Master/M_Master_Surface
  Master/M_Master_Building
  Master/M_Master_Glass
  Master/M_Master_Decal
  Instances/{Brick,Concrete,Plaster,Paint,Tile,Metal,Glass}/M_SJL_*
  Textures/{Brick,Concrete,Plaster,Tile,Metal}/
```

Los 18 materiales son instancias reutilizables con BaseColor, Roughness y Metallic.
Building agrega Pattern (0 liso, 1 ladrillo, 2 mayólica), con juntas calculadas a partir
de UV métricas. Glass usa un proxy opaco oscuro de baja rugosidad, compatible con Nanite.
Los materiales existentes no se reconstruyen al reimportar; las ediciones artísticas
se conservan. Las carpetas Textures están preparadas, sin texturas PBR externas.
Master_Decal es una base preparada; no se han colocado decals ni máscaras de suciedad.

## Validación

- `blender/validate_facades.py`: reimporta los 11 FBX de fachada y comprueba medidas,
  pivots, densidad UV y ausencia de mampostería en los huecos mediante rayos.
- `unreal/validate_buildings.py`: comprueba importación, materiales, colisiones,
  36 distribuciones, regeneración determinista y semillas 100–103. Genera el nivel
  de revisión y crea el Blueprint sólo si no existe.
- `unreal/capture_validation.py`: captura el nivel con ticks reales del editor.
- Evidencia y copias previas de código en `validation/`.
- El importador exige un marcador de éxito; un error Python interno ya no se
  confunde con éxito aunque Unreal devuelva código de proceso 0.

Los interiores completos, tráfico, RoadSpline V2, texturas finales, suciedad y decals
pertenecen a etapas posteriores.
