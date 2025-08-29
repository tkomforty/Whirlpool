# Whirlpool - Rhythmic Filter Sculptor VST3

**Professional tempo-synced rhythmic filter effect plugin**

Created by **Orcas Audio** | Version 1.0.0

---

## Overview

Whirlpool is a powerful VST3 audio effect plugin designed for creating dynamic, tempo-synchronized rhythmic filtering effects. With its unique multi-band processing approach and intuitive whirlpool-inspired interface, Whirlpool transforms static audio into pulsating, rhythmic textures that breathe life into your productions.

Perfect for electronic music, ambient soundscapes, and any genre where rhythmic movement and textural depth are desired.

## Key Features

### 🎛️ **Advanced Filtering Engine**
- **Multi-band processing** with customizable low/mid/high frequency crossovers
- **Multiple filter types** per band for creative sound shaping
- **Independent resonance control** for each frequency band
- **Smooth parameter automation** with professional-grade audio quality

### 🎵 **Rhythmic Pattern Engine**
- **8 built-in rhythmic patterns** for instant musical results
- **16-step pattern sequencer** with per-step intensity control
- **Tempo synchronization** with host DAW for perfect timing
- **Swing and timing variations** for humanized rhythmic feel

### 🎨 **Intuitive Visual Interface**
- **Custom whirlpool visualization** that responds to audio processing
- **Real-time parameter feedback** with smooth animations
- **Professional dark theme** optimized for studio environments
- **Responsive design** that scales beautifully across different screen sizes

### ⚡ **Professional Controls**
- **Mix/Dry-Wet control** for blending processed and original signals
- **Output gain staging** with precise level control
- **Phase offset per band** for stereo width and movement effects
- **Intensity modulation** for dynamic filtering effects

## System Requirements

### Minimum Requirements
- **Operating System**: Windows 10+ (64-bit), macOS 10.13+, or Linux Ubuntu 18.04+
- **RAM**: 4 GB
- **CPU**: Intel Core i5 or AMD equivalent
- **Plugin Format**: VST3 compatible DAW

### Recommended
- **RAM**: 8 GB or more
- **CPU**: Intel Core i7 or AMD Ryzen 7
- **Audio Interface**: Professional audio interface with ASIO drivers (Windows)

## Installation

### Windows
1. Download the Whirlpool installer from [Releases](https://github.com/tkomforty/Whirlpool/releases)
2. Run the installer as administrator
3. The VST3 plugin will be installed to your standard VST3 directory
4. Restart your DAW and scan for new plugins

### macOS
1. Download the macOS package from [Releases](https://github.com/tkomforty/Whirlpool/releases)
2. Open the .pkg file and follow the installation wizard
3. The plugin will be installed to `/Library/Audio/Plug-Ins/VST3/`
4. Restart your DAW to recognize the new plugin

### Linux
1. Download the Linux build from [Releases](https://github.com/tkomforty/Whirlpool/releases)
2. Extract to your VST3 directory (typically `~/.vst3/`)
3. Ensure proper permissions: `chmod +x Whirlpool.vst3`
4. Restart your DAW

## Building from Source

Whirlpool is built using the JUCE framework. To build from source:

### Prerequisites
- **JUCE Framework 7.0+** - Download from [juce.com](https://juce.com)
- **IDE**: Visual Studio 2022 (Windows), Xcode (macOS), or Code::Blocks/Makefile (Linux)
- **Git** for cloning the repository

### Build Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/tkomforty/Whirlpool.git
   cd Whirlpool
   ```

2. **Open the project:**
   - Open `RhythmicFilterSculptor.jucer` in Projucer
   - Set your JUCE modules path
   - Generate IDE project files for your platform

3. **Build the plugin:**
   
   **Windows (Visual Studio):**
   ```bash
   cd Builds/VisualStudio2022
   # Open Whirlpool.sln in Visual Studio
   # Build in Release configuration
   ```
   
   **macOS (Xcode):**
   ```bash
   cd Builds/MacOSX
   xcodebuild -project Whirlpool.xcodeproj -configuration Release
   ```
   
   **Linux (Makefile):**
   ```bash
   cd Builds/LinuxMakefile
   make CONFIG=Release
   ```

4. **Install the built plugin:**
   - Copy the generated `.vst3` file to your system's VST3 directory

## Controls & Parameters

### Main Interface

| Control | Description | Range |
|---------|-------------|-------|
| **Pattern** | Select from 8 rhythmic patterns | 1-8 |
| **Tempo Sync** | Synchronize with host tempo | Musical divisions |
| **Intensity** | Overall effect intensity | 0-100% |
| **Resonance** | Filter resonance amount | 0-100% |
| **Mix** | Dry/wet signal blend | 0-100% |
| **Output** | Final output level | -24dB to +12dB |

### Advanced Parameters

- **Filter Type**: Choose filter characteristics per frequency band
- **Crossover Frequencies**: Adjust low/mid and mid/high frequency splits
- **Phase Offset**: Create stereo movement and width effects
- **Pattern Steps**: Fine-tune individual step intensities in the 16-step sequencer

### Tips for Best Results

1. **Start with subtle settings** - Whirlpool can be very powerful; begin with lower intensity values
2. **Experiment with different patterns** - Each pattern offers unique rhythmic characteristics
3. **Use tempo sync** - Lock to your DAW's tempo for musical results
4. **Layer multiple instances** - Try different patterns on parallel tracks for complex rhythms
5. **Automate parameters** - Create evolving soundscapes by automating intensity and mix controls

## License

This software is released as **Freeware** under a simple license agreement. You are free to:

- ✅ Use Whirlpool in commercial and non-commercial music productions
- ✅ Share and distribute the plugin with others
- ✅ Create derivative works and remixes using Whirlpool

**Restrictions:**
- ❌ Do not reverse engineer, decompile, or modify the plugin binary
- ❌ Do not claim ownership of the Whirlpool software or brand
- ❌ Distribution must include proper attribution to Orcas Audio

See [LICENSE](LICENSE) file for complete terms.

## Credits

**Whirlpool** is proudly developed by **Orcas Audio**.

### Development Team
- **Audio Engine**: Advanced DSP algorithms and multi-band filtering
- **User Interface**: Custom JUCE-based GUI with whirlpool visualizations  
- **Quality Assurance**: Extensive testing across multiple DAW environments

### Technology Stack
- **Framework**: JUCE 7.x
- **Audio Processing**: Custom DSP with JUCE audio utilities
- **Graphics**: Custom OpenGL-accelerated visualizations
- **Cross-Platform**: Native compilation for Windows, macOS, and Linux

## Support & Community

### Getting Help
- 📧 **Email Support**: [support@orcasaudio.com](mailto:support@orcasaudio.com)
- 🌐 **Website**: [www.orcasaudio.com](https://www.orcasaudio.com)
- 📋 **Bug Reports**: Use GitHub Issues for technical problems
- 💡 **Feature Requests**: Submit enhancement ideas via GitHub Issues

### Stay Connected
- Follow development updates and releases on our website
- Join the community discussions in GitHub Issues
- Share your music using #WhirlpoolVST

### Troubleshooting

**Common Issues:**

1. **Plugin not appearing in DAW:**
   - Verify VST3 is installed in correct directory
   - Rescan plugins in your DAW
   - Check plugin compatibility (64-bit VST3 required)

2. **Audio dropouts or glitches:**
   - Increase audio buffer size in your DAW
   - Disable other CPU-intensive plugins temporarily
   - Check system performance and available RAM

3. **Parameters not responding:**
   - Ensure proper MIDI controller mapping
   - Check automation lanes in your DAW
   - Restart the plugin instance

For additional support, visit our website or contact support directly.

---

**Made with ❤️ by Orcas Audio | Whirlpool v1.0.0**

*Transform your audio with rhythmic precision.*