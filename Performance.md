This is a summary of reported performance of using UEVR with Satisfactory and the UEVR Enhancements mod.

To check your render resolution in UEVR Tools, click both thumbsticks in together to bring up the UEVR Tools settings. Select "Runtime" from the side menu, and it should show the "Render Resolution". 
To show your FPS in UEVR Tools, check "Show Advanced Options", then select "Debug" from the side menu, and check the "Show FPS" option.

For the table below, "Game Size" is a rough estimate of how complex your game save is that you are testing: Small = early tiers, minimal builds, Medium = mid-game, modest builds, Large = late-game, decent builds, Huge = mid to late game with larger builds, Andrew = Mega-builds stretching the capabilities of the engine.

Global Illumination is *off* unless mentioned in the Notes.

| Contributor | Headset       | Connection / Runtime         | CPU / GPU                 | Graphics / Scaling        | Game Size | Render Res,      | FPS | Notes                 |
| ----------- | ------------- | ---------------------------- | ------------------------- | ------------------------- | --------- | ---------------- | --- | --------------------- |
| Dortamur    | Quest 3       | Virtual Desktop <br/> OpenXR | i7-13700KF <br/> RTX 2080 | High<br/>DLSS Perf | Small     | 4032 x 2112      | ~70 |                       |
| Dortamur    | Quest 3       | Virtual Desktop <br/> OpenXR | i7-13700KF <br/> RTX 2080 | High<br/>DLSS Perf | Large     | 4032 x 2112      | ~40 |                       |
| Dortamur    | Quest 3       | SteamLink       <br/> OpenVR | i7-13700KF <br/> RTX 2080 | High<br/>DLSS Perf | Large     |                  |     |                       |
| Dortamur    | Rift S        | Wired/Oculus    <br/> OpenVR | i7-13700KF <br/> RTX 2080 | High<br/>DLSS Perf | Large     |                  |     |                       |
| KroyVR      | Index         | Wired           <br/> OpenVR | 5950x      <br/> RTX 4090 | High<br/>DLSS Perf | Small     | 2016 x 2240 /eye | ~90 | Multiplayer (as host) |
| xmccaryx    | Pimax Crystal | Wired<br>OpenXR              | 7800x3D<br>RTX 4090       | Low-Med<br>**none**       | Small     | 4312x5102 /eye   | ~55 |                       |
## Game Engine Settings

Submitted by tester **xmccaryx**, adding these settings to your Satisfactory config should give clearer visuals and better performance. Feel free to turn back on effects you personally like (although this may affect FPS).
- Find your Satisfactory settings under Local App Data: `%LocalAppData%\FactoryGame\Saved\Config\Windows`
- Make a backup copy of the file `engine.ini`
- Edit `engine.ini` and make the following changes:
```ini
[SystemSettings]
r.FastBlurThreshold=0
r.Tonemapper.GrainQuantization=0
r.Tonemapper.Quality=0
r.SceneColorFringeQuality=0
r.DepthOfFieldQuality=0
r.DisableDistortion=1
r.BloomQuality=0
r.FilmGrain=0
r.SubsurfaceQuality=0
r.LensFlareQuality=0
r.MotionBlurQuality=0

[/Script/Engine.RendererSetings]
r.Streaming.Boost=1
r.Streaming.PoolSize=0
r.Streaming.LimitPoolSizeToVRAM=1
```
