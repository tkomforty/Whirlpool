# Contributing to Whirlpool

Thank you for your interest in contributing to **Whirlpool VST3 Plugin**! 🎵

We welcome contributions from the community to help improve this rhythmic filter sculptor. Whether you're reporting bugs, suggesting features, improving documentation, or contributing code, your help is greatly appreciated.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Pull Request Process](#pull-request-process)
- [Development Guidelines](#development-guidelines)
- [Community Guidelines](#community-guidelines)

---

## Code of Conduct

This project adheres to a code of conduct that promotes a welcoming and inclusive environment. By participating, you agree to:

### Our Standards

**Positive behavior includes:**
- ✅ Using welcoming and inclusive language
- ✅ Being respectful of differing viewpoints and experiences
- ✅ Gracefully accepting constructive criticism
- ✅ Focusing on what is best for the community
- ✅ Showing empathy towards other community members

**Unacceptable behavior includes:**
- ❌ Trolling, insulting/derogatory comments, and personal attacks
- ❌ Public or private harassment
- ❌ Publishing others' private information without permission
- ❌ Other conduct which could reasonably be considered inappropriate

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be reported by contacting **support@orcasaudio.com**. All complaints will be reviewed and investigated promptly and fairly.

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **GitHub account** - Required for submitting issues and pull requests
2. **Development environment** (for code contributions):
   - JUCE Framework 7.0+
   - Appropriate IDE (Visual Studio 2022, Xcode, or Code::Blocks)
   - Git for version control
3. **Basic knowledge** of audio plugin development and C++ (for code contributions)

### Setting Up Development Environment

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/your-username/Whirlpool.git
   cd Whirlpool
   ```
3. **Set up JUCE** and configure module paths in Projucer
4. **Build the project** following instructions in [README.md](README.md)

---

## How to Contribute

### 🐛 Bug Reports

Found a bug? Help us fix it!

### 💡 Feature Suggestions

Have an idea for improving Whirlpool? We'd love to hear it!

### 📖 Documentation

Documentation improvements are always welcome, including:
- README updates
- Code comments
- User guides
- Video tutorials

### 💻 Code Contributions

We welcome code contributions for:
- Bug fixes
- Performance improvements
- New features (after discussion)
- Code quality improvements

---

## Reporting Bugs

When reporting bugs, please use the provided issue template and include:

### Required Information

1. **Clear bug description** - What happened vs. what you expected
2. **Steps to reproduce** - Detailed, step-by-step instructions
3. **Environment details**:
   - Operating System (Windows 10/11, macOS version, Linux distro)
   - DAW name and version
   - Plugin version
   - System specifications (CPU, RAM)
4. **Additional context**:
   - Screenshots or screen recordings
   - Audio examples (if relevant)
   - Console/error logs
   - Related plugin settings

### Bug Report Template

```markdown
**Bug Description**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Open DAW '...'
2. Load Whirlpool on track
3. Set parameters to '...'
4. Play audio and observe '...'

**Expected Behavior**
A clear description of what you expected to happen.

**Environment:**
 - OS: [e.g. Windows 11, macOS 12.3, Ubuntu 20.04]
 - DAW: [e.g. Ableton Live 11, Logic Pro X, Reaper 6.5]
 - Plugin Version: [e.g. 1.0.0]
 - System: [e.g. Intel i7-9700K, 16GB RAM]

**Additional Context**
Add any other context about the problem here, including:
- Screenshots
- Audio examples
- Error messages
```

### Priority Levels

- **Critical**: Plugin crashes, data loss, or security issues
- **High**: Major functionality broken, significant performance issues
- **Medium**: Minor functionality issues, cosmetic problems
- **Low**: Enhancement requests, documentation improvements

---

## Suggesting Features

We encourage feature suggestions! Before submitting:

### Before You Suggest

1. **Check existing issues** - Your idea might already be discussed
2. **Consider the scope** - Does it fit Whirlpool's core purpose?
3. **Think about users** - Would this benefit the broader community?

### Feature Request Template

```markdown
**Feature Summary**
A clear and concise description of the feature you'd like to see.

**Problem Statement**
What problem does this feature solve? What workflow would it improve?

**Proposed Solution**
Describe your preferred solution in detail.

**Alternative Solutions**
Describe any alternative solutions or features you've considered.

**Use Cases**
Describe specific scenarios where this feature would be useful:
1. Genre/style: [e.g. techno, ambient]
2. Workflow: [e.g. live performance, studio production]
3. User type: [e.g. beginner, professional]

**Implementation Considerations**
- Performance impact
- User interface changes
- Compatibility concerns
- Learning curve for users

**Additional Context**
Add any other context, screenshots, or examples about the feature request.
```

---

## Pull Request Process

### Before Creating a Pull Request

1. **Discuss major changes** - Open an issue first for significant modifications
2. **Check existing PRs** - Avoid duplicate work
3. **Review contribution guidelines** - Ensure your changes align with project goals

### Pull Request Steps

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b bugfix/issue-number-description
   ```

2. **Make your changes**:
   - Follow coding standards
   - Add appropriate comments
   - Update documentation if needed

3. **Test thoroughly**:
   - Build on your target platform
   - Test with multiple DAWs if possible
   - Verify audio quality and performance

4. **Commit with clear messages**:
   ```bash
   git commit -m "Add: New feature description"
   git commit -m "Fix: Bug description (closes #123)"
   git commit -m "Update: Documentation improvements"
   ```

5. **Push and create PR**:
   ```bash
   git push origin feature/your-feature-name
   ```

### Pull Request Template

```markdown
## Description
Brief description of changes and motivation.

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing
- [ ] Tested on Windows
- [ ] Tested on macOS  
- [ ] Tested on Linux
- [ ] Tested in multiple DAWs
- [ ] Audio quality verified
- [ ] Performance impact assessed

## Checklist
- [ ] My code follows the project's coding standards
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings or errors
- [ ] New and existing tests pass locally

## Related Issues
Closes #(issue number)
```

---

## Development Guidelines

### Coding Standards

#### C++ Code Style
- **Indentation**: 4 spaces (no tabs)
- **Naming conventions**:
  - Classes: `PascalCase` (e.g., `FilterBank`)
  - Functions/methods: `camelCase` (e.g., `processAudio`)
  - Variables: `camelCase` (e.g., `sampleRate`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_FILTER_ORDER`)
- **Bracing**: Opening brace on same line
- **Line length**: Maximum 120 characters
- **Headers**: Include guards or `#pragma once`

#### JUCE-Specific Guidelines
- Use JUCE conventions and idioms
- Leverage JUCE's smart pointers and containers
- Follow JUCE's parameter and component patterns
- Use JUCE's threading and audio processing best practices

### Code Organization

```
Source/
├── DSP/                    # Audio processing algorithms
│   ├── FilterBank.*        # Multi-band filtering
│   ├── PatternSequencer.*  # Rhythm pattern engine
│   └── TempoSync.*         # DAW synchronization
├── GUI/                    # User interface components
│   └── CustomLookAndFeel.* # Visual styling
├── Parameters/             # Plugin parameters
│   └── ParameterDefinitions.* # Parameter setup
├── PluginProcessor.*       # Main plugin class
└── PluginEditor.*          # Main GUI class
```

### Performance Considerations

- **Real-time safety**: No memory allocation in audio thread
- **CPU efficiency**: Profile and optimize hot code paths
- **Memory usage**: Minimize allocations and avoid leaks
- **Thread safety**: Use appropriate synchronization

### Testing Guidelines

- **Build testing**: Verify compilation on all target platforms
- **Functional testing**: Test all parameters and features
- **Performance testing**: Monitor CPU usage and latency
- **Compatibility testing**: Test with popular DAWs
- **Regression testing**: Ensure existing functionality still works

---

## Community Guidelines

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests, and general discussion
- **Email**: [support@orcasaudio.com](mailto:support@orcasaudio.com) for private matters
- **Website**: [www.orcasaudio.com](https://www.orcasaudio.com) for news and updates

### Response Times

- **Bug reports**: We aim to respond within 48 hours
- **Feature requests**: Initial response within 1 week
- **Pull requests**: Review begins within 3-5 business days
- **General questions**: Response within 72 hours

### Recognition

Contributors will be acknowledged in:
- Release notes and changelogs
- Project README contributors section
- Special thanks in significant releases

### Getting Help

Need help contributing? Don't hesitate to:

1. **Ask questions** in GitHub Issues with the "question" label
2. **Request guidance** on implementation approaches
3. **Seek clarification** on project requirements
4. **Contact maintainers** directly for complex issues

---

## Thank You! 🙏

Your contributions help make Whirlpool better for everyone in the audio production community. Whether you're fixing a small typo or implementing a major feature, every contribution is valued and appreciated.

**Happy contributing!** 🎵

---

*Last updated: January 2025*