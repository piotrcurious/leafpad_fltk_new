# Memory-Optimized leafpad_fltk (No FLTK Recompile Required)

## Quick Start

1. **Install minimal fonts:**
```bash
sudo apt-get install xfonts-unifont xfonts-base xfonts-75dpi xfonts-100dpi
xset fp rehash
```

2. **Build the patched version:**
```bash
make clean
make
```

3. **Run with wrapper (recommended):**
```bash
./launch_wrapper.sh
```

Or run directly:
```bash
./leafpad_fltk
```

## How It Works

This solution forces FLTK to use X11 core fonts instead of Xft/fontconfig/FreeType:

1. **Font Name Format:** Uses XLFD (X Logical Font Description) format like:
   - `-misc-fixed-medium-r-normal--14-*-*-*-*-*-iso10646-1`
   - This format forces X11 core font system

2. **Early Initialization:** Sets font mapping before FLTK creates any windows

3. **Environment Control:** Launch wrapper prevents accidental loading of heavy libraries

## Expected Results

- **Memory Savings:** 5-10MB RSS reduction
- **Performance:** Faster startup (no font cache parsing)
- **Unicode Support:** Basic coverage with unifont

## Font Options

Edit `main.cpp` in `setup_minimal_fonts()`:

```cpp
// Option 1: Minimal memory (standard fixed font)
const char* font = get_fixed_font(14);

// Option 2: Better Unicode (requires unifont package)
const char* font = get_unifont(16);
```

## Verification

Check that X11 fonts are being used:
```bash
xlsfonts | grep fixed
xlsfonts | grep unifont
```

## Troubleshooting

**Text not visible:**
- Verify fonts installed: `dpkg -l | grep xfonts`
- Refresh font cache: `xset fp rehash`
- Check X11 font path: `xset q | grep "Font Path"`

**Still high memory:**
- Use the launch wrapper: `./launch_wrapper.sh`
- Check `ldd ./leafpad_fltk` - if it shows libfontconfig, environment isn't set early enough
- Monitor with: `ps aux | grep leafpad_fltk` or `pmap $(pidof leafpad_fltk)`

**Fallback if nothing works:**
Use the wrapper script and set additional environment variables before launch.
