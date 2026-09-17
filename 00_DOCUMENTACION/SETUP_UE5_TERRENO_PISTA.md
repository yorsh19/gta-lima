# Setup rapido UE5 - Terreno y pista KalexUrban

Este documento deja un flujo rapido para generar el blockout de terreno y avenida en el proyecto.

## Archivo implementado

- Script Python: `01_PROYECTO_UNREAL/kalexurban/Content/Python/build_kalex_avenue_blockout.py`

## Que construye el script

- Base de terreno modular de ~2.1 km x 2.1 km (proxies).
- Avenida principal de ~700 m x 24 m.
- Veredas a ambos lados.
- Separador central.
- Fila de barreras temporales en carril derecho.
- Bloques proxy de edificios a ambos lados para fijar escala urbana.

## Como ejecutarlo en Unreal Engine 5

1. Abrir el proyecto `kalexurban.uproject`.
2. Activar plugin de Python si aun no esta activo:
   - `Edit > Plugins > Scripting > Python Editor Script Plugin`.
3. Reiniciar el editor si UE5 lo solicita.
4. Abrir el nivel donde quieras generar la escena (recomendado: crear uno nuevo para entorno urbano).
5. Abrir `Window > Developer Tools > Output Log`.
6. En la consola de Python ejecutar:

```python
import build_kalex_avenue_blockout as kalex
kalex.build_scene(clear_existing=True)
```

## Notas

- El script etiqueta todo con `KalexUrban_Blockout`.
- Si lo vuelves a correr con `clear_existing=True`, borra el blockout anterior y crea uno nuevo.
- Es una base de trabajo (proxies). El siguiente paso es reemplazar por:
  - Landscape real esculpido
  - Materiales de asfalto/tierra
  - Bordillos, postes, cableado, assets de mercado/casas
