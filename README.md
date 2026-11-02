# Neon Cityscape Parallax (Pebble)

A cyberpunk skyline watchface. PixelLab city layers shift with the accelerometer for a faux-3D parallax effect — far sky moves least, rooftops move most.

## Features

- Three art layers: night sky, neon skyline, rooftop props (PixelLab)
- Accelerometer parallax with Low / Medium / High intensity
- Clay settings on the phone for time, look, and motion
- Neon accent colors on color watches (Cyan / Magenta / Amber)
- Optional invert for black & white displays

## Settings (phone)

Open **Neon Cityscape → Settings** in the Pebble/Rebble app (`@rebble/clay`):

- Hour format / leading zero / date / seconds
- Parallax on/off + intensity
- Neon accent theme
- Invert on B&W

## Build

On Windows (Podman or Docker):

```powershell
./build.ps1
```

Output: `build/NeonCityscapeParallax.pbw`

```powershell
./build.ps1 -Phone 192.168.1.23
./build.ps1 -Zip
```

## Layout

```
src/c/              watchface + accelerometer parallax
src/pkjs/           Clay settings
resources/images/   PixelLab sky / skyline / rooftops + menu icon
tools/              asset prep helpers
```
