# Estructura de Contenido UE5 (MVP)

## Convenciones

- Prefijos: `BP_` (Blueprint), `WBP_` (UI), `DA_` (Data Asset), `DT_` (Data Table), `SFX_` (Sound Cue), `M_` (Material), `SM_` (Static Mesh).
- Nombres en PascalCase, sin espacios.

## Arbol recomendado de `/Content/KalexUrban`

```text
KalexUrban/
  Core/
    Blueprints/
    Input/
    GameModes/
    Player/
  World/
    Maps/
    Graybox/
    Props/
    Lighting/
  Systems/
    Missions/
    Traffic/
    Transport/
    NPC/
    Economy/
    Reputation/
  Audio/
    SFX/
    VO/
    Music/
    MetaSounds/
  UI/
    HUD/
    Menus/
  Data/
    DataTables/
    DataAssets/
```

## Minimo a crear en Semana 1

- `Core/Player/BP_KalexCharacter`
- `Core/Player/BP_KalexPlayerController`
- `Core/GameModes/BP_KalexGameMode`
- `World/Maps/L_SJL_Avenida_Graybox`
- `Systems/Missions/BP_MissionManager`
- `Systems/Transport/BP_CombiStop`
- `Data/DataTables/DT_Missions`
