# Texture Based IsoSurface Rendering

## build & install

please pre install `freeglut` before build by cmake.

Tested by `NMake` on `Windows 11`, `MSVC 142 (Visual Studio 19)`, `freeglut` can be installed by `vcpkg`.

## some changes

Set shader files path absolutely may cause less problems.

## run

```powershell
isorender.exe  C:\Users\murez\CG_Final\flame.bin float
isorender.exe  C:\Users\murez\CG_Final\cthead.bin uchar
```

 ```
 [t] --- open/close transfer function editor
 [p] --- change btween 1D / 2D PreIntegration
 [-] --- decrease render step
 [+/=] - increase render step
 ```

