# MonkSynth VST3

## Build, CI & Release Engineering Specification

This document defines the complete build pipeline, CI setup, versioning
model, artifact packaging, code signing considerations, and release
workflow for MonkSynth.

Goal: Deliver reproducible, stable, cross-platform builds suitable for
public distribution.

------------------------------------------------------------------------

# 1. Target Platforms

## 1.1 Formats

-   VST3 (Primary)
-   AU (macOS optional)

## 1.2 Operating Systems

-   macOS (Intel + Apple Silicon, Universal Binary)
-   Windows (x64)

------------------------------------------------------------------------

# 2. Toolchain Requirements

## 2.1 macOS

-   Xcode (latest stable)
-   macOS SDK matching JUCE recommendation
-   C++17 or later
-   Code signing identity (Developer ID Application)

## 2.2 Windows

-   Visual Studio (latest stable)
-   MSVC toolchain
-   Windows SDK current stable
-   C++17 or later

## 2.3 JUCE

-   Pin to specific JUCE commit or release version
-   Record version in README and release notes

------------------------------------------------------------------------

# 3. Versioning Strategy

Use Semantic Versioning (SemVer):

MAJOR.MINOR.PATCH

Examples: - 1.0.0 (first stable release) - 1.1.0 (new features, backward
compatible) - 1.1.1 (bug fix)

Pre-release tags: - 1.0.0-beta.1 - 1.0.0-rc.1

Store version in: - PluginProcessor constructor - CMakeLists.txt /
Projucer config - Git tag

------------------------------------------------------------------------

# 4. Repository Structure

Root:

/plugin /dsp /ui /midi /tests /docs /.github/workflows

------------------------------------------------------------------------

# 5. CI Pipeline (GitHub Actions Recommended)

## 5.1 Workflow Triggers

-   On push to main
-   On pull request
-   On tag (release build)

## 5.2 Build Matrix

macOS: - macos-latest - Build Universal Binary

Windows: - windows-latest - Build x64 Release

------------------------------------------------------------------------

# 6. CI Steps

For each platform:

1.  Checkout repository
2.  Install dependencies (if needed)
3.  Generate project (CMake or Projucer)
4.  Build Release configuration
5.  Run basic unit tests (if present)
6.  Package artifacts
7.  Upload artifacts

------------------------------------------------------------------------

# 7. Artifact Packaging

## 7.1 macOS

Output: - MonkSynth.vst3 - Optional MonkSynth.component

Package as: - .zip archive OR - .pkg installer (future)

Directory structure inside zip:

/MonkSynth.vst3

Install location (user): \~/Library/Audio/Plug-Ins/VST3/

------------------------------------------------------------------------

## 7.2 Windows

Output: - MonkSynth.vst3

Package as: - .zip archive

Install location (user):
C:`\Program `{=tex}Files`\Common `{=tex}Files`\VST3`{=tex}\

------------------------------------------------------------------------

# 8. Code Signing (macOS)

Before public distribution:

1.  Sign binary: codesign --deep --force --options runtime --sign
    "Developer ID Application: ..." MonkSynth.vst3

2.  Notarize (if distributing outside App Store)

3.  Staple notarization ticket

Acceptance: - Plugin loads without security warnings

------------------------------------------------------------------------

# 9. Build Configurations

## Debug

-   Assertions enabled
-   RT assertions enabled
-   Logging allowed (non-audio thread only)

## Release

-   Optimizations enabled
-   No debug logging
-   No assertions (except critical sanity checks)
-   Strip symbols (optional separate debug symbols)

------------------------------------------------------------------------

# 10. Reproducibility

-   Lock JUCE version
-   Lock compiler version in CI
-   Avoid non-deterministic build scripts
-   Record dependency versions in /docs/BUILD_ENVIRONMENT.md

------------------------------------------------------------------------

# 11. Release Workflow

1.  Complete all QA checks
2.  Increment version number
3.  Update CHANGELOG.md
4.  Create Git tag: vX.Y.Z
5.  Push tag
6.  CI builds release artifacts
7.  Upload artifacts to GitHub Releases
8.  Attach release notes

------------------------------------------------------------------------

# 12. Release Notes Template

Release X.Y.Z

New: - Feature additions

Improved: - Performance or UI improvements

Fixed: - Bug fixes

Known Issues: - Any remaining minor issues

------------------------------------------------------------------------

# 13. Installer (Optional Future)

If creating installer:

macOS: - Signed .pkg installer

Windows: - NSIS or similar installer

Installer must: - Place plugin in correct directory - Not overwrite
unrelated files - Optionally install presets to user folder

------------------------------------------------------------------------

# 14. Preset Packaging

Presets location:

macOS: \~/Music/Audio Music Apps/Presets/MonkSynth/

Windows: Documents`\MonkSynth`{=tex}`\Presets`{=tex}\

Ship: - Factory preset bank (e.g., 20 presets)

Version presets with plugin version.

------------------------------------------------------------------------

# 15. Beta Release Process

For beta versions:

-   Append -beta.X to version
-   Clearly label build as Beta
-   Collect crash reports
-   Use small tester group
-   Fix critical issues before stable tag

------------------------------------------------------------------------

# 16. Final Release Criteria

A version is considered release-ready when:

-   All automated tests pass
-   No memory leaks
-   No RT violations
-   No crashes in host matrix
-   CPU within target range
-   Code signed and notarized (macOS)
-   Version tag applied
-   Release notes complete

------------------------------------------------------------------------

End of Build, CI & Release Engineering Specification
